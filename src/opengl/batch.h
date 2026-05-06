#pragma once

#include "batch_instance_primitives.h"
#include "../common/standard_quad.h"
#include "../components/ui/text.h"


static constexpr size_t MIN_SPRITES_WITH_RESERVED_MEM = 50;

class Batch {
	
	std::vector<const SpriteSheet*> m_SpriteSheets;
	std::vector<const Font*> m_Fonts;
	
	FlagTracker m_Flags = FlagTracker(c_NotInitialised);

private:

	static unsigned int s_VAO;
	size_t m_CurrentlyQueuedSprites = 0;
	std::vector<FullSprite> m_QueuedSpritesForRender;
	unsigned int m_BatchBuffer = 0;

protected:	//	Flags

	//	Errors
	static unsigned int c_NotInitialised;


	//	Exceptions
	static unsigned int c_MaximumInstanceCountExceeded;

protected:	//	UBOs

	unsigned int m_SheetUVRegionsUBO = 0;
	unsigned int m_SheetIndexOffsetsUBO = 0;

private:

	void IncreaseBufferMemoryTo(
		size_t spriteCount
	);

public:

	Batch() {}

	Batch(
		bool initialiseGLObjects
	);

	void BindUniqueBuffer() const;

	void BindUBOs() const;

	void BufferUBOs();

	void ActivateTextures(
		const char* _sampler2DArrayUniformName
	) const;

	//	Add a SpriteSheet* to this batch. It will be used in every
	//	draw call made with this batch.
	void AddSheetToBatch(
		const SpriteSheet* _spriteSheet
	);

	void AddFont(
		const Font* font
	);

	Text GenText(
		const std::u32string& str,
		const char* fontName = nullptr
	) const;

	Text GenText(
		const std::u32string& str,
		const TextOptions& options
	) const;

	TextOptions GetTextOptionsForFont(
		const char* fontName
	) const;

	void DrawSprite(
		const SpriteInstance& spriteInstance,
		float x, 
		float y,
		float rotation = 0.f,
		float z = 0.f
	);

	void DrawText(
		const Text* textObject,
		float x,
		float y,
		float z
	);

	void DrawSprites(
		const std::vector<FullSprite>& sprite,
		float x,
		float y,
		float z
	);

	void DrawSprite(
		const FullSprite& sprite
	);

	//	Note: figure out a way to remove this
	int SendSpriteDataToGPU();

	void InitialiseBuffers();

	void DeleteBuffers();
	
	//	If the sheet or sprite is missing, it'll return an SI with values (0, 0)
	SpriteInstance GetSprite(
		const char* sheetName,
		const char* spriteNameWithinSheet
	) const;

	//	Get all sprites for a certain sprite sheet.
	//	If sheetName is not found in batch, OUT_spriteArray will be cleared.
	//	If sheetName is nullptr, returns all sprites from first to last for each sheet, where sheets are also from first to last
	void GetSprites(
		std::vector<SpriteInstance>& OUT_spriteArray,
		const char* sheetName = nullptr
	) const;

#ifdef DEBUG__CODE

	std::string dm_BatchName;
	void DEBUG_SetName(
		const char* _newName
	) {
		dm_BatchName = _newName;
	}

#else

	inline void DEBUG_SetName(
		const char* DEBUG_MODE_IS_DISABLED_newName
	) {}

#endif

public:

	const size_t GetInstanceCount() const { return m_CurrentlyQueuedSprites; }

	const SpriteSheet* GetSpecialSheetPointer() const;

	const size_t GetSheetCount() const { return m_SpriteSheets.size(); }

	static void InitialiseCommonVAO();

	static void BindCommonVAO();
	static void UnbindCommonVAO();

	virtual ~Batch();

private:
	
	const unsigned int GetBufferID() const { return m_BatchBuffer; }

	const Font* GetFont(
		const char* fontName
	) const;

	const uint32_t GetSheetIndex(
		const SpriteSheet* sheetPtr
	) const;

};



