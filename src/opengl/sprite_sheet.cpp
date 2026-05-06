#include "sprite_sheet.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../dependancies/stb_image.h"

#include <fstream>
#include <filesystem>


static std::pair<const char*, int> ParameterValues[] = {
	{"repeat",				GL_REPEAT},
	{"mirrored_repeat",		GL_MIRRORED_REPEAT},
	{"clamp_to_edge",		GL_CLAMP_TO_EDGE},
	{"clamp_to_border",		GL_CLAMP_TO_BORDER}
};


glm::vec2 SpriteSheet::GetSpriteDimensions() const {
	return { m_SpriteUniformUVs.u1, m_SpriteUniformUVs.v1 };
}


glm::vec2 SpriteSheet::GetCalculatedSpriteOffsets(
	int _spriteIndex
) const {
	DEBUG_ASSERT(_spriteIndex >= 0 && _spriteIndex < m_SpriteCountPerCol * m_SpriteCountPerCol, "Sprite index out of bounds.");
	
	int xCoord = _spriteIndex % m_SpriteCountPerRow;
	int yCoord = _spriteIndex / m_SpriteCountPerRow;

	return { xCoord * m_SpriteUniformUVs.u1, yCoord * m_SpriteUniformUVs.v1 };
}


SpriteSheet::SpriteSheet(
	const std::string& _locationOfImageOrConfigFile,
	const std::string& _sheetName,
	const Shader* _preferredShader,
	int _spritesPerRow_IGNORED_IF_LOADING_CONFIG_FILE,
	int _spritesPerCol_IGNORED_IF_LOADING_CONFIG_FILE,
	int paddingPx
) 
	: 
		m_SheetName(_sheetName),
		m_Shader(_preferredShader)
{

	int ReturnCode = DetermineLoadingMethodFromGivenPath(_locationOfImageOrConfigFile);
	DEBUG_ASSERT(ReturnCode != c_ErrorInLoadingMethodReturnCode,
		"Incorrect file type set for sheet with name [%s]. Must have an extension .png or .cfg to work.", GetName().c_str());

	if (ReturnCode == c_StandardImageLoadingMethodReturnCode) {
		StandardImageLoadingMethod(
			_locationOfImageOrConfigFile.c_str(),
			_spritesPerRow_IGNORED_IF_LOADING_CONFIG_FILE,
			_spritesPerCol_IGNORED_IF_LOADING_CONFIG_FILE,
			paddingPx
		);
	}
	else if (ReturnCode == c_ConfigFileLoadingMethodReturnCode) {
		ConfigurationPairLoadingMethod(
			_locationOfImageOrConfigFile.c_str()
		);
	}
}


void SpriteSheet::TransformIndicesToUVRegionArray(
	const int* _indexArray,
	const int _indexArraySize,
	std::vector<UVRegion>& OUT_uvRegionArray,
	std::vector<float>* OUT_vertexArray
) const {

	DEBUG_ASSERT(_indexArray != nullptr, "Nullptr sent to TransformIndicesToUVregionArray() in sprite sheet with name [%s].", GetName().c_str());
	DEBUG_ASSERT(_indexArraySize >= 0, "0 length array passed to TransformIndicesToUVregionArray() in sprite sheet with name [%s].", GetName().c_str());

	OUT_uvRegionArray.clear();


	if (OUT_uvRegionArray.capacity() < _indexArraySize) {
		OUT_uvRegionArray.reserve(_indexArraySize);
	}

	OUT_uvRegionArray.resize(_indexArraySize);

	if (OUT_vertexArray) OUT_vertexArray->resize(static_cast<size_t>(_indexArraySize) * 8);

	const UVRegion* UVRegionArray = GetUVRegionArray().data();
	for (size_t i = 0; i < static_cast<int>(_indexArraySize); i++) {
		
		const int SpriteIndex = _indexArray[i];

		DEBUG_ASSERT(SpriteIndex < m_UVregionsFromConfigFile.size(), "Indexing out of bounds in UVRegion array in sprite sheet with name [%s].", GetName().c_str());

		OUT_uvRegionArray[i] = UVRegionArray[SpriteIndex];
		
		if (OUT_vertexArray) {

			float* VertexArray = OUT_vertexArray->data() + i * 8;

			float 
				u0 = UVRegionArray[SpriteIndex].u0,
				v0 = UVRegionArray[SpriteIndex].v0,
				u1 = UVRegionArray[SpriteIndex].u1,
				v1 = UVRegionArray[SpriteIndex].v1;

			u0 *=  m_SheetWidth;
			u1 *=  m_SheetWidth;

			v0 *=  m_SheetHeight;
			v1 *=  m_SheetHeight;

			float width = u1 - u0;
			float height = v1 - v0;

			float left =  - width / 2;
			float right = width / 2;
			float top = - height / 2;
			float bottom = height / 2;

			VertexArray[0] = left;		//1
			VertexArray[1] = top;
			VertexArray[2] = left;		//2
			VertexArray[3] = bottom;
			VertexArray[4] = right;	//3
			VertexArray[5] = bottom;
			VertexArray[6] = right;	//4
			VertexArray[7] = top;
		}
	}

}


int SpriteSheet::DetermineLoadingMethodFromGivenPath(
	const std::string& _pathFromConstructor
) {
	std::string FileExtention;
	
	int FirstDotFromEndToBeginning = static_cast<int>(_pathFromConstructor.rfind('.'));
	int FirstSlash = static_cast<int>(_pathFromConstructor.rfind('/'));
	int FirstBackslash = static_cast<int>(_pathFromConstructor.rfind('\\'));

	
	if (FirstSlash > FirstDotFromEndToBeginning
		|| FirstBackslash > FirstDotFromEndToBeginning) {
		DEBUG_ASSERT(0, "Strange file path given to sheet with name [%s].", GetName().c_str());
		m_Type = SpriteSheetType::ErrorLoading;
		return c_ErrorInLoadingMethodReturnCode;
	}

	FileExtention = _pathFromConstructor.substr(FirstDotFromEndToBeginning,
		_pathFromConstructor.length() - FirstDotFromEndToBeginning);


	if (!FileExtention.compare(".png")) {
		m_Type = SpriteSheetType::IndexBased;
		return c_StandardImageLoadingMethodReturnCode;
	}
	else if (!FileExtention.compare(".cfg")) {
		m_Type = SpriteSheetType::DirectUV;
		return c_ConfigFileLoadingMethodReturnCode;
	}
	
	m_Type = SpriteSheetType::ErrorLoading;
	return c_ErrorInLoadingMethodReturnCode;
}


void SpriteSheet::ConfigurationPairLoadingMethod(
	const char* _pathToConfigFile
) {
	std::fstream File;
	File.open(_pathToConfigFile, std::ios::in);

	DEBUG_ASSERT(File.is_open(), "[%s] > File couldn't be opened.", _pathToConfigFile);


	std::string Line;
	Line.reserve(200);
	
	std::getline(File, Line);


	size_t FirstSlash = Line.find('\"');
	size_t LastSlash = Line.rfind('\"');

	DEBUG_ASSERT(
		Line.find("relpath=") != std::string::npos ||
		FirstSlash != std::string::npos ||
		LastSlash != std::string::npos, 
		"[%s] > Configuration file malformed.", _pathToConfigFile);
	
	
	std::string TextureImagePath = Line.substr(
		FirstSlash + 1,
		LastSlash - FirstSlash - 1
	);

	std::getline(File, Line);
	InterpretTextureParametersString(Line.substr(Line.find('\"') + 1, Line.rfind('\"') - Line.find('\"') - 1));

	LoadImageInTexture(TextureImagePath.c_str());

	std::string AssetName;
	AssetName.resize(40, '\0');
	while (std::getline(File, Line)) {

		if (Line.empty()) continue;

		DEBUG_ASSERT(Line.length() > 1, "[%s]> Strange line found inside configuration file.", _pathToConfigFile);

		if (!Line.substr(0, 2).compare("//")) continue;


		if (sscanf(Line.c_str(), "[%100[^]]]", AssetName.data()) == 1) {
			m_UVregionNamesFromConfigFile.emplace_back(Line.substr(1, Line.size() - 2));
			continue;
		}

		int xOffset, yOffset, AssetWidth, AssetHeight;
		int Result = sscanf(Line.c_str(), "x%dy%dw%dh%d", &xOffset, &yOffset, &AssetWidth, &AssetHeight);
		DEBUG_ASSERT( Result == 4, "[%s] > Configuration file malformed.", _pathToConfigFile);

		if (Result != 4) {
			continue;
		}

		m_UVregionsFromConfigFile.emplace_back(
			xOffset / static_cast<float>(m_SheetWidth),
			yOffset / static_cast<float>(m_SheetHeight),
			(xOffset + AssetWidth) / static_cast<float>(m_SheetWidth),
			(yOffset + AssetHeight) / static_cast<float>(m_SheetHeight)
		);

		const xyDimensions dims = {
			.x = static_cast<float>(AssetWidth),
			.y = static_cast<float>(AssetHeight)
		};

		m_SpriteDimensionsPx.emplace_back(dims);
	}
}


void SpriteSheet::StandardImageLoadingMethod(
	const char* _pathFromConstructor,
	int _spritesPerRow,
	int _spritesPerCol,
	int paddingPx
) {

	m_SpriteCountPerRow = _spritesPerRow;
	m_SpriteCountPerCol = _spritesPerCol;
	m_PaddingPx = paddingPx;

	m_TexParams.S_WrapMode = GL_CLAMP_TO_EDGE;
	m_TexParams.T_WrapMode = GL_CLAMP_TO_EDGE;

	LoadImageInTexture(_pathFromConstructor);

	float assetWidth = float(m_SheetWidth) / float(m_SpriteCountPerRow);
	float assetHeight = float(m_SheetHeight) / float(m_SpriteCountPerCol);

	auto WriteData = [this, assetWidth, assetHeight](int x, int y) {
		float xTotalOffsetPx = x * assetWidth;
		float yTotalOffsetPx = y * assetHeight;

		m_UVregionsFromConfigFile.emplace_back(
			(xTotalOffsetPx + m_PaddingPx) / float(m_SheetWidth),
			(yTotalOffsetPx + m_PaddingPx) / float(m_SheetHeight),
			(xTotalOffsetPx - m_PaddingPx + assetWidth) / float(m_SheetWidth),
			(yTotalOffsetPx - m_PaddingPx + assetHeight) / float(m_SheetHeight)
		);

		xyDimensions dims = {
			.x = assetWidth - 2 * m_PaddingPx,
			.y = assetHeight - 2 * m_PaddingPx
		};

		m_SpriteDimensionsPx.emplace_back(dims);
	};

	for (int y = 0; y < m_SpriteCountPerCol; y++)
		for (int x = 0; x < m_SpriteCountPerRow; x++)
			WriteData(x, y);
}


void SpriteSheet::LoadImageInTexture(
	const char* _pathToFile
) {
	unsigned char* imageData = nullptr;
	int nChannels;
	imageData = stbi_load(
		_pathToFile,
		&m_SheetWidth,
		&m_SheetHeight,
		&nChannels,
		4
	);

	if (!imageData) {
		// unfixable, crash here
		std::cout << stbi_failure_reason();
		throw std::exception(stbi_failure_reason());
	}

	glGenTextures(1, &m_TextureBufferID);
	glBindTexture(GL_TEXTURE_2D, m_TextureBufferID);

	SetTextureParametersToGL();

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_SheetWidth, m_SheetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void SpriteSheet::SetTextureParametersToGL() {

	const TextureParamsDataStruct& Params = GetTexParams();

	if (Params.S_WrapMode == -1) {
		// just in case
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Params.S_WrapMode);

	if (Params.T_WrapMode == -1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Params.T_WrapMode);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


void SpriteSheet::InterpretTextureParametersString(
	const std::string& _texParamsLine
) {
	//	params="S__wrapmode__T_wrapmode__+something else in the future+"
	//	params="S repeat T repeat"
	//	Spaces ignored, parameters after S/T related to those dimensions

	enum class Attributes {
		S_WrapMode = 1,
		T_WrapMode = 2,

		Nothing = -1
	};

	Attributes BoundAttribute = Attributes::Nothing;
	
	TextureParamsDataStruct& Params = m_TexParams;

	for (size_t i = 0; i < _texParamsLine.size(); i++) {
		char Character = _texParamsLine[i];

		if (Character == 'S') {
			BoundAttribute = Attributes::S_WrapMode;
			continue;
		}

		if (Character == 'T') {
			BoundAttribute = Attributes::T_WrapMode;
			continue;
		}

		if (Character == ' ') {
			continue;
		}

		// At this point, we should be @ first char of a parameter value

		DEBUG_ASSERT(BoundAttribute != Attributes::Nothing, "No attribute to apply parameters to in config file for sheet [%s].", GetName().c_str());

		bool ParamValueIsFound = false;
		for (size_t j = 0; j < sizeof(ParameterValues) / sizeof(std::pair<const char*, int>) && !ParamValueIsFound; j++) {
			size_t ParamNameLength = strlen(ParameterValues[j].first);	// doesn't include \0
			if (_texParamsLine.size() < i + ParamNameLength) continue;

			//	case insensitive cmp
			if (!memcmp(_texParamsLine.data() + i, ParameterValues[j].first, ParamNameLength)) {
				if (BoundAttribute == Attributes::S_WrapMode) {
					Params.S_WrapMode = ParameterValues[j].second;
				}

				if (BoundAttribute == Attributes::T_WrapMode) {
					Params.T_WrapMode = ParameterValues[j].second;
				}

				ParamValueIsFound = true;
				i += ParamNameLength - 1;	// account for i++ from for loop
			}
		}

		if (ParamValueIsFound) continue;

		DEBUG_ASSERT(0, "Strange parameter value passed to sheet [%s]", GetName().c_str());
		return;
	}
}


unsigned short SpriteSheet::GetSpriteIndexByName(
	const char* _spriteName
) const {

	for (size_t i = 0; i < m_UVregionNamesFromConfigFile.size(); i++) {

		if (FastStringCompare(m_UVregionNamesFromConfigFile[i].c_str(), _spriteName)) {
			return static_cast<unsigned short>(i);
		}
	}

	return gc_ui16ErrorCode;
}



const UVRegion* SpriteSheet::GetUVRegionByName(
	const char* _assetName
) const {

	for (size_t i = 0; i < m_UVregionNamesFromConfigFile.size(); i++) {

		if (!strcmp(_assetName, m_UVregionNamesFromConfigFile[i].data())) {
			return &m_UVregionsFromConfigFile[i];
		}
	}
	return nullptr;
}


void SpriteSheet::DestroyGLTextureObject() { glDeleteTextures(1, &m_TextureBufferID); }
const std::string& SpriteSheet::GetName() const { return m_SheetName; }
SpriteSheet::SpriteSheet() {}


UVRegion::UVRegion(
	float _u0, float _v0,
	float _u1, float _v1
) : 
	u0(_u0), v0(_v0),
	u1(_u1), v1(_v1)
{}


bool UVRegion::operator==(const UVRegion& other) const {	
	return fEqual(u0, other.u0)
		&& fEqual(v0, other.v0)
		&& fEqual(u1, other.u1)
		&& fEqual(v1, other.v1);
}


SpriteInstance SpriteSheet::GetSpriteInstance(
	const char* spriteName,
	size_t sheetIndex
) const {
	DEBUG_ASSERT(m_UVregionNamesFromConfigFile.size() == m_SpriteDimensionsPx.size(),
		"Sprite sheet [%s] has data corruption. Sprite names and sprite pxDimension arrays have differing sizes.", m_SheetName.c_str());

	for (size_t i = 0; i < m_SpriteDimensionsPx.size(); i++) {
		if (!m_UVregionNamesFromConfigFile[i].compare(spriteName)) {
			SpriteInstance si;
			si.SpriteInfo = SpriteInformation(sheetIndex, i);
			si.dimensions.x = m_SpriteDimensionsPx[i].x;
			si.dimensions.y = m_SpriteDimensionsPx[i].y;
			
			return si;
		}
	}

	SpriteInstance badRetVal;
	return badRetVal;
}


void SpriteSheet::GetSpriteInstances(
	std::vector<SpriteInstance>& OUT_spriteArray,
	size_t sheetIndex
) const {
	DEBUG_ASSERT(m_UVregionNamesFromConfigFile.size() == m_SpriteDimensionsPx.size(),
		"Sprite sheet [%s] has data corruption. Sprite names and sprite pxDimension arrays have differing sizes.", m_SheetName.c_str());
	
	OUT_spriteArray.clear();

	for (size_t i = 0; i < m_SpriteDimensionsPx.size(); i++) {
		SpriteInstance si;
		si.SpriteInfo = SpriteInformation(sheetIndex, i);
		si.dimensions.x = m_SpriteDimensionsPx[i].x;
		si.dimensions.y = m_SpriteDimensionsPx[i].y;

		OUT_spriteArray.emplace_back(std::move(si));
	}
}

SpriteInstance SpriteSheet::GetSpriteInstanceByIndex(
	size_t index,
	size_t sheetIndex
) const {
	DEBUG_ASSERT(index < m_SpriteDimensionsPx.size(),
		"SpriteSheet [%s]: index out of bounds (%zu / %zu)",
		m_SheetName.c_str(), index, m_SpriteDimensionsPx.size());

	SpriteInstance si;
	si.SpriteInfo = SpriteInformation(sheetIndex, index);
	si.dimensions.x = m_SpriteDimensionsPx[index].x;
	si.dimensions.y = m_SpriteDimensionsPx[index].y;

	return si;
}

SpriteInstance SpriteSheet::GetSpriteInstanceByGrid(
	size_t x,
	size_t y,
	size_t sheetIndex
) const {
	DEBUG_ASSERT(x < m_SpriteCountPerRow && y < m_SpriteCountPerCol,
		"SpriteSheet [%s]: grid coords out of bounds (%zu, %zu)",
		m_SheetName.c_str(), x, y);

	size_t index = y * m_SpriteCountPerRow + x;
	return GetSpriteInstanceByIndex(index, sheetIndex);
}

void SpriteSheet::GetAllSpriteInstances(
	std::vector<SpriteInstance>& OUT_spriteArray,
	size_t sheetIndex
) const {
	OUT_spriteArray.clear();
	OUT_spriteArray.reserve(m_SpriteDimensionsPx.size());

	for (size_t i = 0; i < m_SpriteDimensionsPx.size(); i++) {
		SpriteInstance si;
		si.SpriteInfo = SpriteInformation(sheetIndex, i);
		si.dimensions.x = m_SpriteDimensionsPx[i].x;
		si.dimensions.y = m_SpriteDimensionsPx[i].y;

		OUT_spriteArray.emplace_back(std::move(si));
	}
}