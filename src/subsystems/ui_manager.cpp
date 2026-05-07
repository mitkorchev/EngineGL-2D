#include "ui_manager.h"
#include <print>

void UIManager::SetZSpaceDistribution(
	float zClose,
	float zFurther
) {
	m_CloserBoundOfZLayerDistribution = zClose;
	m_FurtherBoundOfZLayerDistribution = zFurther;
}

void UIManager::InterpretInput(
	float elapsedTimeSeconds
) {
	GameInput& input = GetInput()->ExposeGameInput();

	/* process keyboard here */

	Batch* uiBatch = GetResService()->GetUIBatch();

	const int MAXIMUM_NESTED_WIDGETS = 4;
	const int totalLayers = 1 + m_OpenedWindows.size();	//	dont forget UI-manager ui
	const int totalSublayers = totalLayers * MAXIMUM_NESTED_WIDGETS;
	
	
	const float baseZLayer = m_FurtherBoundOfZLayerDistribution;
	const float workRange = GetFurtherZBound() - GetCloserZBound();
	const float substep = - (workRange / float(totalSublayers));

	
	//	Window in the END of the vector is closest to us.
	//	We render from bottom to top
	//	We detect from top to bottom

	//	---		RENDER		---	 //

	RenderWidgetTree(uiBatch, glm::vec2(0.f, 0.f), baseZLayer, substep);

	//	render windows
	for (size_t i = 0; i < m_OpenedWindows.size(); i++) {
		float correctBaseZlayer = baseZLayer + ((i + 1) * MAXIMUM_NESTED_WIDGETS * substep);
		m_OpenedWindows[i].get()->RenderWidgetTree(
			uiBatch,
			glm::vec2(0.f, 0.f),
			correctBaseZlayer,
			substep
		);
	}

	if (input.IsPressed(GLFW_MOUSE_BUTTON_LEFT) || input.IsPressed(GLFW_KEY_ESCAPE)) {
		std::println("Focus released");
		m_FocusedInputField = nullptr;
	}

	if (m_FocusedInputField) {
		std::u32string bufferedInput = input.GetUtfInput();
		
		glm::vec2 caretPosition = m_FocusedInputField->GetAbsoluteCaretPosition();
		
		if (m_CaretBlinkerTimer >= 1.0f)
			m_CaretBlinkerTimer -= 1.0f;

		if(m_CaretBlinkerTimer < 0.5f) {
			SpriteInstance caretInstance = GetResService()->GetCaretInstance();
			caretInstance.dimensions.y = m_FocusedInputField->GetCharHeight();
			uiBatch->DrawSprite(caretInstance, caretPosition.x, caretPosition.y, 0, GetCloserZBound());
		}
		m_CaretBlinkerTimer += elapsedTimeSeconds;

		if (input.IsPressed(GLFW_KEY_LEFT)) {
			if (input.IsHeld(GLFW_KEY_LEFT_CONTROL))
				m_FocusedInputField->MoveCaretToPrevNonLetter();
			else
				m_FocusedInputField->MoveCaretBackwardOnce();
			m_CaretBlinkerTimer = 0.f;
		}

		if (input.IsPressed(GLFW_KEY_RIGHT)) {
			if (input.IsHeld(GLFW_KEY_LEFT_CONTROL))
				m_FocusedInputField->MoveCaretToNextNonLetter();
			else
				m_FocusedInputField->MoveCaretForwardOnce();
			m_CaretBlinkerTimer = 0.f;
		}

		if (input.IsPressed(GLFW_KEY_BACKSPACE)) {
			m_FocusedInputField->DeleteCharacterBeforeCaretPosition();
			m_CaretBlinkerTimer = 0.f;
		}

		if (bufferedInput.size()) {
			m_CaretBlinkerTimer = 0.f;
		}

		for (auto ch : bufferedInput) {
			m_FocusedInputField->InsertCharacterAtCaretPosition(ch);
		}

		input.SetKeyboardCapturedFlag();
	}

	if (input.AccessRecentKeystateBitmask(GLFW_MOUSE_BUTTON_LEFT)) {
		float mouseX, mouseY;
		input.GetMousePosition(mouseX, mouseY);
		glm::vec2 mousePos = glm::vec2(mouseX, mouseY);
		WidgetCompositionInterface* clickedWidget = DetectClick(glm::vec2(0.f, 0.f), mousePos);
		Window* clickedWindow = nullptr;

		for (size_t i = m_OpenedWindows.size(); i-- > 0; ) {
			Window* targetWindow = m_OpenedWindows[i].get();
			WidgetCompositionInterface* windowTarget = targetWindow->DetectClick(glm::vec2(0.f, 0.f), mousePos);
			
			if (windowTarget) {
				clickedWidget = windowTarget;
				clickedWindow = targetWindow;
				break;
			}
		}

		if (clickedWindow) {
			MoveWindowToFront(clickedWindow->GetID());
		}

		if (clickedWidget && clickedWidget == clickedWindow) {
			clickedWindow->ApplyChangeToPosition(input.GetMouseChange());
		}

		if (clickedWidget && input.IsPressed(GLFW_MOUSE_BUTTON_LEFT)) {
			clickedWidget->OnClick(&m_EventEmitter, clickedWindow);
			input.SetMouseCapturedFlag();
		}
	}

	EnforceUIEvents();
}

void UIManager::OpenWindow(std::unique_ptr<Window> window, glm::vec2 openWhere) {
	window.get()->SetRelPosition(openWhere);
	m_OpenedWindows.push_back(std::move(window));
}

void UIManager::MoveWindowToFront(ID windowID) {
	for (size_t i = 0; i < m_OpenedWindows.size(); i++) {
		if (m_OpenedWindows[i]->GetID() == windowID) {
			std::unique_ptr<Window> movingWindow = std::move(m_OpenedWindows[i]);
			m_OpenedWindows.erase(m_OpenedWindows.begin() + i);
			m_OpenedWindows.push_back(std::move(movingWindow));
			return;
		}
	}
}

void UIManager::CloseWindow(ID windowID) {
	for (size_t i = 0; i < m_OpenedWindows.size(); i++) {
		if (m_OpenedWindows[i]->GetID() == windowID) {
			m_OpenedWindows.erase(m_OpenedWindows.begin() + i);
			return;
		}
	}
}

void UIManager::EnforceUIEvents() {
	auto& events = m_EventEmitter.GetEvents();

	for (auto& event : events) {
		switch (event.type) {
		case EventType::CLOSE_WINDOW: {
			CloseWindow(event.closeWindowEvent.targetWindowID);
			break;
		}
		case EventType::SET_FOCUS_ON_INPUT: {
			m_FocusedInputField = event.setInputFocusEvent.targetInputPointer;
			std::println("Focus set");
		}
		}
	}

	events.clear();
}

std::unique_ptr<Window> UIManager::GenWindowObject(
	glm::vec2 dimensions,
	const BackgroundSkinInterface* bgSkin,
	bool haveClosingButton
) {
	const BackgroundSkinInterface* selectedSkin = bgSkin;
	if (!selectedSkin) {
		selectedSkin = GetResService()->GetBgSkinByName();
	}

	auto self = std::make_unique<Window>(GetNextWindowID(), dimensions, selectedSkin);
	
	if (haveClosingButton) {
		glm::vec2 closeBtnPos = dimensions;
		closeBtnPos.x -= 20 + 4;	//	TODO:	HARDCODED DIMENSIONS OF CLOSING BUTTON	
		closeBtnPos.y = 4;			//	TODO:	HARDCODED PADDING FOR CLOSING BUTTON

		auto closeBtn = std::make_unique<TextlessButton>(closeBtnPos, glm::vec2(20.f, 20.f), GetResService()->GetCloseBtnBgSkin());
		closeBtn.get()->SetOnClick([](UI_EVENT_CONTEXT_PARAMS) {
			Event self;
			self.type = EventType::CLOSE_WINDOW;
			self.closeWindowEvent.targetWindowID = owningWindow->GetID();
			ctx->PushEvent(self);
		});

		self.get()->AddChild(std::move(closeBtn));
	}

	return self;
}
