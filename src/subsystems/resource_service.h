#pragma once

#include "../opengl/batch.h"

#include "../opengl/shader.h"
#include "../opengl/sprite_sheet.h"
#include "../components/ui/font.h"
#include "../components/ui/widget/widget_interface.h"

struct ShaderLoadingParameters {
	const std::string m_ShaderName;
	const std::string m_LocationOfShaderFile;
	ShaderLoadingParameters(
		const char* _location,
		const char* _shaderName
	)
		: m_ShaderName(_shaderName), m_LocationOfShaderFile(_location) {
	}
};

struct SpriteSheetLoadingParameters {
	const std::string m_LocationOfImage;
	const std::string m_SheetName;
	const std::string m_PreferredShaderName;
	int m_SpritesPerRow;
	int m_SpritesPerCol;
	int paddingPx = 0;
	bool loadInUIsheet;

	SpriteSheetLoadingParameters(
		const char* _locationRawImage,
		const char* _sheetName,
		const char* _preferredShader,
		int _spritesPerRow,
		int _spritesPerCol,
		bool loadInUIsheet,
		int paddingPx = 0
	) :
		m_LocationOfImage(_locationRawImage),
		m_SheetName(_sheetName),
		m_PreferredShaderName(_preferredShader),
		m_SpritesPerRow(_spritesPerRow),
		m_SpritesPerCol(_spritesPerCol),
		loadInUIsheet(loadInUIsheet),
		paddingPx(paddingPx)
	{}
};

struct FontLoadingParameters {
	const std::string location;
	const std::string name;
	bool loadInUISheet;

	FontLoadingParameters(
		const char* fontName,
		const char* fontFileLocation,
		bool loadInUIsheet = false
	) :
		location(fontFileLocation),
		name(fontName),
		loadInUISheet(loadInUISheet)
	{}
};

class ResourceService {

	GLFWwindow* m_MainWindowContext = nullptr;

private:

	std::vector<Shader> m_Shaders;
	std::vector<SpriteSheet> m_Sheets;

	std::vector<Font> m_Fonts;
	std::vector<std::unique_ptr<BackgroundSkinInterface>> m_BgSkins;

	std::vector<ShaderLoadingParameters> m_ShaderLoadQueue;
	std::vector<SpriteSheetLoadingParameters> m_SpriteSheetLoadQueue;
	std::vector<FontLoadingParameters> m_FontLoadQueue;

private:

	TextOptions m_DefaultTextOptions;
	Batch m_UIBatch;

	SpriteInstance m_CaretSprite;

private:

	const BackgroundSkinInterface* m_bgCloseBtnSkin = nullptr;

public:

	ResourceService() {}

	ResourceService(
		GLFWwindow* win
	) 
		: m_MainWindowContext(win)
	{}

private:

	void LoadShader(
		const std::string& _locationShaderFile,
		const std::string& _shaderName
	);

	void LoadSpriteSheet(
		const std::string& _locationRawImage,
		const std::string& _sheetName,
		const Shader* _preferredShader,
		int _spritesPerRow,
		int _spritesPerCol,
		int paddingPx
	);

	void LoadFonts();

	void LoadDefaultVariables();

public:

	void UploadShaderParameters(
		const char* _location,
		const char* _shaderName
	);

	void UploadSpriteSheetParameters(
		const char* _locationRawImage,
		const char* _sheetName,
		const char* _preferredShader,
		int _spritesPerRow,
		int _spritesPerCol,
		bool loadInUIsheet = false,
		int paddingPx = 0
	);

	void UploadFontParameters(
		const char* fontName,
		const char* fontFileLocation,
		bool loadInUIsheet = false
	);

	void StartLoadingProcess();

public:

	void AddBgSkin(
		std::unique_ptr<BackgroundSkinInterface> _skin
	);

public:

	const Shader* GetShaderByName(
		const char* _shaderName
	);

	const SpriteSheet* GetSpriteSheetByName(
		const char* _spriteSheetName
	);

	const BackgroundSkinInterface* GetBgSkinByName(
		const char* _name = nullptr
	) const;

	const Font* GetFontByName(
		const char* name = nullptr
	) const;

	const BackgroundSkinInterface* GetCloseBtnBgSkin() const { return m_bgCloseBtnSkin; }

public:

	const TextOptions& GetDefaultTextOptions() const { return m_DefaultTextOptions; }
	
	Batch* GetUIBatch() { return &m_UIBatch; }

	const SpriteInstance GetCaretInstance() const { return m_CaretSprite; }

public:

	const char* c_SpecialUISheetName	= "SPECIAL_UI_SPRITESHEET_NAME";

};