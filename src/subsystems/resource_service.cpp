#include "resource_service.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static const char* g_DefaultFontName = "cyrillic";

const SpriteSheet* ResourceService::GetSpriteSheetByName(
	const char* _spriteSheetName
) {
	if (!_spriteSheetName) return nullptr;
	size_t len = m_Sheets.size();
	for (size_t i = 0; i < len; i++) {
		if (
			strcmp(_spriteSheetName, m_Sheets[i].GetName().c_str()) == 0
			) {
			return &m_Sheets[i];
		}
	}
	DEBUG_WARN(0, "GetSpriteSheetByName() for name [%s] returned nullptr.", _spriteSheetName);
	return nullptr;
}

const Shader* ResourceService::GetShaderByName(
	const char* _shaderName
) {
	if (!_shaderName) return nullptr;
	size_t len = m_Shaders.size();
	for (size_t i = 0; i < len; i++) {
		if (
			strcmp(_shaderName, m_Shaders[i].GetName().c_str()) == 0
			) {
			return &m_Shaders[i];
		}
	}
	DEBUG_WARN(0, "GetShaderByName() for name [%s] returned nullptr.", _shaderName);
	return nullptr;
}

void ResourceService::UploadShaderParameters(
	const char* _location,
	const char* _shaderName
) {
	m_ShaderLoadQueue.emplace_back(_location, _shaderName);
}

void ResourceService::UploadSpriteSheetParameters(
	const char* location,
	bool loadInUIsheet
) {
	m_SpriteSheetLoadQueue.emplace_back(
		location,
		loadInUIsheet
	);
}

void ResourceService::StartLoadingProcess() {
	LoadShaders();
	LoadSpriteSheets();
	LoadFonts();
	LoadDefaultVariables();
}

void ResourceService::LoadDefaultVariables() {
	if (m_Fonts.size()) {
		m_DefaultTextOptions.font = &m_Fonts[0];
	}

	m_UIBatch.InitialiseBuffers();
	m_UIBatch.BufferUBOs();

	auto defSkin = std::make_unique<NineSliceBgSkin>("default");
	defSkin.get()->cornerLengthPx = 20;
	for (int i = 0; i < 9; i++) {
		defSkin.get()->instanceArray[i] = m_UIBatch.GetSprite(c_SpecialUISheetName, NineSliceBgSkin::DEFAULT_BG_SUBSPRITE_NAMES[i]);
	}
	AddBgSkin(std::move(defSkin));
	
	auto closeBtnSkin = std::make_unique<ImageBgSkin>("closeBtnSkin");
	closeBtnSkin.get()->imageInstance = m_UIBatch.GetSprite(c_SpecialUISheetName, "closeBtn");
	AddBgSkin(std::move(closeBtnSkin));


	m_bgCloseBtnSkin = GetBgSkinByName("closeBtnSkin");
	
	m_CaretSprite = m_UIBatch.GetSprite(c_SpecialUISheetName, "caret_pixel");
	m_CaretSprite.dimensions.x = 1;
}

const BackgroundSkinInterface* ResourceService::GetBgSkinByName(
	const char* _name
) const {
	size_t paneCount = m_BgSkins.size();
	if (paneCount == 0) {
		DEBUG_ASSERT(0, "GetBgSkinByName() called when no skins were loaded.");
		return nullptr;
	}

	if (!_name) {
		return m_BgSkins[0].get();
	}

	for (size_t i = 0; i < paneCount; i++) {
		if (
			strcmp(_name, m_BgSkins[i].get()->name.c_str()) == 0
			) {
			return m_BgSkins[i].get();
		}
	}

	DEBUG_WARN(0, "GetBgSkinByName() for name [%s] returned nullptr.", _name);
	return nullptr;
}

void ResourceService::AddBgSkin(
	std::unique_ptr<BackgroundSkinInterface> _skin
) {
	m_BgSkins.emplace_back(std::move(_skin));
}

const Font* ResourceService::GetFontByName(
	const char* name
) const {
	size_t fontCount = m_Fonts.size();
	if (fontCount == 0) {
		DEBUG_ASSERT(0, "GetFontByName() called when no skins were loaded.");
		return nullptr;
	}

	if (!name) {
		return &m_Fonts[0];
	}

	for (size_t i = 0; i < fontCount; i++) {
		if (
			strcmp(name, m_Fonts[i].GetName().c_str()) == 0
			) {
			return &m_Fonts[i];
		}
	}

	DEBUG_WARN(0, "GetFontByName() for name [%s] returned nullptr.", name);
	return nullptr;
}

void ResourceService::UploadFontParameters(
	const char* fontFileLocation,
	const char* fontName,
	bool loadInUIsheet
) {
	m_FontLoadQueue.emplace_back(
		fontName,
		fontFileLocation,
		loadInUIsheet
	);
}

void ResourceService::LoadFonts() {
	m_Fonts.clear();
	m_Fonts.reserve(m_FontLoadQueue.size());

	for (size_t i = 0; i < m_FontLoadQueue.size(); i++) {
		FontLoadingParameters params = m_FontLoadQueue[i];
		const std::string& name = params.name;
		const std::string& location = params.location;
		std::fstream file(GetAbsolutePathForAsset(location.c_str()), std::ios::in);
		std::string currentLine;

		std::u32string preparedCharset;
		std::vector<unsigned short> preparedOffsets;
		int preparedGlyphCount = 0;
		int offsetCounter = 0;
		std::string fontName;
		std::string sheetName;
		float gWidth = 0.f, gHeight = 0.f;

		DEBUG_ASSERT(file.is_open(), "Font with name [%s] tried to open file at [%s] but couldn\'t open it.", name.c_str(), location.c_str());
		
		while (std::getline(file, currentLine)) {
			if (currentLine.length() == 0 || currentLine.starts_with("//")) {
				continue;
			}
			
			DEBUG_ASSERT(currentLine.contains("="), "Font file [%s] has bad key-value pair.", location.c_str());

			auto equalSign = currentLine.find("=");
			std::string key = currentLine.substr(0, equalSign);
			std::string val = currentLine.substr(equalSign + 1, currentLine.length() - equalSign - 1);

			if (key == "font_name") {
				fontName = std::move(val);
				continue;
			}

			if (key == "sheet_name") {
				sheetName = std::move(val);
				continue;
			}

			if (key == "width") {
				gWidth = std::stof(val);
				continue;
			}

			if (key == "height") {
				gHeight = std::stof(val);
				continue;
			}

			if (key == "charset") {
				std::stringstream ss(val);
				uint32_t codepoint;
				while (ss >> codepoint) {
					preparedCharset.push_back(codepoint);
					preparedGlyphCount++;
				}
				continue;
			}

			if (key == "offsets") {
				std::stringstream ss(val);
				unsigned short offset;
				while (ss >> offset) {
					preparedOffsets.push_back(offset);
					offsetCounter++;
				}
				continue;
			}
		}

		const SpriteSheet* sheetSearchResult = GetSpriteSheetByName(sheetName.c_str());
		DEBUG_ASSERT(sheetSearchResult, "Font with name [%s] queried for non-existant sprite sheet with name [%s]", name.c_str(), sheetName.c_str());
		DEBUG_ASSERT(offsetCounter >= preparedGlyphCount, "Font with name [%s] has less offsets than glyphs in its charset.", name.c_str());

		Font self(sheetSearchResult, fontName, gWidth, gHeight);
		self.Init(
			preparedCharset,
			preparedOffsets.data(),
			preparedGlyphCount
		);

		m_Fonts.emplace_back(std::move(self));
		if (params.loadInUISheet) {
			m_UIBatch.AddFont(GetFontByName(params.name.c_str()));
		}
	}

	m_FontLoadQueue.clear();
}

void ResourceService::LoadSpriteSheets() {
	m_Sheets.clear();
	m_Sheets.reserve(m_SpriteSheetLoadQueue.size());

	for (const auto& params : m_SpriteSheetLoadQueue) {
		const std::string sheetPath = GetAbsolutePathForAsset(params.location);

		DEBUG_ASSERT(sheetPath.ends_with(".sheet"), "Attempted to load sprite sheet with bad extension [%s]", sheetPath.c_str());

		std::fstream file(sheetPath, std::ios::in);

		DEBUG_ASSERT(
			file.is_open(),
			"Failed to open sprite sheet [%s]",
			sheetPath.c_str()
		);

		SpriteSheetDefinition definition;

		std::string currentLine;
		SpriteDefinition* currentSprite = nullptr;

		while (std::getline(file, currentLine)) {
			if (currentLine.empty() || currentLine.starts_with("//")) {
				continue;
			}

			// [sprite_name]
			if (currentLine.starts_with("[") &&
				currentLine.ends_with("]")) {

				std::string spriteName =
					currentLine.substr(1, currentLine.size() - 2);

				definition.sprites.emplace_back();
				currentSprite = &definition.sprites.back();
				currentSprite->name = spriteName;

				continue;
			}

			if (currentSprite &&
				currentLine.contains("x=") &&
				currentLine.contains("y=") &&
				currentLine.contains("w=") &&
				currentLine.contains("h=")) {

				std::stringstream ss(currentLine);
				std::string token;

				bool foundX = false;
				bool foundY = false;
				bool foundW = false;
				bool foundH = false;

				while (ss >> token) {
					auto equalSign = token.find('=');

					DEBUG_ASSERT(
						equalSign != std::string::npos,
						"Sprite sheet [%s] has malformed sprite definition [%s]",
						sheetPath.c_str(),
						currentLine.c_str()
					);

					std::string key = token.substr(0, equalSign);
					std::string value = token.substr(equalSign + 1);

					if (key == "x") {
						currentSprite->x = std::stoi(value);
						foundX = true;
					}
					else if (key == "y") {
						currentSprite->y = std::stoi(value);
						foundY = true;
					}
					else if (key == "w") {
						currentSprite->w = std::stoi(value);
						foundW = true;
					}
					else if (key == "h") {
						currentSprite->h = std::stoi(value);
						foundH = true;
					}
				}

				DEBUG_ASSERT(
					foundX && foundY && foundW && foundH,
					"Sprite sheet [%s] has incomplete sprite definition [%s]",
					sheetPath.c_str(),
					currentLine.c_str()
				);

				continue;
			}

			DEBUG_ASSERT(
				currentLine.contains("="),
				"Sheet file [%s] has bad key-value pair.",
				sheetPath.c_str()
			);

			auto equalSign = currentLine.find('=');

			std::string key =
				currentLine.substr(0, equalSign);

			std::string val =
				currentLine.substr(equalSign + 1);

			// remove surrounding quotes
			if (val.size() >= 2 &&
				val.front() == '"' &&
				val.back() == '"') {

				val = val.substr(1, val.size() - 2);
			}

			if (key == "img_path") {
				LoadImageFile(GetAbsolutePathForAsset(val.c_str()).c_str(), &definition.image);
				continue;
			}

			if (key == "params") {
				definition.params = std::move(val);
				continue;
			}

			if (key == "filter") {
				definition.filter = std::move(val);
				continue;
			}

			if (key == "name") {
				definition.sheetName = std::move(val);
				continue;
			}

			if (key == "shader_name") {
				const Shader* result = GetShaderByName(val.c_str());
				definition.shader = result;
				continue;
			}

			if (key == "rowCount") {
				definition.rowCount = std::stoi(val);
				continue;
			}

			if (key == "colCount") {
				definition.colCount = std::stoi(val);
				continue;
			}

			if (key == "padding") {
				definition.padding = std::stoi(val);
				continue;
			}

			DEBUG_ASSERT(
				false,
				"Unknown key [%s] in sprite sheet [%s]",
				key.c_str(),
				sheetPath.c_str()
			);
		}

		SpriteSheet sheet = SpriteSheet(definition);

		m_Sheets.emplace_back(std::move(sheet));

		if (params.loadInUISheet) {
			m_UIBatch.AddSheetToBatch(&m_Sheets.back());
		}
	}

	m_SpriteSheetLoadQueue.clear();
}

void ResourceService::SetAbsoluteBasePath(const char* path) {
	m_AbsoluteBasePath = fs::path(path);
}

std::string ResourceService::GetAbsolutePathForAsset(
	const char* relPath
) const {
	return (GetAbsoluteBasePath() / fs::path(relPath)).string();
}

struct ShaderProgramSources {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	DEBUG_ASSERT(stream.is_open(), "Failed to open shader file: %s", filepath.c_str())

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) {

			if (line.find("vertex") != std::string::npos) {
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				//set mode to frag
				type = ShaderType::FRAGMENT;
			}
		}
		else
			ss[(int)type] << line << "\n";
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));  //dynamic stack allocation
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
		std::cout << message << "\n";
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void ResourceService::LoadShaders() {
	m_Shaders.clear();
	m_Shaders.reserve(m_ShaderLoadQueue.size());

	for (const auto& params : m_ShaderLoadQueue) {
		std::string fullPathToShader = GetAbsolutePathForAsset(params.m_LocationOfShaderFile.c_str());
		ShaderProgramSources src = ParseShader(fullPathToShader.c_str());
		uint32_t programId = CreateShader(src.VertexSource, src.FragmentSource);

		m_Shaders.emplace_back(
			programId,
			params.m_ShaderName
		);
	}
}

void ResourceService::LoadImageFile(
	const char* fullPath,
	ImageFile* OUT_image
) {
	unsigned char* imageData = nullptr;
	int nChannels;
	int heightPx;
	int widthPx;

	imageData = stbi_load(
		fullPath,
		&widthPx,
		&heightPx,
		&nChannels,
		4
	);

	if (!imageData) {
		// unfixable, crash here
		std::cout << stbi_failure_reason();
		throw std::runtime_error(stbi_failure_reason());
	}

	OUT_image->widthPx = widthPx;
	OUT_image->heightPx = heightPx;
	OUT_image->imageData = imageData;
}

void ImageFile::Destroy() {
	if (imageData) {
		stbi_image_free(imageData);
		imageData = nullptr;
		widthPx = 0;
		heightPx = 0;
	}
}