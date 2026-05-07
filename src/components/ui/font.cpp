#include "font.h"

Font::Font(
	const SpriteSheet* _initialisedSheet,
	const std::string& _name,
    float glyphWidth,
    float glyphHeight
):
	m_SpriteSheet(_initialisedSheet),
	m_FontName(_name),
    m_GlyphWidth(glyphWidth),
    m_GlyphHeight(glyphHeight)
{}

void Font::Init(
    const std::u32string& glyphs,
    unsigned short* glyphOffsets,
    int glyphCount
)
{
    m_GlyphIdentifier.clear();
    m_GlyphIdentifier.reserve(
        glyphs.size()
    );

    for (char32_t c : glyphs)
        m_GlyphIdentifier.push_back(c);

    memcpy(m_GlyphAdvances, glyphOffsets, glyphCount * sizeof(unsigned short));
}

unsigned short Font::GetCharacterAdvance(
    char32_t ch
) const {
    for (int i = 0; i < m_GlyphIdentifier.size(); i++)
    {
        if (m_GlyphIdentifier[i] == ch)
            return m_GlyphAdvances[i];
    }

    return 0;
}

size_t Font::GetGlyphIndex(
    char32_t ch
) const {
    for (int i = 0; i < m_GlyphIdentifier.size(); i++)
    {
        if (m_GlyphIdentifier[i] == ch)
            return i;
    }

    //  TODO: HACKFIX until cyrillic font fixes this
    return MAXIMUM_LETTERS * 2 - 2;
}

GlyphSprite Font::GetGlyph(
    char32_t ch
) const {
    size_t chIndex = GetGlyphIndex(ch);
    GlyphSprite result;
    //  Sheet index is queried in the DrawText function, once per Text draw call
    result.instance = m_SpriteSheet->GetSpriteInstanceByIndex(chIndex, 0);  
    result.advance = m_GlyphAdvances[chIndex];

    return result;
}

float Font::GetGlyphAdvance(
    char32_t ch
) const {
    size_t chIndex = GetGlyphIndex(ch);
    return m_GlyphAdvances[chIndex];
}