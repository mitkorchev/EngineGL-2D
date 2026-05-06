#include "batch.h"


/*		DATA TYPES		*/

//	Spread(bits) for type SpriteInformation
//	31	-	y-axis cut off direction - 0 shrink from bottom, 1 shrink from top
//	30	-	x-axis cut off direction - 0 shrink from right,  1 shrink from left
//	
//	29	-	sheet index
//	..
//	24
// 
//	23	-	sprite index within sheet
//	..
//	0

SpriteInformation::SpriteInformation(
	uint32_t _sheetIndex,
	uint32_t _spriteIndex
) {
	SetSheetIndex(_sheetIndex);
	SetSpriteIndex(_spriteIndex);
}

uint32_t SpriteInformation::GetSheetIndex() const {
	return (data & 0x3F000000ui32) >> 24;
}

uint32_t SpriteInformation::GetSpriteIndex() const {
	return data & 0x00FFFFFFui32;
}

void SpriteInformation::SetSheetIndex(uint32_t sheetIndex) {
	data &= 0xC0FFFFFFui32;
	data |= (sheetIndex & 0x3F) << 24;
}

void SpriteInformation::SetSpriteIndex(uint32_t spriteIndex) {
	data &= 0xFF000000ui32;
	data |= (spriteIndex & 0x00FFFFFFui32);
}

void SpriteInformation::SetXCutoff(bool fromLeft) {
	if (!fromLeft) {
		data &= ~(1u << 30);                 // clear
	}
	else {
		data |= (1u << 30);                  // set
	}
}

void SpriteInformation::SetYCutoff(bool fromTop) {
	if (!fromTop) {
		data &= ~(1u << 31);                 // clear
	}
	else {
		data |= (1u << 31);                  // set
	}
}

unsigned short SpriteInstance::PackRemainFactor(float remainFactor) {
	return 65535ui16 * remainFactor;
}

void SpriteInstance::SetXCut(float remainFactor, bool cutFromLeft) {
	xCut = PackRemainFactor(remainFactor);
	SpriteInfo.SetXCutoff(cutFromLeft);
}

void SpriteInstance::SetYCut(float remainFactor, bool cutFromTop) {
	yCut = PackRemainFactor(remainFactor);
	SpriteInfo.SetYCutoff(cutFromTop);
}

void SpriteInstance::SetXCutPixels(float pixelsToCut, bool cutFromLeft) {
	if (dimensions.x <= 0.0f) {
		xCut = 0;
		return;
	}

	float remainPixels = std::max(0.0f, dimensions.x - pixelsToCut);
	float remainFactor = remainPixels / dimensions.x;

	dimensions.x -= pixelsToCut;
	SetXCut(remainFactor, cutFromLeft);
}

void SpriteInstance::SetYCutPixels(float pixelsToCut, bool cutFromTop) {
	if (dimensions.y <= 0.0f) {
		yCut = 0;
		return;
	}

	float remainPixels = std::max(0.0f, dimensions.y - pixelsToCut);
	float remainFactor = remainPixels / dimensions.y;

	dimensions.y -= pixelsToCut;
	SetYCut(remainFactor, cutFromTop);
}



unsigned int Batch::c_NotInitialised = 1 << 0;
unsigned int Batch::c_MaximumInstanceCountExceeded = 1 << 16;
unsigned int Batch::s_VAO = 0;

Batch::Batch(
	bool initialiseGLObjects
) {
	if (initialiseGLObjects) {
		InitialiseBuffers();
	}
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::ActivateTextures(
	const char* _sampler2DArrayUniformName
) const {
	if (m_SpriteSheets.empty()) return;

	const Shader* Shader = m_SpriteSheets[0]->GetShader();
	std::vector<int> TexUnits;

	for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));
		glBindTexture(GL_TEXTURE_2D, m_SpriteSheets[i]->GetTextureBufferID());
		TexUnits.push_back(static_cast<int>(i));
	}

#ifdef DEBUG__CODE
	CheckGLErrors();
#endif

	Shader->SetIntArray(_sampler2DArrayUniformName, TexUnits.data(), static_cast<const int>(TexUnits.size()));

#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::AddSheetToBatch(
	const SpriteSheet* _spriteSheet
) {
	DEBUG_ASSERT(_spriteSheet != nullptr, "Null pointers passed to batch method for adding sheets.");

	if (m_SpriteSheets.size()) {
		DEBUG_ASSERT(_spriteSheet->GetShader() == m_SpriteSheets[0]->GetShader(), "Batch w/ name [%s] has tried to append SpriteSheet objects with different shaders!", dm_BatchName.c_str());
	}

	for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
		if (_spriteSheet == m_SpriteSheets[i]) {
			//DEBUG_WARN(0, "Batch w/ name [%s] has tried to append the same SprteSheet object more than once. Skipping...", dm_BatchName.c_str());
			return;
		}
	}

	m_SpriteSheets.emplace_back(_spriteSheet);
}

void Batch::AddFont(
	const Font* font
) {
	DEBUG_ASSERT(font != nullptr, "Null pointers passed to batch method for adding fonts.");

	//	Confirm font is not added already
	for (size_t i = 0; i < m_Fonts.size(); i++) {
		if (font == m_Fonts[i]) {
			DEBUG_WARN(0, "Batch tried to enter font with name [%s] twice.", font->GetName().c_str());
			return;
		}
	}

	AddSheetToBatch(font->GetFontSheet());
	m_Fonts.push_back(font);
}

Text Batch::GenText(
	const std::u32string& str,
	const char* fontName
) const {
	DEBUG_ASSERT(m_Fonts.size(), "Batch attepmpted to generate Text object without fonts.");

	const Font* font = GetFont(fontName);
	TextOptions options;
	options.font = font;
	options.storedSheetIndex = GetSheetIndex(font->GetFontSheet());
	
	return Text(str, options);
}

Text Batch::GenText(
	const std::u32string& str,
	const TextOptions& options
) const {
	return Text(str, options);
}

TextOptions Batch::GetTextOptionsForFont(
	const char* fontName
) const {
	TextOptions self;
	self.font = GetFont(fontName);
	self.storedSheetIndex = GetSheetIndex(self.font->GetFontSheet());

	return self;
}

const SpriteSheet* Batch::GetSpecialSheetPointer() const {
	DEBUG_ASSERT(m_SpriteSheets.size() > 0, "Batch has empty Sheet container.");

	return m_SpriteSheets[0];
}

void Batch::BufferUBOs() {
	if (m_SpriteSheets.empty()) return;

	CheckGLErrors();

	if (!GLdiagnostics::IsBuffer(m_SheetIndexOffsetsUBO)) {
		DEBUG_ASSERT(0, "Buffering UBOs that aren't initialised.");
		return;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, m_SheetUVRegionsUBO);

	std::vector<UVRegion> UVs;
	for (const auto& sheet : m_SpriteSheets) {
		const auto& arr = sheet->GetUVRegionArray();
		UVs.insert(UVs.end(), arr.begin(), arr.end());
	}

	glBufferData(GL_UNIFORM_BUFFER, UVs.size() * sizeof(UVRegion), UVs.data(), GL_STATIC_DRAW);

	//	for the (std140) buffer alignment requiring 16 byte blocks within the shader code
	//	Note: UVRegion doesn't need it since it's already 4 * float = 16 bytes
	struct alignas(16) PaddedInteger {
		int value;
		PaddedInteger(int _val) : value(_val) {}
	};

	glBindBuffer(GL_UNIFORM_BUFFER, m_SheetIndexOffsetsUBO);
	std::vector<PaddedInteger> Offsets;
	int TotalOffset = 0;
	for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
		Offsets.emplace_back(static_cast<int>(TotalOffset));
		TotalOffset += m_SpriteSheets[i]->GetContainedSpriteCount();
	}

	glBufferData(GL_UNIFORM_BUFFER, Offsets.size() * sizeof(PaddedInteger), Offsets.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	const Shader* ShaderObject = GetSpecialSheetPointer()->GetShader();

	unsigned int Program = ShaderObject->GetShaderId();

	unsigned int UBO_UVRegions = ShaderObject->GetUniformBlockLocation("ubo_UVRegions");
	unsigned int UBO_SheetOffsets = ShaderObject->GetUniformBlockLocation("ubo_SheetOffsets");

	CheckGLErrors();
	glUniformBlockBinding(Program, UBO_UVRegions, 0);
	glUniformBlockBinding(Program, UBO_SheetOffsets, 1);
	CheckGLErrors();
}

void Batch::BindUBOs() const {
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_SheetUVRegionsUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_SheetIndexOffsetsUBO);
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::DrawSprite(
	const FullSprite& sprite
) {
	if (m_CurrentlyQueuedSprites == m_QueuedSpritesForRender.capacity()) {
		IncreaseBufferMemoryTo(m_CurrentlyQueuedSprites * 2);
	}

	if (m_CurrentlyQueuedSprites < m_QueuedSpritesForRender.size()) {
		m_QueuedSpritesForRender[m_CurrentlyQueuedSprites] = sprite;
	}
	else {
		m_QueuedSpritesForRender.emplace_back(sprite);
	}

	m_CurrentlyQueuedSprites++;
}

void Batch::DrawSprite(
	const SpriteInstance& spriteInstance,
	float x,
	float y,
	float rotation,
	float z
) {
	FullSprite constructedRenderData;
	constructedRenderData.instance = spriteInstance;
	constructedRenderData.position.x = x;
	constructedRenderData.position.y = y;
	constructedRenderData.rotation = rotation;
	constructedRenderData.z = z;

	DrawSprite(constructedRenderData);
}

//	should return instance count and update the buffer
int Batch::SendSpriteDataToGPU() {
	glNamedBufferSubData(
		GetBufferID(),
		0,
		m_CurrentlyQueuedSprites * sizeof(FullSprite),
		m_QueuedSpritesForRender.data()
	);

	size_t retVal = m_CurrentlyQueuedSprites;
	m_CurrentlyQueuedSprites = 0;
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
	return int(retVal);
}

void Batch::DrawText(
	const Text* textObject,
	float x,
	float y,
	float z
) {
	auto textGeometry = textObject->GetTextGeometry();

	for (auto&& sprite : textGeometry) {
		DrawSprite(
			sprite.instance,
			sprite.position.x + x,
			sprite.position.y + y,
			0.f,
			z
		);
	}
}

void Batch::DrawSprites(
	const std::vector<FullSprite>& sprites,
	float x,
	float y,
	float z
) {
	for (const auto& sprite : sprites) {
		DrawSprite(
			sprite.instance,
			(sprite.position.x) + x,
			(sprite.position.y) + y,
			0.f,
			z
		);
	}
}

void Batch::IncreaseBufferMemoryTo(
	size_t spriteCount
) {
	glNamedBufferData(GetBufferID(), spriteCount * sizeof(FullSprite), 0, GL_STREAM_DRAW);
	CheckGLErrors();
	m_QueuedSpritesForRender.reserve(spriteCount);
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::InitialiseBuffers() {
	unsigned int vbo[3];
	glCreateBuffers(3, vbo);
	
	m_BatchBuffer = vbo[0];
	IncreaseBufferMemoryTo(MIN_SPRITES_WITH_RESERVED_MEM);

	m_SheetIndexOffsetsUBO = vbo[1];
	m_SheetUVRegionsUBO = vbo[2];
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::DeleteBuffers() {
	if (m_BatchBuffer != 0) {
		glDeleteBuffers(1, &m_BatchBuffer);
		glDeleteBuffers(1, &m_SheetUVRegionsUBO);
		glDeleteBuffers(1, &m_SheetIndexOffsetsUBO);

		m_BatchBuffer = 0;
		m_SheetUVRegionsUBO = 0;
		m_SheetIndexOffsetsUBO = 0;
	}
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

Batch::~Batch() {
	DeleteBuffers();
}

SpriteInstance Batch::GetSprite(
	const char* _sheetName,
	const char* _spriteNameWithinSheet
) const {
	SpriteInstance retVal;
	
	for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
		if (!m_SpriteSheets[i]->GetName().compare(_sheetName)) {
			retVal = m_SpriteSheets[i]->GetSpriteInstance(_spriteNameWithinSheet, i);
			
			DEBUG_ASSERT(!retVal.IsNull(), "Batch derived bad sprite [%s] from sheet [%s].", _spriteNameWithinSheet, _sheetName);

			break;
		}
	}

	return retVal;
}

void Batch::GetSprites(
	std::vector<SpriteInstance>& OUT_spriteArray,
	const char* sheetName
) const {
	OUT_spriteArray.clear();

	if (!sheetName) {
		std::vector<SpriteInstance> placeholder;
		for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
			m_SpriteSheets[i]->GetSpriteInstances(placeholder, i);

			OUT_spriteArray.insert(
				OUT_spriteArray.end(),
				placeholder.begin(),
				placeholder.end()
			);
		}
	}
	else {
		for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
			if (!m_SpriteSheets[i]->GetName().compare(sheetName)) {
				m_SpriteSheets[i]->GetSpriteInstances(OUT_spriteArray, i);
				return;
			}
		}
	}
}

void Batch::InitialiseCommonVAO() {

	if (GLdiagnostics::IsVertexArray(s_VAO)) {
		DEBUG_ASSERT(0, "Attempt to reinitialise common FreeBatch VAO object.");
		return;
	}

	glGenVertexArrays(1, &s_VAO);
	glBindVertexArray(s_VAO);

	//	@0 - common quad VBO
	g_StandardQuad.BindVertexBufferAt(0);

	//	@element - common element VBO
	g_StandardQuad.BindElementIndexArray();

	//	@1 - common texture VBO
	glBindBuffer(GL_ARRAY_BUFFER, g_StandardQuad.GetUnmodifiedTextureUVBuffer());
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(1);

	glBindVertexBuffer(2, 0, 0, sizeof(FullSprite)); // buffer bound later per-batch

	//	SpriteInfo
	glVertexAttribIFormat(2, 1, GL_UNSIGNED_INT, offsetof(FullSprite, instance) + offsetof(SpriteInstance, SpriteInfo));
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	//	Rotation
	glVertexAttribFormat(3, 1, GL_FLOAT, GL_FALSE, offsetof(FullSprite, rotation));
	glVertexAttribBinding(3, 2);
	glEnableVertexAttribArray(3);

	//	Position
	glVertexAttribFormat(4, 2, GL_FLOAT, GL_FALSE, offsetof(FullSprite, position));
	glVertexAttribBinding(4, 2);
	glEnableVertexAttribArray(4);

	//	Dimensions
	glVertexAttribFormat(5, 2, GL_FLOAT, GL_FALSE, offsetof(FullSprite, instance) + offsetof(SpriteInstance, dimensions));
	glVertexAttribBinding(5, 2);
	glEnableVertexAttribArray(5);

	//	Z coord	(ignored in regular render)
	glVertexAttribFormat(6, 1, GL_FLOAT, GL_FALSE, offsetof(FullSprite, z));
	glVertexAttribBinding(6, 2);
	glEnableVertexAttribArray(6);

	//	Z coord	(ignored in regular render)
	glVertexAttribFormat(6, 1, GL_FLOAT, GL_FALSE, offsetof(FullSprite, z));
	glVertexAttribBinding(6, 2);
	glEnableVertexAttribArray(6);

	//	Cut values
	glVertexAttribIFormat(7, 2, GL_UNSIGNED_SHORT, offsetof(FullSprite, instance) + offsetof(SpriteInstance, xCut));
	glVertexAttribBinding(7, 2);
	glEnableVertexAttribArray(7);

	// All share the same divisor (since per-instance)
	glVertexBindingDivisor(2, 1);
	glBindVertexArray(0);
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::BindUniqueBuffer() const {
	glBindVertexBuffer(2, m_BatchBuffer, 0, sizeof(FullSprite));
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::BindCommonVAO(){
	glBindVertexArray(s_VAO);
#ifdef DEBUG__CODE
	CheckGLErrors();
#endif
}

void Batch::UnbindCommonVAO(){
	glBindVertexArray(0);
}

const Font* Batch::GetFont(
	const char* fontName
) const {
	DEBUG_ASSERT(m_Fonts.size(), "Batch searched for fonts when none were loaded.");

	for (size_t i = 0; i < m_Fonts.size(); i++) {
		if (!strcmp(fontName, m_Fonts[i]->GetName().c_str())) {
			return m_Fonts[i];
		}
	}

	return m_Fonts[0];
}

const uint32_t Batch::GetSheetIndex(
	const SpriteSheet* sheetPtr
) const {
	for (size_t i = 0; i < m_SpriteSheets.size(); i++) {
		if (sheetPtr == m_SpriteSheets[i]) {
			return static_cast<uint32_t>(i);
		}
	}

	return 0;
}