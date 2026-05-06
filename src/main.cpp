#include "engine.h"
#include "components/ui/widget/widgets/label.h"
#include "components/ui/widget/widgets/window.h"
#include "components/ui/widget/widgets/button.h"

#include <print>

int main(int argc, char** argv) {

	Engine2D eng(1600, 900, "nog", false);
	auto& resService = eng.GetResourceService();

	resService.UploadShaderParameters("test\\res\\batch.shader",		"batch");		//	WITH UBO


	resService.UploadSpriteSheetParameters("test\\res\\cyrillic_print_big.png", "cyrillic_print_big",		"batch",		10, 10, true, 4);
	
	resService.UploadSpriteSheetParameters("test\\res\\test.cfg",		"testSheet",						"batch",		0, 0);
	resService.UploadSpriteSheetParameters("test\\res\\panda.cfg",		"pandaSheet",						"batch",		0, 0);
	resService.UploadSpriteSheetParameters("test\\res\\gui.cfg",		resService.c_SpecialUISheetName,	"batch",		0, 0, true);
	
	resService.UploadFontParameters("cyrillic_print_big", "test\\res\\ui\\cyrillic_print_big.font");

	eng.Init();
	
	const bool INIT_OPENGL_OBJECTS = true;
	Batch freebatch = Batch(INIT_OPENGL_OBJECTS);
	freebatch.AddSheetToBatch(eng.GetResourceService().GetSpriteSheetByName("testSheet"));
	freebatch.AddSheetToBatch(eng.GetResourceService().GetSpriteSheetByName("pandaSheet"));
	freebatch.BufferUBOs();

	float x = 150;
	float y = 101;
	float Rotation = 0.f;
	
	SpriteInstance instance = freebatch.GetSprite("pandaSheet", "nose");
	SpriteInstance eye = freebatch.GetSprite("pandaSheet", "eye");
	SpriteInstance testedInstance = freebatch.GetSprite("testSheet", "nose");
	

	constexpr float oneDef = 1.f / 3.1418f;

	TextOptions options = resService.GetUIBatch()->GetTextOptionsForFont("cyrillic_print_big");
	options.scale = 0.7f;

	Text txt = Text(
		U"ЕДНО",
		options
	);

	Text txt2 = Text(
		U"ДВЕ",
		options
	);

	Text btntxt = Text(
		U"ЗАТВОРИ",
		options
	);

	Text emptyText = Text(U"", options);

	auto label = std::make_unique<Label>(txt, glm::vec2(10.f, 10.f));
	auto window = eng.GetUIManager().GenWindowObject(glm::vec2(300, 200));

	window.get()->AddChild(std::move(label));

	auto label2 = std::make_unique<Label>(txt2, glm::vec2(10.f, 10.f));
	auto window2 = eng.GetUIManager().GenWindowObject(glm::vec2(300, 200));
	
	auto input2 = std::make_unique<Input>(glm::vec2(10.f, 90.f), glm::vec2(215.f, 30.f), emptyText, resService.GetBgSkinByName("default"));
	Input* input2ptr = input2.get();

	auto btn2 = std::make_unique<Button>(glm::vec2(10.f, 40.f), glm::vec2(150.f, 30.f), btntxt, resService.GetBgSkinByName("default"));
	btn2.get()->SetOnClick([input2ptr](EventEmitter* ctx, Window* win) {
		std::print("Input:");
		auto* str = input2ptr->GetValue();
		for (size_t i = 0; i < str->size(); i++) {
			std::print(" {}", int(str->at(i)));
		}
		std::println();
		input2ptr->ClearText();
	});

	auto btn3 = std::make_unique<Button>(glm::vec2(10.f, 40.f), glm::vec2(150.f, 30.f), btntxt, resService.GetBgSkinByName("default"));
	btn3.get()->SetOnClick([](EventEmitter* ctx, Window* win) {
		std::print("Button clicked");
	});
	eng.GetUIManager().AddChild(std::move(btn3));

	window2.get()->AddChild(std::move(label2));
	window2.get()->AddChild(std::move(btn2));
	window2.get()->AddChild(std::move(input2));

	eng.GetUIManager().OpenWindow(std::move(window), glm::vec2(300, 400));
	eng.GetUIManager().OpenWindow(std::move(window2), glm::vec2(350, 350));

	eng.SetGameLoop([&](float elapsedTimeSeconds, GameInput input, GameLoopReturnType& renderComms) {
		renderComms.QueueRenderObject(&freebatch, 2);

		//std::println("FPS: {:.0f}", 1 / elapsedTimeSeconds); 

		if (input.IsHeld(GLFW_KEY_LEFT)) {
			x -= 1;
			std::println("NEW X: {}", x);
		}

		if (input.IsHeld(GLFW_KEY_RIGHT)) {
			x += 1;
			std::println("NEW X: {}", x);
		}

		if (input.IsHeld(GLFW_KEY_R)) {
			Rotation += oneDef;
		}

		if (input.IsHeld(GLFW_KEY_DOWN)) {
			instance.dimensions.x += 5;
		}

		freebatch.DrawSprite(instance, x, y, Rotation);
		freebatch.DrawSprite(eye, x, y + 200, Rotation);

		//	Testing cuts
		SpriteInstance testEye = eye;

		testEye = eye;
		testEye.SetXCutPixels(10, false);
		testEye.SetYCutPixels(10, false);
		freebatch.DrawSprite(testEye, x, y + 260, Rotation);

		testEye = eye;
		float cut = 15.f;
		testEye.SetXCutPixels(cut, true);
		freebatch.DrawSprite(testEye, x + cut, y + 360, Rotation);
	});

	while (eng.IsRunning()) {
		eng.ExecuteFrame();
	}

	return 0;
}