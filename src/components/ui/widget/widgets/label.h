#pragma once

#include "../widget_interface.h"

class Label : public WidgetCompositionInterface {

	Text m_StoredText;

	float m_ScrollX = 0.f;
	bool m_IsClampingText = false;

public:
	
	Label(
		const Text& text,
		glm::vec2 offsetRelToParent, ,
		Alignment alignment = Alignment::TOPLEFT,
		Alignment anchor = Alignment::TOPLEFT
	) :
		m_StoredText(text),
		WidgetCompositionInterface(
			offsetRelToParent,
			glm::vec2(text.GetLineLength(), text.GetLineHeight()),
			nullptr,
			false,
			alignment,
			anchor
		)
	{}

	Label(
		const Text& text,
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
		Alignment alignment = Alignment::TOPLEFT,
		Alignment anchor = Alignment::TOPLEFT
	) :
		m_StoredText(text),
		WidgetCompositionInterface(
			offsetRelToParent,
			dimensions,
			nullptr,
			false,
			alignment,
			anchor
		)
	{
		m_StoredText.SetLineLength(dimensions.x);
	}

	Text* GetText() { return &m_StoredText; }

private:

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteParentOrigin,
		float z
	) const override;

public:

	void SetClampingMode(bool isClamping) { m_IsClampingText = isClamping; }

	void UpdateScrollX(float scrollX) { m_ScrollX = scrollX; }

	virtual ~Label() {}

};