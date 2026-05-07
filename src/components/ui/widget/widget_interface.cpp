#include "widget_interface.h"

void WidgetCompositionInterface::RenderWidgetTree(
	Batch* uiBatch,
	glm::vec2 absoluteParentOrigin,
	float baseZLayer,
	float zSubstep
) {
	float currentZLayer = baseZLayer;
	glm::vec2 absoluteCurrentWidgetOrigin = GetAbsolutePosition(absoluteParentOrigin);
	
	SendBackgroundRenderData(uiBatch, absoluteCurrentWidgetOrigin, currentZLayer);
	SendOwnRenderData(uiBatch, absoluteCurrentWidgetOrigin, currentZLayer);

	for (size_t i = 0; i < m_WidgetComposition.size(); i++) {
		m_WidgetComposition[i].get()->RenderWidgetTree(
			uiBatch, 
			absoluteCurrentWidgetOrigin, 
			baseZLayer + zSubstep,
			zSubstep
		);
	}
}

WidgetCompositionInterface* WidgetCompositionInterface::DetectClick(
	glm::vec2 absoluteParentOrigin,
	glm::vec2 mousePosition
) {
	WidgetCompositionInterface* retVal = nullptr;
	glm::vec2 absoluteCurrentWidgetPosition = GetAbsolutePosition(absoluteParentOrigin);

	//	We allow for widgets to exist outside the borders of their parent
	if (PointInRect(mousePosition, absoluteCurrentWidgetPosition, m_Dimensions) && m_IsInteractable) {
		retVal = this;
	}

	WidgetCompositionInterface* nestedWidgetClicked = nullptr;
	for (size_t i = 0; i < m_WidgetComposition.size(); i++) {
		nestedWidgetClicked = m_WidgetComposition[i].get()->DetectClick(absoluteCurrentWidgetPosition, mousePosition);
		if (nestedWidgetClicked) {
			retVal = nestedWidgetClicked;
			break;
		}
	}

	return retVal;
}

void WidgetCompositionInterface::SendBackgroundRenderData(
	Batch* uiBatch,
	glm::vec2 absoluteCurrentWidgetOrigin,
	float z
) {
	for (size_t i = 0; i < m_BackgroundGeometry.size(); i++) {
		const FullSprite& sprite = m_BackgroundGeometry[i];
		uiBatch->DrawSprite(
			sprite.instance,
			sprite.position.x + absoluteCurrentWidgetOrigin.x,
			sprite.position.y + absoluteCurrentWidgetOrigin.y,
			0.f,
			z
		);
	}
}

/*
+-+----------+-+
|0|    4     |1|
+-+----------+-+
| |          | |
|6|    8     |7|
| |          | |
+-+----------+-+
|2|    5     |3|
+-+----------+-+
*/

void WidgetCompositionInterface::CalculateBackgroundGeometry() {
	if (m_BackgroundSkin) {
		m_BackgroundSkin->CalculateBackgroundGeometry(
			m_BackgroundGeometry,
			m_Dimensions
		);
	}
}

glm::vec2 WidgetCompositionInterface::GetAbsolutePosition(
	glm::vec2 parentOrigin
) const {
	return parentOrigin + m_PositionRelToParent;
}

void WidgetCompositionInterface::SetOnClick(
	std::function<void(EventEmitter* ctx, Window* owningWindow)> fn
) {
	m_OnClickFn = fn;
}

void WidgetCompositionInterface::ApplyChangeToPosition(
	glm::vec2 change
) {
	m_PositionRelToParent += change;
}

void WidgetCompositionInterface::SetRelPosition(
	glm::vec2 newPos
) {
	m_PositionRelToParent = newPos;
}