#pragma once

#include "glm/glm.hpp"

/**
* \brief Represents active camera.
*
* Stores view matrix.
*/
class CCamera
{
public:
	/**
	* \brief Sets the view matrix.
	*/
	CCamera();

	/**
	* \brief Deletes resources
	*/
	~CCamera();

	/** 
	* \brief Read access to the view matrix.
	*/
	const glm::mat4& getViewMatrix() const;

	/**
	* \brief Sets view matrix
	*/
	void setViewMatrix(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

	/**
	* \brief Sets projection matrix
	*/
	void setProjectionMatrix(float fieldOfView, float aspectRatio, float zNear, float zFar);

	/**
	* \brief Returns current aspect ratio
	*/
	float getAspectRatio() const;

	/**
	* \brief Returns current field of view in degree
	*/
	float getFieldOfView() const;
	
	/**
	* \brief Returns current z near
	*/
	float getZNear() const;

	/**
	* \brief Returns current z far
	*/
	float getZFar() const;

	/**
	* \brief Returns the camera position
	*/
	const glm::vec3& getPosition() const;

	/**
	* \brief Returns projection matrix
	*/
	const glm::mat4& getProjectionMatrix() const;

private:
	glm::vec3 d_position; /**< Camera position. */
	glm::mat4 d_view; /**< View matrix. */
	glm::mat4 d_proj; /**< Projection matrix. */

	float d_fieldOfView; /**< Current field of view. */
	float d_aspectRatio; /**< Current aspect ratio. */
	float d_zNear; /**< Current z near. */
	float d_zFar; /**< Current z far. */
};