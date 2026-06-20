#pragma once

#include <memory>

#include "resource_service.h"

#include "../opengl/batch.h"

#include "../components/input_controller.h"
#include "../components/ui/widget/event_emitter.h"
#include "../components/ui/widget/widgets/window.h"
#include "../components/ui/widget/widgets/input.h"
#include "../components/ui/widget/widgets/button.h"
#include "../components/ui/widget/widgets/textless_button.h"

struct CapturedStates {
	bool capturedMouse = false;
	bool capturedKeyboard = false;
};

class UIManager : public WidgetCompositionInterface {
	
	ResourceService* m_ResService = nullptr;
	InputController* m_InputController = nullptr;

	Batch* m_UIBatch = nullptr;
	
private:
	
	float m_CloserBoundOfZLayerDistribution = 0.f;
	float m_FurtherBoundOfZLayerDistribution = 0.f;

	EventEmitter m_EventEmitter;

	Input* m_FocusedInputField = nullptr;

	ID m_UniqueWindowIDCounter = 0;

	std::vector<std::unique_ptr<Window>> m_OpenedWindows;

	float m_CaretBlinkerTimer = 0.f;

public:

	UIManager(
		glm::vec2 screenDimensions
	) : 
		WidgetCompositionInterface(
			glm::vec2(0, 0),
			glm::vec2(screenDimensions),
			nullptr,
			false
		)
	{}

	void Init(
		ResourceService* resService,
		InputController* inputController
	) {
		m_ResService = resService;
		m_InputController = inputController;
		m_UIBatch = m_ResService->GetUIBatch();
	}

	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;

	std::unique_ptr<Window> GenWindowObject(
		glm::vec2 dimensions,
		const BackgroundSkinInterface* bgSkin = nullptr,
		bool haveClosingButton = true
	);

	void InterpretInput(
		float elapsedTimeSeconds
	);

	void OpenWindow(std::unique_ptr<Window> window, glm::vec2 openWhere);

	void MoveWindowToFront(ID windowID);

	void CloseWindow(ID windowID);

private:

	void EnforceUIEvents();

private:

	ResourceService* GetResService() { return m_ResService; }
	InputController* GetInput() { return m_InputController; }
	Batch* GetBatch() { return m_UIBatch; }
	ID GetNextWindowID() { return ++m_UniqueWindowIDCounter; }

public:

	void SetZSpaceDistribution(
		float zClose,
		float zFurther
	);

	virtual void SendOwnRenderData(
		Batch* uiBatch,
		glm::vec2 absoluteCurrentWidgetOrigin,
		float z
	) const {}

public:

	float GetCloserZBound() const { return m_CloserBoundOfZLayerDistribution; }
	float GetFurtherZBound() const { return m_FurtherBoundOfZLayerDistribution; }

};