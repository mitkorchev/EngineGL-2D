#pragma once

#include "label.h"

class Input : public WidgetCompositionInterface {

	Label* m_TextContent = nullptr;

	size_t m_CaretPosition = 0;
	float m_CaretXPosition = 0.f;
	float m_ScrollX = 0.f;

	//	Must equal the padding of the nested Label
	float m_LeftLabelPadding;
	float m_TopLabelPadding;

	mutable glm::vec2 m_AbsoluteCaretPosition = glm::vec2(0.f, 0.f);

public:

	Input(
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
		const Text& text,
		const BackgroundSkinInterface* bgSkin
	) :
		WidgetCompositionInterface(
			offsetRelToParent,
			dimensions,
			bgSkin,
			true
		)
	{
		auto label = std::make_unique<Label>(text, glm::vec2(5.f, 5.f), dimensions - glm::vec2(10.f, text.GetLineHeight()));
		
		m_TextContent = label.get();
		m_TextContent->SetClampingMode(true);

		AddChild(std::move(label));

		m_LeftLabelPadding = 5.f;
		m_TopLabelPadding = 5.f;
	}

	void OnClick(EventEmitter* ctx, Window* owningWindow) {
		Event self;
		self.type = EventType::SET_FOCUS_ON_INPUT;
		self.setInputFocusEvent.targetInputPointer = this;
		ctx->PushEvent(self);
	}

	void MoveCaretForwardOnce();
	void MoveCaretBackwardOnce();

	void MoveCaretToNextNonLetter();
	void MoveCaretToPrevNonLetter();

	void InsertCharacterAtCaretPosition(
		char32_t ch
	);

	void DeleteCharacterBeforeCaretPosition();

	//	Ctrl Backspace
	void ClearTillLastSpace();
	
	//	Ctrl Delete
	void ClearTillNextSpace();

	void ClearText();

	const std::u32string* GetValue() const { return &m_TextContent->GetText()->GetTextString(); }

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const {
		m_AbsoluteCaretPosition = absoluteCurrentWidgetOrigin + glm::vec2(m_CaretXPosition + m_LeftLabelPadding, m_TopLabelPadding);
	}

private:

	void MoveCaretToPosition(
		size_t newPosition
	);

	float GetXOffsetForCaretPosition(
		size_t caretPosition
	) const;

private:

	const Text* GetText() const { return m_TextContent->GetText(); }

	Text* GetText() { return m_TextContent->GetText(); }

public:

	static const float c_CaretWidthPx;

	glm::vec2 GetAbsoluteCaretPosition() const { return m_AbsoluteCaretPosition; }

	float GetCharHeight() const { return GetText()->GetLineHeight(); }

};