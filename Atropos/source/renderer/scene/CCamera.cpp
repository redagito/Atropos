#include "CCamera.h"

#include <glm\ext.hpp>

// Create from view matrix
CCamera::CCamera()
{
	// Default
	setViewMatrix(glm::vec3(0.f, 7.f, -10.f), glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	setProjectionMatrix(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

CCamera::~CCamera()
{
	
}

// Returns view matrix
const glm::mat4& CCamera::getViewMatrix() const
{
	return d_view;
}

// Sets view matrix
void CCamera::setViewMatrix(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
	d_position = eye;
	d_view = glm::lookAt(eye, center, up);
}

// Sets projection matrix, updates members
void CCamera::setProjectionMatrix(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
	// Update member variables
	d_fieldOfView = fieldOfView;
	d_aspectRatio = aspectRatio;
	d_zNear = zNear;
	d_zFar = zFar;
	d_proj = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

// Returns projection matrix
const glm::mat4& CCamera::getProjectionMatrix() const
{
	return d_proj;
}

float CCamera::getAspectRatio() const
{
	return d_aspectRatio;
}

float CCamera::getFieldOfView() const
{
	return d_fieldOfView;
}

float CCamera::getZNear() const
{
	return d_zNear;
}

float CCamera::getZFar() const
{
	return d_zFar;
}

// Returns the current position
const glm::vec3& CCamera::getPosition() const
{
	return d_position;
}