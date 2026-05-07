#pragma once


#include "../common/common.h"


/*
*	Camera 2D:
*	With Y increasing toward bottom of the screen.
*
*/

class Camera {

	glm::vec2 m_Position;
	glm::vec2 m_ViewPortSize;
	float m_Zoom = 1.f;

	//	This is useless here.
	//glm::mat4 m_ModelMatrix;		//	model		-> world space
	glm::mat4 m_ViewMatrix;			//	world space -> view space (POV of camera)
	glm::mat4 m_ProjectionMatrix;	//	view space	-> 2d screen coordinates


public:

	Camera() {}

	void Initialisation(
		glm::vec2 _position,
		glm::vec2 _viewPortSize
	);

	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	
	void Move(
		glm::vec2 _delta
	);

	
	bool IsInView(
		glm::vec2 _positionInWorld,
		glm::vec2 _objectSize = { 0.f, 0.f }
	);

	
	glm::vec2 ScreenToWorldSpaceConversion(
		glm::vec2 _screenPosition
	);

public:

	void SetZoom(
		float _zoom
	);

	void SetPosition(glm::vec2 _newPosition) { m_Position = _newPosition; }

	glm::vec2 GetPosition() const { return m_Position; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
};

