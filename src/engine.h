#pragma once

#include "subsystems/main_renderer.h"
#include "subsystems/resource_service.h"
#include "subsystems/ui_manager.h"

#include "subsystems/factories/text_factory.h"

class Engine2D {

	Renderer2D m_Renderer;

	UIManager m_UIManager;

	InputController m_InputController;

	ResourceService m_ResourceService;

private:

	TextFactory m_TextFactory;

private:

	GLFWwindow* m_MainWindowContext = nullptr;

	int m_ScreenWidth = -1;
	int m_ScreenHeight = -1;
	std::string m_WindowTitle;

	bool m_Fullscreen = false;

	bool m_IsRunning = false;

private:

	float m_ElapsedTimeSeconds = 0.00f;

	std::function<void(float, GameInput, GameLoopReturnType&)> m_GameLoop;

public:

	Engine2D(
		int screenWidth,
		int screenHeight,
		const char* windowTitle,
		bool fullscreen
	)
		:
		m_ScreenWidth(screenWidth),
		m_ScreenHeight(screenHeight),
		m_WindowTitle(windowTitle),
		m_Fullscreen(fullscreen),
		m_UIManager(glm::vec2(screenWidth, screenHeight))
	{
		PreInit();
	}

	Engine2D(Engine2D& other) = delete;
	Engine2D& operator=(const Engine2D&) = delete;

	void Init();

	void ExecuteFrame();

private:

	void PreInit();

	bool GLFWInitialisation();

	void QueueBatchesToRenderer(
		GameLoopReturnType& gameLoopRetVal
	);

public:

	bool IsRunning() const { return !glfwWindowShouldClose(m_MainWindowContext); }

	const TextFactory& GetTextFactory() { return m_TextFactory; }

	void SetGameLoop(
		std::function<void(float, GameInput, GameLoopReturnType&)> gameLoop
	) {
		m_GameLoop = gameLoop;
	}

	ResourceService& GetResourceService() { return m_ResourceService; }
	InputController& GetInputController() { return m_InputController; }
	UIManager& GetUIManager() { return m_UIManager; }
	Renderer2D& GetRenderer() { return m_Renderer; }

};
