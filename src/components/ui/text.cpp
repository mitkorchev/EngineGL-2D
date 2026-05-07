#include "text.h"

Text::Text() {}

Text::Text(
	const std::u32string& _string,
	TextOptions _textOptions
) :
	m_TextContent(_string),
	m_TextOptions(_textOptions)
{
	m_HasChanged = true;
    CalculateGeometry();
}

void Text::UpdateTextValue(
	std::u32string&& _stringToMove
) {
	m_TextContent = std::move(_stringToMove);
	m_HasChanged = true;
    CalculateGeometry();
}

void Text::UpdateTextValue(
	const std::u32string& _stringToCopy
) {
	m_TextContent = _stringToCopy;
	m_HasChanged = true;
    CalculateGeometry();
}

void Text::AppendCharacter(
	char32_t _char
) {
	m_TextContent += _char;
	m_HasChanged = true;
    CalculateGeometry();
}

void Text::RemoveLastCharacter() {
	if (m_TextContent.size() > 0) {
		m_TextContent.pop_back();
		m_HasChanged = true;
        CalculateGeometry();
    }
}

void Text::SetLineLength(
	float length
) {
	m_TextOptions.lineLength = length;
	m_HasChanged = true;
    CalculateGeometry();
}

const std::vector<FullSprite>& Text::GetTextGeometry() const {
	if (m_HasChanged) {
		CalculateGeometry();
	}
	return m_TextGeometry;
}

void Text::CalculateGeometry() const {

	m_TextGeometry.clear();
	m_HasChanged = false;
    float scale = m_TextOptions.scale;

    auto AppendGlyph = [&](char32_t ch, float x, float y) {
        GlyphSprite glyph = GetFont()->GetGlyph(ch);
        glyph.instance.SpriteInfo.SetSheetIndex(m_TextOptions.storedSheetIndex);

        FullSprite self;
        self.instance = glyph.instance;

        self.instance.dimensions.x *= scale;
        self.instance.dimensions.y *= scale;

        self.position.x = x;
        self.position.y = y;

        m_TextGeometry.emplace_back(self);

        return glyph.advance * scale;
    };

	if (m_TextOptions.scrollType == TextScroll::Oneline) {
		float CurrentLineLength = 0;

        for (size_t index = 0; index < m_TextContent.size(); index++) {
            CurrentLineLength += AppendGlyph(
                m_TextContent[index],
                CurrentLineLength,
                0.0f
            );
        }

		return;
	}

    if (m_TextOptions.scrollType == TextScroll::Multiline) {
        float CurrentLineLength = 0.0f;
        int CurrentLineCount = 0;

        size_t index = 0;

        while (index < m_TextContent.length()) {

            if (m_TextContent[index] == '\n') {
                CurrentLineLength = 0.0f;
                CurrentLineCount++;
                index++;
                continue;
            }

            size_t wordStart = index;
            float wordLength = 0.0f;

            while (index < m_TextContent.length() && m_TextContent[index] != ' ' && m_TextContent[index] != '\n') {
                wordLength += GetFont()->GetCharacterAdvance(m_TextContent[index]) * scale ;
                index++;
            }

            size_t wordEnd = index;

            size_t spacesStart = index;
            float spacesLength = 0.0f;
            float singleSpaceLength = GetFont()->GetCharacterAdvance(' ') * scale;
            while (index < m_TextContent.length() && m_TextContent[index] == ' ') {
                spacesLength += singleSpaceLength;
                index++;
            }

            bool FitsCurrentLine = (CurrentLineLength + wordLength) <= GetLineLength();

            if (!FitsCurrentLine) {

                if (CurrentLineLength > 0.0f) {
                    CurrentLineCount++;
                    CurrentLineLength = 0.0f;
                }

                if (wordLength > GetLineLength()) {

                    for (size_t charIndex = wordStart; charIndex < wordEnd; ++charIndex) {
                        GlyphSprite glyph = GetFont()->GetGlyph(m_TextContent[charIndex]);

                        if (CurrentLineLength + glyph.advance * scale > GetLineLength()) {
                            CurrentLineCount++;
                            CurrentLineLength = 0.0f;
                        }

                        CurrentLineLength += AppendGlyph(
                            m_TextContent[charIndex],
                            CurrentLineLength,
                            float(CurrentLineCount) * GetLineHeight() * scale
                        );
                    }
                }
                else {
                    for (size_t charIndex = wordStart; charIndex < wordEnd; ++charIndex) {
                        CurrentLineLength += AppendGlyph(
                            m_TextContent[charIndex],
                            CurrentLineLength,
                            float(CurrentLineCount) * GetLineHeight() * scale
                        );
                    }
                }
            }
            else {
                for (size_t charIndex = wordStart; charIndex < wordEnd; ++charIndex) {
                    CurrentLineLength += AppendGlyph(
                        m_TextContent[charIndex],
                        CurrentLineLength,
                        float(CurrentLineCount) * GetLineHeight() * scale
                    );
                }
            }

            if (CurrentLineLength + spacesLength <= GetLineLength()) {
                for (size_t spaceIndex = spacesStart; spaceIndex < index; ++spaceIndex) {
                    CurrentLineLength += AppendGlyph(
                        m_TextContent[spaceIndex],
                        CurrentLineLength,
                        float(CurrentLineCount) * GetLineHeight() * scale
                    );;
                }
            }
            else {
                CurrentLineCount++;
                CurrentLineLength = 0.0f;
            }
        }

        return;
    }
}

void Text::SetScrollType(
	TextScroll scrollType
) {
	m_TextOptions.scrollType = scrollType;
	m_HasChanged = true;
    CalculateGeometry();
}

void Text::InsertCharacter(
    char32_t ch,
    size_t oldCaretPosition
) {
    DEBUG_ASSERT(oldCaretPosition <= m_TextContent.length(), "Text object has tried to enter a character at invalid caret position.");

    if (oldCaretPosition == m_TextContent.length()) {
        m_TextContent += ch;
    }
    else {
        m_TextContent.insert(m_TextContent.begin() + oldCaretPosition, ch);
    }

    CalculateGeometry();
}

void Text::DeleteCharacter(
    size_t caretPosition
) {
    if (caretPosition == 0) return;
    m_TextContent.erase(m_TextContent.begin() + caretPosition - 1);
    CalculateGeometry();
}