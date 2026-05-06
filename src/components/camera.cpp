
#include "camera.h"




void Camera::Initialisation(
	glm::vec2 _position,
	glm::vec2 _viewPortSize

) {
	m_Position = _position;
	DEBUG_WARN(m_Position.x > -FLOAT_COMPARE_TOLERANCE && m_Position.y > -FLOAT_COMPARE_TOLERANCE, "Camera initial position has negative values.")
	m_ViewPortSize = _viewPortSize;
	DEBUG_ASSERT(m_ViewPortSize.x > 0 && m_ViewPortSize.y > 0, "Camera view port size has non-positive values.")

	UpdateProjectionMatrix();
	UpdateViewMatrix();
}


void Camera::UpdateViewMatrix() {	//	reminder:	from +Z to -Z
	m_ViewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-m_Position, 0.f));
}


void Camera::UpdateProjectionMatrix() {
	DEBUG_LOG("Reminder Camera doesn't have configurable NearZ and FarZ");
	m_ProjectionMatrix = glm::ortho(
		0.f,
		m_ViewPortSize.x / m_Zoom,
		m_ViewPortSize.y / m_Zoom,
		0.f,
		-1.f,
		-10.f
	);
}

void Camera::Move(
	glm::vec2 _delta
) {
	m_Position += _delta;
	DEBUG_WARN(m_Position.x > 0 && m_Position.y > 0, "Camera position has negative values.")
	UpdateViewMatrix();
}

void Camera::SetZoom(
	float _zoom
) {
	DEBUG_ASSERT(_zoom > 0, "Camera zoom set to non-positive value.")
	m_Zoom = _zoom;
	UpdateProjectionMatrix();
}

bool Camera::IsInView(
	glm::vec2 _positionInWorld,
	glm::vec2 _objectSize
) {
	DEBUG_ASSERT(_objectSize.x > 0 && _objectSize.y > 0, "Caught object with negative sizes.")

	glm::vec2 camMin = m_Position;
	glm::vec2 camMax = m_Position + m_ViewPortSize;

	glm::vec2 objMin = _positionInWorld;
	glm::vec2 objMax = _positionInWorld + _objectSize;

	bool overlapX = objMax.x >= camMin.x && objMin.x <= camMax.x;
	bool overlapY = objMax.y >= camMin.y && objMin.y <= camMax.y;

	return overlapX && overlapY;
}


glm::vec2 Camera::ScreenToWorldSpaceConversion(
	glm::vec2 _screenPosition
) {
	return GetPosition() + _screenPosition * ( 1.f / m_Zoom);
}


