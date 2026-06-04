#pragma once

#pragma once

#include "../widget_interface.h"

class TextlessButton : public WidgetCompositionInterface {

public:

	TextlessButton(
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
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
	}

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const {
	}
};