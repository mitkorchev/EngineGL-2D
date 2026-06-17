#pragma once

#include "label.h"

class Button : public WidgetCompositionInterface {

public:

	Button(
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
		const Text& text,
		const BackgroundSkinInterface* bgSkin,
		Alignment alignment = Alignment::TOPLEFT,
		Alignment anchor = Alignment::TOPLEFT
	) :
		WidgetCompositionInterface(
			offsetRelToParent,
			dimensions,
			bgSkin,
			true,
			alignment,
			anchor
		)
	{
		AddChild(std::move(std::make_unique<Label>(text, glm::vec2(0.f, 0.f), Alignment::CENTER, Alignment::CENTER)));
	}

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const {}

};