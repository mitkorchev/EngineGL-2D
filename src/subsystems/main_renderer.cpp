#include "main_renderer.h"


void Renderer2D::Draw(
	Batch* _batch,
	float _xPosition,
	float _yPosition,
	float _zLayer,
	UniformDataVector* _uniformArray
) {
	m_BatchArray.emplace_back(
		_batch,
		_xPosition,
		_yPosition,
		_zLayer,
		_uniformArray,
		_batch->SendSpriteDataToGPU()
	);
}

void Renderer2D::ExecuteDraws() {

#ifdef DEBUG__CODE

	// easily recognisable when some sprite is black or at all drawn
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

#else

	glClearColor(0.f, 0.f, 0.f, 1.f);

#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	-- RENDERING STARTS --	//

	RenderBatches();

	//	-- RENDERING ENDS	--	//

	glfwSwapBuffers(GetWinHandle());
}

void Renderer2D::RenderBatches() {
	std::vector<BatchDrawCall>& Arr = m_BatchArray;

	if (Arr.empty()) return;

	Batch::BindCommonVAO();

	for (size_t i = 0; i < Arr.size(); i++) {

		const BatchDrawCall& BatchDrawCallObject = Arr[i];
		const Batch* BatchObject = BatchDrawCallObject.GetBaseBatchPointer();
		const Shader* ShaderObject = BatchObject->GetSpecialSheetPointer()->GetShader();

		size_t InstanceCount = BatchDrawCallObject.GetInstances();

		BatchObject->BindUniqueBuffer();

		ShaderObject->UseShader();
		SendStandardUniforms(ShaderObject, BatchDrawCallObject.GetPositionVector());

		ShaderObject->ApplyUniforms(BatchDrawCallObject.GetAppliedUniforms());

		BatchObject->ActivateTextures("u_Textures");
		BatchObject->BindUBOs();
		

#ifdef DEBUG__CODE
		CheckGLErrors();
#endif
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, static_cast<int>(InstanceCount));

#ifdef DEBUG__CODE
		CheckGLErrors();
#endif
	}

	Batch::UnbindCommonVAO();
	Arr.clear();
}

void Renderer2D::SendStandardUniforms(
	const Shader* _targetShader,
	const glm::vec3& _positionVector
)  const {
	_targetShader->SetStandardModel(glm::translate(glm::mat4(1.f), _positionVector));
	_targetShader->SetStandardView(GetCamera().GetViewMatrix());
	_targetShader->SetStandardProjection(GetCamera().GetProjectionMatrix());
}

bool Renderer2D::Init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	Component initialisation

	const glm::vec2 DefaultCameraLocation = glm::vec2(0.f, 0.f);
	const glm::vec2 DefaultViewportDimensions = glm::vec2(m_ScreenWidth, m_ScreenHeight);

	m_Camera.Initialisation(DefaultCameraLocation, DefaultViewportDimensions);

	//	Globals initialisation

	g_StandardQuad.Init();

	PerClassVAOinitialisationFunction();

	return true;
}

void Renderer2D::PerClassVAOinitialisationFunction() {
	Batch::InitialiseCommonVAO();
}

Renderer2D::Renderer2D(
	GLFWwindow* _initialisedWindow,
	int _screenWidth,
	int _screenHeight,
	const char* _windowTitle,
	bool _fullscreen
):
	m_MainWindowHandle(_initialisedWindow),
	m_ScreenWidth(_screenWidth),
	m_ScreenHeight(_screenHeight),
	m_WindowTitle(_windowTitle),
	m_Fullscreen(_fullscreen)
{}

const bool Renderer2D::HasClicked() {
	return m_HasClickedThisFrame;
}


void GameLoopReturnType::QueueRenderObject(Batch* ptr, unsigned int zLayer){
	RenderCommand self;
	self.Batch = ptr;
	self.StoredValueType = RenderCommand::Type::FBatchDC;
	self.IssuedZLayer = zLayer;

	RenderCommands.emplace_back(self);
}