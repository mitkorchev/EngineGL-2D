#pragma once

#include "../widget_interface.h"

class Label : public WidgetCompositionInterface {

	Text m_StoredText;

	float m_ScrollX = 0.f;
	bool m_IsClampingText = false;

public:
	
	Label(
		const Text& text,
		glm::vec2 offsetRelToParent
	) :
		m_StoredText(text),
		WidgetCompositionInterface(
			offsetRelToParent,
			glm::vec2(text.GetLineLength(), text.GetLineHeight()),
			nullptr,
			false
		)
	{}

	Label(
		const Text& text,
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions
	) :
		m_StoredText(text),
		WidgetCompositionInterface(
			offsetRelToParent,
			dimensions,
			nullptr,
			false
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