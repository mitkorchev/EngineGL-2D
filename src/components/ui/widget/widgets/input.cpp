#include "input.h"

const float Input::c_CaretWidthPx = 2.f;

static bool IsWordTerminator(char32_t ch) {
	// Treat whitespace, punctuation, symbols, and separators as boundaries.
	// Letters/numbers from any script will generally NOT be terminators.
	switch (ch) {
		// Common ASCII whitespace
	case U' ':
	case U'\t':
	case U'\n':
	case U'\r':

		// Common punctuation / symbols
	case U'.': case U',': case U';': case U':':
	case U'!': case U'?':
	case U'"': case U'\'':
	case U'(': case U')':
	case U'[': case U']':
	case U'{': case U'}':
	case U'<': case U'>':
	case U'/': case U'\\':
	case U'|':
	case U'-': case U'_':
	case U'+': case U'=':
	case U'*': case U'&':
	case U'^': case U'%':
	case U'$': case U'#':
	case U'@': case U'~':
	case U'`':
		return true;
	}

	// Unicode space separators
	if (
		ch == 0x00A0 || // non-breaking space
		ch == 0x1680 ||
		(ch >= 0x2000 && ch <= 0x200A) ||
		ch == 0x2028 ||
		ch == 0x2029 ||
		ch == 0x202F ||
		ch == 0x205F ||
		ch == 0x3000
		) {
		return true;
	}

	// Basic symbol/punctuation ranges
	if (
		(ch >= 0x2000 && ch <= 0x206F) || // General punctuation
		(ch >= 0x2190 && ch <= 0x21FF) || // Arrows
		(ch >= 0x2200 && ch <= 0x22FF) || // Math operators
		(ch >= 0x2300 && ch <= 0x23FF) || // Technical symbols
		(ch >= 0x25A0 && ch <= 0x25FF) || // Geometric shapes
		(ch >= 0x2600 && ch <= 0x26FF) || // Misc symbols
		(ch >= 0x2700 && ch <= 0x27BF)    // Dingbats
		) {
		return true;
	}

	return false;
}

void Input::MoveCaretForwardOnce() {
	MoveCaretToPosition(m_CaretPosition + 1);
}

void Input::MoveCaretBackwardOnce() {
	if (m_CaretPosition == 0) return;
	MoveCaretToPosition(m_CaretPosition - 1);
}

void Input::MoveCaretToNextNonLetter() {
	const auto& text = GetText()->GetTextString();
	const size_t stringLength = text.length();

	if (m_CaretPosition == stringLength) return;

	bool mustStopBeforeNextNonLetter = false;
	size_t newCaretPosition = m_CaretPosition;
	for (size_t index = m_CaretPosition; index < stringLength; index++) {
		if (!IsWordTerminator(text[index])) {
			mustStopBeforeNextNonLetter = true;
		}
		else if (text[index] == ' ') {
			if (mustStopBeforeNextNonLetter) {
				newCaretPosition = index;
				break;
			}
		}
		else {
			newCaretPosition = index;
			break;
		}
	}

	if (newCaretPosition == m_CaretPosition)
		m_CaretPosition = stringLength;
	else
		m_CaretPosition = newCaretPosition;

	MoveCaretToPosition(m_CaretPosition);
}

void Input::MoveCaretToPrevNonLetter() {
	const auto& text = GetText()->GetTextString();

	if (m_CaretPosition == 0) return;

	size_t newCaretPosition = m_CaretPosition;
	bool mustStopBeforeNextNonLetter = false;
	for (size_t index = newCaretPosition; index-- > 0; ) {
		if (!IsWordTerminator(text[index])) {
			mustStopBeforeNextNonLetter = true;
		}
		else if (text[index] == ' ') {
			if (mustStopBeforeNextNonLetter) {
				newCaretPosition = index + 1;
				break;
			}
		}
		else {
			newCaretPosition = index + 1;
			break;
		}
	}

	if (newCaretPosition == m_CaretPosition)
		m_CaretPosition = 0;
	else
		m_CaretPosition = newCaretPosition;

	MoveCaretToPosition(m_CaretPosition);
}

void Input::InsertCharacterAtCaretPosition(
	char32_t ch
) {
	GetText()->InsertCharacter(ch, m_CaretPosition);
	MoveCaretForwardOnce();
}

void Input::DeleteCharacterBeforeCaretPosition() {
	GetText()->DeleteCharacter(m_CaretPosition);
	MoveCaretBackwardOnce();
}

//	Ctrl Backspace
void Input::ClearTillLastSpace() {
	DEBUG_LOG("Ctrl + Backspace pressed but not implemented.");
}

//	Ctrl Delete
void Input::ClearTillNextSpace() {
	DEBUG_LOG("Ctrl + Delete pressed but not implemented.");
}

void Input::ClearText() {
	GetText()->UpdateTextValue(U"");
	MoveCaretToPosition(0);
}

void Input::MoveCaretToPosition(
	size_t newPosition
) {
	//	Caret can be 1 character 'in front' of the string
	if (newPosition > GetText()->GetCharCount()) {
		newPosition = GetText()->GetCharCount();
	}

	float caretXposition = GetXOffsetForCaretPosition(newPosition) - c_CaretWidthPx;

	if (caretXposition - m_ScrollX < 0) {
		m_ScrollX = caretXposition;
	} 
	else if (caretXposition - (m_ScrollX + m_TextContent->GetDimensions().x) > 0) {
		m_ScrollX = caretXposition - m_TextContent->GetDimensions().x;
	}

	m_CaretPosition = newPosition;
	m_CaretXPosition = caretXposition - m_ScrollX;
	m_TextContent->UpdateScrollX(m_ScrollX);
}

float Input::GetXOffsetForCaretPosition(
	size_t caretPosition
) const {
	size_t valueCharCount = m_TextContent->GetText()->GetCharCount();
	
	if (valueCharCount == 0) return 0;
	
	if (caretPosition == valueCharCount) {
		size_t lastGlyphIndex = caretPosition - 1;
		const auto& lastGlyph = GetText()->GetTextGeometry()[caretPosition - 1];
		return (lastGlyph.position.x + GetText()->GetFont()->GetCharacterAdvance(GetText()->GetTextString()[caretPosition - 1]) * GetText()->GetTextOptions().scale);
	}

	return GetText()->GetTextGeometry()[caretPosition].position.x;
}