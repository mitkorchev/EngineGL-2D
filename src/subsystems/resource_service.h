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
	const char* location;
	bool loadInUISheet;

	SpriteSheetLoadingParameters(
		const char* fileLocation,
		bool loadInUIsheetArg = false
	) :
		location(fileLocation),
		loadInUISheet(loadInUIsheetArg)
	{}
};

struct FontLoadingParameters {
	const std::string location;
	const std::string name;
	bool loadInUISheet;

	FontLoadingParameters(
		const char* fontName,
		const char* fontFileLocation,
		bool loadInUIsheetArg = false
	) :
		location(fontFileLocation),
		name(fontName),
		loadInUISheet(loadInUIsheetArg)
	{}
};

class ResourceService {

	GLFWwindow* m_MainWindowContext = nullptr;

private:

	fs::path m_AbsoluteBasePath;

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

	void LoadShaders();

	void LoadSpriteSheets();

	void LoadFonts();

	void LoadDefaultVariables();

	std::string GetAbsolutePathForAsset(
		const char* relPath
	) const;

private:

	void LoadImageFile(
		const char* fullPath,
		ImageFile* OUT_image
	);

public:

	void SetAbsoluteBasePath(const char* path);

	void UploadShaderParameters(
		const char* _location,
		const char* _shaderName
	);

	void UploadSpriteSheetParameters(
		const char* location,
		bool loadInUIsheet = false
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

	const fs::path GetAbsoluteBasePath() const { return m_AbsoluteBasePath; }

public:

	const char* c_SpecialUISheetName	= "gui";

};