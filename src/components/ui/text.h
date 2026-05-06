#pragma once

#include "font.h"

enum class TextScroll {
	Multiline,	//	multiple lines, render the lower-most part
	Oneline	//	all in one line, render the right-most part
};

struct TextOptions {
	const Font* font = nullptr;
	uint16_t storedSheetIndex = 0;
	float lineLength = 0.f;
	float lineHeight = 20.f;
	float scale = 1.0f;

	TextScroll scrollType = TextScroll::Oneline;
};

class Text {

	std::u32string m_TextContent;
	TextOptions m_TextOptions;

	mutable std::vector<FullSprite> m_TextGeometry;

	mutable bool m_HasChanged = false;
	
public:

	Text();

	Text(
		const std::u32string& _string,
		TextOptions _textOptions
	);

	void UpdateTextValue(
		std::u32string&& _stringToMove
	);

	void UpdateTextValue(
		const std::u32string& _stringToCopy
	);

	void AppendCharacter(
		char32_t _char
	);

	void RemoveLastCharacter();

	void SetLineLength(
		float length
	);

	void SetScrollType(
		TextScroll scrollType
	);

public:

	void InsertCharacter(
		char32_t ch,
		size_t oldCaretPosition
	);

	void DeleteCharacter(
		size_t caretPosition
	);

private:
	
	void CalculateGeometry() const;

public:

	const std::u32string& GetTextString() const { return m_TextContent; }
	size_t GetCharCount() const { return m_TextContent.size(); }

	const TextOptions& GetTextOptions() const { return m_TextOptions; }
	const Font* GetFont() const { return m_TextOptions.font; }
	const float GetLineHeight() const { return m_TextOptions.lineHeight; }
	float GetLineLength() const { return m_TextOptions.lineLength; }

	const std::vector<FullSprite>& GetTextGeometry() const;

};