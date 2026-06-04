#include "widget_interface.h"

/*
Those will multiply the widget's dimensions and will return vec2 which will offset
the widget such that its relative position will be relevant to whichever point is specified in this array
*/
static const glm::vec2 gs_AlignmentOffsetCoefficients[9] = {
	glm::vec2(0.5f, 0.5f),	//	CENTER
	glm::vec2(0.5f, 0),		//	TOP
	glm::vec2(0.5f, 1.f),	//	BOTTOM
	glm::vec2(0, 0.5f),		//	LEFT
	glm::vec2(1, 0.5f),		//	RIGHT
	glm::vec2(0, 0),		//	TOPLEFT
	glm::vec2(1, 0),		//	TOPRIGHT
	glm::vec2(0, 1),		//	BOTTOMLEFT
	glm::vec2(1, 1),		//	BOTTOMRIGHT
};

glm::vec2 ResolveAlignment(
	Alignment alignment
) {
	return gs_AlignmentOffsetCoefficients[std::to_underlying(alignment)];
}

void WidgetCompositionInterface::RenderWidgetTree(
	Batch* uiBatch,
	WidgetLayoutContext absoluteParentLayout,
	float baseZLayer,
	float zSubstep
) {
	float currentZLayer = baseZLayer;
	WidgetLayoutContext currentWidgetLayout = CalculatePosition(absoluteParentLayout);

	SendBackgroundRenderData(uiBatch, currentWidgetLayout.absoluteOrigin, currentZLayer);
	SendOwnRenderData(uiBatch, currentWidgetLayout.absoluteOrigin, currentZLayer);

	for (size_t i = 0; i < m_WidgetComposition.size(); i++) {
		m_WidgetComposition.at(i).get()->RenderWidgetTree(
			uiBatch, 
			currentWidgetLayout,
			baseZLayer + zSubstep,
			zSubstep
		);
	}
}

WidgetCompositionInterface* WidgetCompositionInterface::DetectClick(
	WidgetLayoutContext absoluteParentLayout,
	glm::vec2 mousePosition
) {
	WidgetCompositionInterface* retVal = nullptr;
	WidgetLayoutContext currentWidgetPosition = CalculatePosition(absoluteParentLayout);

	//	We allow for widgets to exist outside the borders of their parent
	if (PointInRect(mousePosition, currentWidgetPosition.absoluteOrigin, m_Dimensions) && m_IsInteractable) {
		retVal = this;
	}

	WidgetCompositionInterface* nestedWidgetClicked = nullptr;
	for (size_t i = 0; i < m_WidgetComposition.size(); i++) {
		nestedWidgetClicked = m_WidgetComposition.at(i).get()->DetectClick(currentWidgetPosition, mousePosition);
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

void WidgetCompositionInterface::SetOnClick(
	std::function<void(EventEmitter* ctx, Window* owningWindow)> fn
) {
	m_OnClickFn = fn;
}

void WidgetCompositionInterface::ApplyChangeToPosition(
	glm::vec2 change
) {
	m_PositionRelativeToAnchorPoint += change;
}

void WidgetCompositionInterface::SetPosRelativeToAnchor(
	glm::vec2 newPos
) {
	m_PositionRelativeToAnchorPoint = newPos;
}

void WidgetCompositionInterface::SetNewAnchoring(
	Alignment newAnchorPoint
) {
	m_AnchorPoint = newAnchorPoint;
}

WidgetLayoutContext WidgetCompositionInterface::CalculatePosition(
	WidgetLayoutContext parentLayoutContext
) {
	WidgetLayoutContext self;
	glm::vec2 anchorCoefficients = ResolveAlignment(m_AnchorPoint);
	glm::vec2 alignmentCoefficients = ResolveAlignment(m_Alignment);

	self.absoluteOrigin =
		parentLayoutContext.absoluteOrigin
		+ parentLayoutContext.dimensions * anchorCoefficients
		+ m_PositionRelativeToAnchorPoint
		- m_Dimensions * alignmentCoefficients;

	//	First we figure out the absolute position of the anchor point based on parent's rect.
	//	Then we move with position relative to it.
	//	Finally we adjust based on current widget's alignment around that final position, i.e.
	//	does that final position point to this widget's center or top left corner and so on.
	self.dimensions = m_Dimensions;

	return self;
}