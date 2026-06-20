#pragma once

/*
* Regular Sprite sheet where each sprite is next to each other
* and all have uniform dimensions
* 
*	SpriteSheet		- an image subdivided in N x M uniform regions
*	Sprite index	- subregions in the sprite sheet are indexed 
*					  left to right, top to bottom, starting from 0
*/

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

#include "../common/common.h"
#include "batch_instance_primitives.h"
#include "shader.h"

//	Structs intended to ease loading of this type

struct SpriteDefinition {
	std::string name;

	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
};

struct SpriteSheetDefinition {
	std::string sheetName;
	ImageFile image;
	const Shader* shader;
	std::string params;
	std::string filter;

	int rowCount = 0;
	int colCount = 0;
	int padding = 0;

	std::vector<SpriteDefinition> sprites;
};

struct alignas(float) UVRegion {

	UVRegion() {}
	//	  x		  y
	float u0 = 0, v0 = 0;	
	float u1 = 0, v1 = 0;

	UVRegion(
		float _u0, float _v0,
		float _u1, float _v1
	);

	void* GetData() { return this; }
	bool operator==(const UVRegion& other) const;
};

enum class SpriteSheetType {
	NotInitialised = 0,
	IndexBased,
	DirectUV,

	ErrorLoading
};

struct TextureParamsDataStruct {
	int S_WrapMode = -1;
	int T_WrapMode = -1;
};

struct xyDimensions {
	float x = 0, y = 0;
};

class SpriteSheet {

	int m_SheetWidth = -1;
	int m_SheetHeight = -1;

	//	These will be calculated at the creation of each object
	UVRegion m_SpriteUniformUVs;

	int m_SpriteCountPerRow = -1;	//	pieces per row
	int m_SpriteCountPerCol = -1;	//	pieces per col
	int m_PaddingPx = 0;

	//	will be used for easier, but slower, search in Renderer2D
	std::string m_SheetName;

	unsigned int m_TextureBufferID = 0x7FFFFFFF;
	const Shader* m_Shader = nullptr;

private:

	SpriteSheetType m_Type = SpriteSheetType::NotInitialised;
	TextureParamsDataStruct m_TexParams;

private:

	std::vector<UVRegion> m_UVregionsFromConfigFile;
	std::vector<std::string> m_UVregionNamesFromConfigFile;
	std::vector<xyDimensions> m_SpriteDimensionsPx;

public:

	glm::vec2 GetCalculatedSpriteOffsets(
		int _spriteIndex
	) const;

public:

	/*
		If loading a sheet by config file, values for
		sprite counts per row and column will be ignored
	*/

	SpriteSheet(
		const std::string& _locationOfImageOrConfigFile,
		const std::string& _sheetName,
		const Shader* _preferredShader,
		int _spritesPerRow_IGNORED_IF_LOADING_CONFIG_FILE,
		int _spritesPerCol_IGNORED_IF_LOADING_CONFIG_FILE,
		int paddingPx
	);

	SpriteSheet(
		const SpriteSheetDefinition& definition
	);

	SpriteSheet();

	unsigned short GetSpriteIndexByName(
		const char* _spriteName
	) const;

private:

	void SpriteListMethod(
		const SpriteSheetDefinition& definition
	);

	void RowColMethod(
		const SpriteSheetDefinition& definition
	);

	void GenerateTexture(
		const SpriteSheetDefinition& definition
	);

public:

	SpriteInstance GetSpriteInstance(
		const char* spriteName,
		size_t sheetIndex
	) const;

	void GetSpriteInstances(
		std::vector<SpriteInstance>& OUT_spriteArray,
		size_t sheetIndex
	) const;

	SpriteInstance GetSpriteInstanceByIndex(
		size_t index,
		size_t sheetIndex
	) const;

	SpriteInstance GetSpriteInstanceByGrid(
		size_t x,
		size_t y,
		size_t sheetIndex
	) const;

	void GetAllSpriteInstances(
		std::vector<SpriteInstance>& OUT_spriteArray,
		size_t sheetIndex
	) const;

	const UVRegion* GetUVRegionByName ( const char* _assetName ) const;
	SpriteSheetType GetType() const { return m_Type; }
	glm::vec2 GetSpriteDimensions() const;
	const UVRegion& GetSheetSpriteUVregion() const { return m_SpriteUniformUVs; }
	const std::string& GetName() const;
	const Shader* GetShader() const { return m_Shader; }
	int GetSheetRowSpriteCount() const { return m_SpriteCountPerRow; }
	unsigned int GetTextureBufferID() const { return m_TextureBufferID; }
	const TextureParamsDataStruct& GetTexParams() const { return m_TexParams; }
	const std::vector<UVRegion>& GetUVRegionArray() const { return m_UVregionsFromConfigFile; }
	const int GetContainedSpriteCount() const { return static_cast<int>(m_UVregionsFromConfigFile.size()); }
	const int GetSpriteSheetWidth() const { return m_SheetWidth; }
	const int GetSpriteSheetHeight() const { return m_SheetHeight; }
	void DestroyGLTextureObject();

};