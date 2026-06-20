#include "sprite_sheet.h"

#include <fstream>
#include <filesystem>

#include <stb_image.h>

static std::pair<const char*, int> ParameterValues[] = {
	{"repeat",				GL_REPEAT},
	{"mirrored_repeat",		GL_MIRRORED_REPEAT},
	{"clamp_to_edge",		GL_CLAMP_TO_EDGE},
	{"clamp_to_border",		GL_CLAMP_TO_BORDER}
};

static std::pair<const char*, int> FilterValues[] = {
	{"linear",					GL_LINEAR},
	{"nearest",					GL_NEAREST},
	{"linear_mipmap_linear",	GL_LINEAR_MIPMAP_LINEAR }
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
	DEBUG_ASSERT(0, "bad spritesheet ctor called");
}

unsigned short SpriteSheet::GetSpriteIndexByName(
	const char* _spriteName
) const {
	DEBUG_ASSERT(_spriteName, "Nullptr passed for sprite name.");
	for (size_t i = 0; i < m_UVregionNamesFromConfigFile.size(); i++) {

		if (!strcmp(m_UVregionNamesFromConfigFile[i].c_str(), _spriteName)) {
			return static_cast<unsigned short>(i);
		}
	}

	return gc_ui16ErrorCode;
}

const UVRegion* SpriteSheet::GetUVRegionByName(
	const char* _assetName
) const {
	DEBUG_ASSERT(_assetName, "Nullptr passed for asset name.");
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

void SpriteSheet::GenerateTexture(
	const SpriteSheetDefinition& definition
) {
	const ImageFile& image = definition.image;
	m_SheetWidth = image.widthPx;
	m_SheetHeight = image.heightPx;

	glGenTextures(1, &m_TextureBufferID);
	glBindTexture(GL_TEXTURE_2D, m_TextureBufferID);

	//	TODO:	MAKE THIS MORE FLEXIBLE FOR THE FORMATS
	//	I.E. MAKE A TABLE WITH FORMATS, FLAGS FOR THIS FUNCTION AND A SPECIAL FIELD IN ImageFIle
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_SheetWidth, m_SheetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int filterFlag = GL_LINEAR;
	for (const auto& filterKv : FilterValues) {
		if (!strcmp(filterKv.first, definition.filter.c_str())) {
			filterFlag = filterKv.second;
			break;
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterFlag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterFlag);

	glBindTexture(GL_TEXTURE_2D, 0);
	CheckGLErrors();
}

SpriteSheet::SpriteSheet(
	const SpriteSheetDefinition& definition
) {
	bool badConfig = false;
	badConfig = !((definition.rowCount == 0 && definition.colCount == 0) || (definition.rowCount > 0 && definition.colCount > 0));

	DEBUG_ASSERT(badConfig == false, "Sprite sheet with name [%s] has bad configuration.", definition.sheetName.c_str());
	
	m_SheetName = definition.sheetName;
	m_Shader = definition.shader;

	GenerateTexture(definition);

	bool method = definition.rowCount == 0;

	if (method) {
		SpriteListMethod(definition);
	}
	else {
		RowColMethod(definition);
	}
}

void SpriteSheet::SpriteListMethod(
	const SpriteSheetDefinition& definition
) {
	float fWidth = static_cast<float>(m_SheetWidth);
	float fHeight = static_cast<float>(m_SheetHeight);
	const auto& spriteList = definition.sprites;

	for (size_t i = 0; i < spriteList.size(); i++) {
		m_UVregionNamesFromConfigFile.emplace_back(spriteList[i].name);

		float xOffset  = spriteList[i].x;
		float yOffset  = spriteList[i].y;
		float AssetWidth = spriteList[i].w; 
		float AssetHeight = spriteList[i].h;

		m_UVregionsFromConfigFile.emplace_back(
			xOffset / fWidth,
			yOffset / fHeight,
			(xOffset + AssetWidth) / fWidth,
			(yOffset + AssetHeight) / fHeight
		);

		const xyDimensions dims = {
			.x = static_cast<float>(AssetWidth),
			.y = static_cast<float>(AssetHeight)
		};

		m_SpriteDimensionsPx.emplace_back(dims);
	}
}

void SpriteSheet::RowColMethod(
	const SpriteSheetDefinition& definition
) {
	m_SpriteCountPerRow = definition.rowCount;
	m_SpriteCountPerCol = definition.colCount;
	m_PaddingPx = definition.padding;

	float assetWidth = float(m_SheetWidth) / float(m_SpriteCountPerRow);
	float assetHeight = float(m_SheetHeight) / float(m_SpriteCountPerCol);

	auto WriteData = [this, assetWidth, assetHeight](int x, int y) {
		float xTotalOffsetPx = x * assetWidth;
		float yTotalOffsetPx = y * assetHeight;

		float fWidth = float(m_SheetWidth);
		float fHeight = float(m_SheetHeight);

		m_UVregionsFromConfigFile.emplace_back(
			(xTotalOffsetPx + m_PaddingPx) / fWidth,
			(yTotalOffsetPx + m_PaddingPx) / fHeight,
			(xTotalOffsetPx - m_PaddingPx + assetWidth) / fWidth,
			(yTotalOffsetPx - m_PaddingPx + assetHeight) / fHeight
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

SpriteSheet::~SpriteSheet() {
	if (m_TextureBufferID) {
		glDeleteTextures(1, &m_TextureBufferID);
	}
}