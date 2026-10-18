#include "renderer/scene/Camera.h"

#include <glm/ext.hpp>

Camera::Camera()
{
	resetView();
	setProjectionMatrix(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

void Camera::resetView()
{
	setViewMatrix(glm::vec3(0.f, 7.f, -10.f), glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}

Camera::~Camera()
{
	
}

const glm::mat4& Camera::getViewMatrix() const
{
	return d_view;
}

void Camera::setViewMatrix(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
	d_position = eye;
	d_view = glm::lookAt(eye, center, up);
}

void Camera::setProjectionMatrix(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
	d_fieldOfView = fieldOfView;
	d_aspectRatio = aspectRatio;
	d_zNear = zNear;
	d_zFar = zFar;
	d_proj = glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
}

const glm::mat4& Camera::getProjectionMatrix() const
{
	return d_proj;
}

float Camera::getFieldOfView() const
{
	return d_fieldOfView;
}

float Camera::getZNear() const
{
	return d_zNear;
}

float Camera::getZFar() const
{
	return d_zFar;
}

const glm::vec3& Camera::getPosition() const
{
	return d_position;
}