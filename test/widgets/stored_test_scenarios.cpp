//#include "../../components/ui/widget/widgets/window.h"
//#include "../../components/ui/widget/widgets/label.h"
//#include "../../subsystems/ui_manager.h"
//#include "../../engine.h"
//
//void UITestBatch(Engine2D injectedEngine) {
//
//	auto resService = injectedEngine.GetResourceService();
//	auto label = std::make_unique<Label>(injectedEngine.GetTextFactory().GenerateText(U"епично"), glm::vec2(10.f, 10.f), glm::vec2(20, 20));
//
//	//	Seems like BG calculations work well
//	auto window = std::make_unique<Window>(1, glm::vec2(30, 30), glm::vec2(40, 40), injectedEngine.GetResourceService().GetSkinByName("default"));
//	//	Attaching children works well
//	window.get()->AddChild(std::move(label));
//
//	Batch uiBatchTest = Batch(true);
//	uiBatchTest.AddSheetToBatch(injectedEngine.GetResourceService().GetSpriteSheetByName(resService.c_SpecialUISheetName));
//	uiBatchTest.BufferUBOs();
//
//	std::vector<TextWithZLayer> epic;
//
//	/*
//	0,0
//
//
//				30,30	window
//					40,40	label
//	*/
//	window.get()->RenderWidgetTree(
//		&uiBatchTest,
//		epic,
//		glm::vec2(0, 0),
//		2.f,
//		-0.25,
//		0
//	);
//
//	auto clickTest1 = window.get()->DetectClick(
//		glm::vec2(0, 0),
//		glm::vec2(45, 45)
//	);
//
//	auto clickTest2 = window.get()->DetectClick(
//		glm::vec2(0, 0),
//		glm::vec2(10, 10)
//	);
//
//	auto clickTest3 = window.get()->DetectClick(
//		glm::vec2(0, 0),
//		glm::vec2(35, 35)
//	);
//
//
//	UIManager manager = glm::vec2(1600, 900);
//
//	manager.AddChild(std::move(window));
//
//	auto clickTest4 = manager.DetectClick(
//		glm::vec2(0, 0),
//		glm::vec2(500, 500)
//	);
//
//	auto clickTest5 = manager.DetectClick(
//		glm::vec2(0, 0),
//		glm::vec2(35, 35)
//	);
//}