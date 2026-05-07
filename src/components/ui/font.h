#pragma once

#include <string>

#include "../../common/common.h"
#include "../../common/standard_quad.h"

inline constexpr unsigned int MAXIMUM_GLYPHS_PER_FONT = 256;
inline constexpr unsigned int MAXIMUM_LETTERS = 32;

struct GlyphSprite {
	SpriteInstance instance;
	unsigned short advance;
};

class Font {

	//	Holds the graphical aspect of the font, i.e. the
	//	images we'll be rendering
	const SpriteSheet* m_SpriteSheet = nullptr;

	unsigned short m_GlyphAdvances[MAXIMUM_GLYPHS_PER_FONT] = { 0 };

	std::u32string m_GlyphIdentifier;

	std::string m_FontName;

	float m_GlyphWidth = 0.f;
	float m_GlyphHeight = 0.f;

public:

	Font() {}

	Font(
		const SpriteSheet* _initialisedSheet,
		const std::string& _name,
		float glyphWidth,
		float glyphHeight
	);

	void Init(
		const std::u32string& glyphs,
		unsigned short* glyphOffsets,
		int glyphCount
	);

	unsigned short GetCharacterAdvance(
		char32_t character
	) const;

	GlyphSprite GetGlyph(
		char32_t ch
	) const;

	float GetGlyphAdvance(
		char32_t ch
	) const;

private:

	char32_t TransformToLowercase(
		char32_t ch
	) const;

	size_t GetGlyphIndex(
		char32_t ch
	) const;

public:

	const std::string& GetName() const { return m_FontName; }
	const SpriteSheet* GetFontSheet() const { return m_SpriteSheet; }
	const float GetGlyphWidth() const { return m_GlyphWidth; }
	const float GetGlyphHeight() const { return m_GlyphHeight; }

};

