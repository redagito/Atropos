#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	/**
	* \brief Sets the view and projection matrices.
	*/
	Camera();

	~Camera();

	const glm::mat4& getViewMatrix() const;

	void setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

	/**
	* \brief Also updates zNear/zFar, which the scene culling test reads.
	*/
	void setProjectionMatrix(float fieldOfView, float aspectRatio, float zNear, float zFar);

	/**
	* \brief Returns current field of view in degree
	*/
	float getFieldOfView() const;
	
	float getZNear() const;

	float getZFar() const;

	const glm::vec3& getPosition() const;

	const glm::mat4& getProjectionMatrix() const;

	/**
	* \brief Restores the constructor framing. Leaves the projection alone,
	* it belongs to the window rather than to the view.
	*/
	void resetView();

private:
	glm::vec3 d_position;
	glm::mat4 d_view;
	glm::mat4 d_proj;

	float d_fieldOfView;
	float d_aspectRatio;
	float d_zNear;
	float d_zFar;
};