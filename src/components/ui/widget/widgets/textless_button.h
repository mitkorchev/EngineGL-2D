#pragma once

#pragma once

#include "../widget_interface.h"

class TextlessButton : public WidgetCompositionInterface {

public:

	TextlessButton(
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
		const BackgroundSkinInterface* bgSkin
	) :
		WidgetCompositionInterface(
			offsetRelToParent,
			dimensions,
			bgSkin,
			true
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