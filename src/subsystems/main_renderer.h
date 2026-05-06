#pragma once

#include <vector>
#include <queue>

#include "resource_service.h"

#include "../components/camera.h"
#include "../components/input_controller.h"
#include "../components/file_handler.h"

#include "../opengl/batch.h"

#include "../components/ui/text.h"

struct RenderCommand {
	enum class Type {
		FBatchDC,
		TextDC
	} StoredValueType;

	Batch* BatchObj;

	unsigned int IssuedZLayer;
};

struct GameLoopReturnType {
	std::vector<RenderCommand> RenderCommands;

	void QueueRenderObject(Batch* ptr, unsigned int zLayer);
};

struct DrawCall {
protected:
	float m_xScreenCoord;
	float m_yScreenCoord;
	float m_zLayer;
	size_t m_MaxInstanceCapacity = 0;

	//	If value is left at nullptr, no additional uniforms will be applied
	const UniformDataVector* m_AppliedUniforms = nullptr;

public:
	DrawCall(float x, float y, float z, const UniformDataVector* _uniformDataArray, size_t _instances)
		: m_xScreenCoord(x), m_yScreenCoord(y), m_zLayer(z), m_AppliedUniforms(_uniformDataArray), m_MaxInstanceCapacity(_instances)
	{}

	const UniformDataVector* GetAppliedUniforms() const { return m_AppliedUniforms; }
	glm::vec3 GetPositionVector() const { return glm::vec3(m_xScreenCoord, m_yScreenCoord, m_zLayer); }
	const size_t GetInstances() const { return m_MaxInstanceCapacity; }
};

//		BATCHES

struct BatchDrawCall : public DrawCall {
private:
	const Batch* m_Base = nullptr;
public:
	BatchDrawCall(const Batch* _softBatch, float x, float y, float z, const UniformDataVector* _uniformDataArray, size_t _instances)
		: m_Base(_softBatch), DrawCall(x, y, z, _uniformDataArray, _instances)
	{
	}
	const Batch* GetBaseBatchPointer() const { return m_Base; }
};

class Renderer2D {
private:	//	Window-related information

	int m_ScreenWidth = -1;
	int m_ScreenHeight = -1;
	std::string m_WindowTitle = "";

	bool m_Fullscreen = false;

	bool m_IsRunning = false;
	GLFWwindow* m_MainWindowHandle = nullptr;

	const InputController* m_InputController = nullptr;
	const ResourceService* m_ResService = nullptr;

private:	//	Logical components

	StandardQuad m_StandardQuad;

	Camera m_Camera;

private:

	bool m_HasClickedThisFrame = false;

public:

	const bool HasClicked();

private:	//	Structures for draw queue optimisation

	std::vector<BatchDrawCall> m_BatchArray;

private:	//	Methods to render queued Drawcalls

	void RenderBatches();

public:		//	Exposed functions

	Renderer2D() {}

	
	Renderer2D(
		GLFWwindow* _initialisedWindow,
		int _screenWidth,
		int _screenHeight,
		const char* _windowTitle,
		bool _fullscreen = false
	);

	
	//	Initialise window and global variables.
	//	Returns false on fail.
	bool Init();

	//	Execute all draw calls.
	void ExecuteDraws();

	void Draw(
		Batch* _batch,
		float _xPosition,
		float _yPosition,
		float _zLayer,
		UniformDataVector* _uniformArray
	);

private:

	void PerClassVAOinitialisationFunction();

public:

	void SendStandardUniforms(
		const Shader* _targetShader,
		const glm::vec3& _positionVector
	) const;

	//	Gets the single instance governing OpenGL objects like 
	//	VBOs, IBO and VAO used for rendering
	GLFWwindow*			GetWinHandle() const { return m_MainWindowHandle; }	// always required to be non-const
	Camera&				GetCamera() { return m_Camera; }


	const Camera&			GetCamera() const { return m_Camera; }


	const InputController* GetInput() const { return m_InputController; }
	const ResourceService* GetResources() const { return m_ResService; }

	void SetInputController(const InputController* input) { m_InputController = input; }
	void SetResService(const ResourceService* res) { m_ResService = res; }

	StandardQuad& GetQuad() { return m_StandardQuad; }

	float GetCloserZCoord() const { return 1.f; }
	float GetFarZCoord() const { return 10.f; }
};