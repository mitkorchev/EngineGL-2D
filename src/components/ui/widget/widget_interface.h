#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "background_skin.h"
#include "event_emitter.h"

#define UI_EVENT_CONTEXT_PARAMS EventEmitter* ctx, Window* owningWindow

class Window;

class WidgetCompositionInterface {

	std::vector<std::unique_ptr<WidgetCompositionInterface>> m_WidgetComposition;

private:

	glm::vec2 m_PositionRelToParent = glm::vec2(0.f, 0.f);
	glm::vec2 m_Dimensions = glm::vec2(0.f, 0.f);

	bool m_IsHitTestable = true;	//<<< Set to true, very advanced concept
	bool m_IsInteractable = false;

	const BackgroundSkinInterface* m_BackgroundSkin = nullptr;

	std::vector<FullSprite> m_BackgroundGeometry;

private:

	std::function<void(EventEmitter* ctx, Window* owningWindow)> m_OnClickFn;

public:

	virtual void OnClick(EventEmitter* ctx, Window* owningWindow) {
		if (m_OnClickFn) {
			m_OnClickFn(ctx, owningWindow);
		}
	}

public:

	WidgetCompositionInterface() {}

	WidgetCompositionInterface(
		glm::vec2 offsetRelToParent,
		glm::vec2 dimensions,
		const BackgroundSkinInterface* background,
		bool isInteractable
	) :
		m_PositionRelToParent(offsetRelToParent),
		m_Dimensions(dimensions),
		m_BackgroundSkin(background),
		m_IsInteractable(isInteractable)
	{
		if (background) {
			CalculateBackgroundGeometry();
		}
	}

	WidgetCompositionInterface(
		float xOffsetRelToParent,
		float yOffsetRelToParent,
		float xDimension,
		float yDimension,
		const BackgroundSkinInterface* background,
		bool isInteractable
	) :
		m_PositionRelToParent(xOffsetRelToParent, yOffsetRelToParent),
		m_Dimensions(xDimension, yDimension),
		m_BackgroundSkin(background),
		m_IsInteractable(isInteractable)
	{
		if (background) {
			CalculateBackgroundGeometry();
		}
	}

	WidgetCompositionInterface(WidgetCompositionInterface& other) = delete;
	WidgetCompositionInterface& operator=(const WidgetCompositionInterface& other) = delete;

public:

	void AddChild(std::unique_ptr<WidgetCompositionInterface> widget) {
		m_WidgetComposition.push_back(std::move(widget));
	}

	void ApplyChangeToPosition(
		glm::vec2 change
	);

	//	Note: design is smooth except here
	void RenderWidgetTree(
		Batch* uiBatch,
		glm::vec2 absoluteParentOrigin,
		float baseZLayer,
		float zSubstep
	);

	WidgetCompositionInterface* DetectClick(
		glm::vec2 absoluteParentOrigin,
		glm::vec2 mousePosition
	);

private:

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const = 0;

	//	As it stands, backgrounds will probably be shared by widgets who are groups of others only.
	//	Widgets like "Label" probably won't work properly if BG data is sent.
	void SendBackgroundRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	);

	void CalculateBackgroundGeometry();

protected:

	glm::vec2 GetAbsolutePosition(
		glm::vec2 parentOrigin
	) const;

public:

	void SetRelPosition(
		glm::vec2 newPos
	);

	void SetOnClick(
		std::function<void(EventEmitter* ctx, Window* owningWindow)> fn
	);

public:

	glm::vec2 GetDimensions() const { return m_Dimensions; }

	const std::vector<std::unique_ptr<WidgetCompositionInterface>>& GetWidgets() { return m_WidgetComposition; }

	virtual ~WidgetCompositionInterface() {}

};