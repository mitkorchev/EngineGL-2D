#pragma once

#include "label.h"

class Button : public WidgetCompositionInterface {

public:

	Button(
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
		AddChild(std::move(std::make_unique<Label>(text, glm::vec2(5.f, 5.f))));
	}

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const {}

};