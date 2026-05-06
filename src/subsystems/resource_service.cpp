#include "resource_service.h"

static const char* g_DefaultFontName = "cyrillic";

void ResourceService::LoadShader(
	const std::string& _locationShaderFile,
	const std::string& _shaderName
) {
	m_Shaders.emplace_back(
		_locationShaderFile,
		_shaderName
	);
}

void ResourceService::LoadSpriteSheet(
	const std::string& _locationRawImage,
	const std::string& _sheetName,
	const Shader* _preferredShader,
	int _spritesPerRow,
	int _spritesPerCol,
	int paddingPx
) {
	m_Sheets.emplace_back(
		_locationRawImage,
		_sheetName,
		_preferredShader,
		_spritesPerRow,
		_spritesPerCol,
		paddingPx
	);
}

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
	const char* _locationRawImage,
	const char* _sheetName,
	const char* _preferredShader,
	int _spritesPerRow,
	int _spritesPerCol,
	bool loadInUIsheet,
	int paddingPx
) {
	m_SpriteSheetLoadQueue.emplace_back(
		_locationRawImage,
		_sheetName,
		_preferredShader,
		_spritesPerRow,
		_spritesPerCol,
		loadInUIsheet,
		paddingPx
	);
}

void ResourceService::StartLoadingProcess() {
	m_Shaders.clear();
	m_Sheets.clear();

	if (m_Shaders.capacity() <= m_ShaderLoadQueue.size()) {
		m_Shaders.reserve(m_ShaderLoadQueue.size());
	}

	for (size_t i = 0; i < m_ShaderLoadQueue.size(); i++) {
		ShaderLoadingParameters& params = m_ShaderLoadQueue[i];
		LoadShader(
			std::string(params.m_LocationOfShaderFile),
			std::string(params.m_ShaderName)
		);
	}
	m_ShaderLoadQueue.clear();

	
	if (m_Sheets.capacity() <= m_SpriteSheetLoadQueue.size()) {
		m_Sheets.reserve(m_SpriteSheetLoadQueue.size());
	}

	for (size_t i = 0; i < m_SpriteSheetLoadQueue.size(); i++) {
		SpriteSheetLoadingParameters& params = m_SpriteSheetLoadQueue[i];
		
		LoadSpriteSheet(
			std::string(params.m_LocationOfImage),
			std::string(params.m_SheetName),
			GetShaderByName(params.m_PreferredShaderName.c_str()),
			params.m_SpritesPerRow,
			params.m_SpritesPerCol,
			params.paddingPx
		);

		if (params.loadInUIsheet) {
			m_UIBatch.AddSheetToBatch(GetSpriteSheetByName(params.m_SheetName.c_str()));
		}
	}
	m_SpriteSheetLoadQueue.clear();

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
	const char* fontName,
	const char* fontFileLocation,
	bool loadInUIsheet
) {
	m_FontLoadQueue.emplace_back(
		fontName,
		fontFileLocation,
		loadInUIsheet
	);
}

void ResourceService::LoadFonts() {
	for (size_t i = 0; i < m_FontLoadQueue.size(); i++) {
		FontLoadingParameters params = m_FontLoadQueue[i];
		const std::string& name = params.name;
		const std::string& location = params.location;
		std::fstream file(location.c_str(), std::ios::in);
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
