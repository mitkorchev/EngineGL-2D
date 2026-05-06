#include "engine.h"

#include "components/ui/widget/widgets/window.h"

#include <print>

void Engine2D::PreInit() {
	if (!GLFWInitialisation()) {
		throw new std::exception("idk bro");
	}

	m_Renderer = Renderer2D(
		m_MainWindowContext,
		m_ScreenWidth,
		m_ScreenHeight,
		m_WindowTitle.c_str(),
		m_Fullscreen
	);

	m_InputController = InputController(m_MainWindowContext);

	m_ResourceService = ResourceService(m_MainWindowContext);
}

void Engine2D::Init() {
	m_Renderer.SetInputController(&m_InputController);
	m_Renderer.SetResService(&m_ResourceService);
	m_Renderer.Init();

	m_UIManager.Init(&m_ResourceService, &m_InputController);
	m_UIManager.SetZSpaceDistribution(1, 2);

	m_InputController.SetTrackedKeystatesBitmask(
		InputController::c_ArrowTrackBit |
		InputController::c_LetterTrackBit |
		InputController::c_SpecialTrackBit
	);

	m_ResourceService.StartLoadingProcess();

	m_TextFactory = TextFactory(&GetResourceService());
}

bool Engine2D::GLFWInitialisation() {
	//	Window and GLFW context initialisation
	if (!glfwInit()) {
		std::cout << "Window not opened.\n";
		return false;
	}

	m_MainWindowContext = glfwCreateWindow(
		m_ScreenWidth,
		m_ScreenHeight,
		m_WindowTitle.c_str(),
		NULL, NULL
	);

	if (!m_MainWindowContext) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_MainWindowContext);
	glfwSwapInterval(0);
	GLenum glewReturnCode = glewInit();
	if (glewReturnCode != GLEW_OK) {
		std::cout << glewGetErrorString(glewReturnCode);
		return false;
	}

	return true;
}

void Engine2D::QueueBatchesToRenderer(
	GameLoopReturnType& gameLoopRetVal
) {
	size_t CommCount = gameLoopRetVal.RenderCommands.size();

	float nearZ = GetUIManager().GetFurtherZBound();
	float farZ = GetRenderer().GetFarZCoord();

	float range = farZ - nearZ;
	float layerStep = range / static_cast<float>(CommCount);

	GetRenderer().Draw(GetResourceService().GetUIBatch(), 0, 0, 0, nullptr);
	
	std::ranges::stable_sort(gameLoopRetVal.RenderCommands, [](const RenderCommand& a, const RenderCommand& b) {
		return a.IssuedZLayer < b.IssuedZLayer;
	});

	for (size_t i = 0; i < gameLoopRetVal.RenderCommands.size(); i++) {
		const auto& rCommand = gameLoopRetVal.RenderCommands.at(i);
		float calculatedZcoord = nearZ + i * layerStep + 2.f;
		switch (rCommand.StoredValueType) {
			case RenderCommand::Type::FBatchDC: {
				GetRenderer().Draw(rCommand.Batch, 0, 0, calculatedZcoord, nullptr);
				break;
			}
		}
	}
}

void Engine2D::ExecuteFrame() {
	GameLoopReturnType StoredRenderCommands;
	using Clock = std::chrono::steady_clock;
	auto start = Clock::now();

	glfwPollEvents();
	m_InputController.CaptureKeystates();
	m_UIManager.InterpretInput(m_ElapsedTimeSeconds);

	m_GameLoop(m_ElapsedTimeSeconds, m_InputController.ExposeGameInput(), StoredRenderCommands);

	QueueBatchesToRenderer(StoredRenderCommands);
	m_Renderer.ExecuteDraws();

	auto end = Clock::now();
	m_ElapsedTimeSeconds = std::chrono::duration<float>(end - start).count();
}

/*
remaining:
	cyrillic graphic
	UI ergonomics like auto centering of text within a button
	colour palette-dependant sprites
	playing of sounds

	font file chars are in this order:
	АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯЍ
	абвгдежзийклмнопрстуфхцчшщъьюяѝ
	0123456789
	 .,+-!?;:&><#/
*/
