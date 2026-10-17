#include "game/control/PlayerMovementController.h"

#include "game/GameObject.h"

#include "util/Input.h"
#include "glfw/glfw3.h"

#include <glm/ext.hpp>

#include <cmath>

PlayerMovementController::PlayerMovementController(float speedSide)
	:
	d_speedSide(speedSide),
	d_active(true),
	d_object(nullptr),
	d_rotationDegree(0.f)
{
	return;
}

PlayerMovementController::~PlayerMovementController()
{
	return;
}

void PlayerMovementController::attach(GameObject* object)
{
	d_object = object;
}

void PlayerMovementController::detach()
{
	d_object = nullptr;
	d_rotationDegree = 0.f;
}

void PlayerMovementController::setActive(bool state)
{
	d_active = state;
}

void PlayerMovementController::update(float dtime)
{
	if (d_active && d_object != nullptr)
	{
		// Rotation speed by degree / sec
		float rateOfRotation = 300.f;
		// Back rotation when no keys are pressed
		float backRotation = 500.f;

		// X axis boundary for movement
		float minX = -20.f;
		float maxX = 20.f;
		// Y axis boundary for movement
		float minY = 3.f;
		float maxY = 20.f;

		glm::vec4 temp = (d_object->getTranslation() * glm::vec4(0.f, 0.f, 0.f, 1.f));
		glm::vec3 pos = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);

		// Movement vector
		glm::vec3 dPos(0.f);

		// Move up
		if (Input::held(GLFW_KEY_UP))
		{
			// Y new position
			dPos.y = dtime * d_speedSide;
			if (dPos.y + pos.y > maxY)
			{
				// Move to limit
				dPos.y = maxY - pos.y;
			}
		}

		// Move down
		if (Input::held(GLFW_KEY_DOWN))
		{
			// Y new position
			dPos.y = -dtime * d_speedSide;
			if (dPos.y + pos.y < minY)
			{
				// Move to limit
				dPos.y = minY - pos.y;
			}
		}

		// Rotate right
		if (Input::held(GLFW_KEY_RIGHT))
		{
			d_rotationDegree += dtime * rateOfRotation;
			if (d_rotationDegree > 90.f)
			{
				d_rotationDegree = 90.f;
			}
		}
		else if (d_rotationDegree > 0.f)
		{
			d_rotationDegree -= dtime * backRotation;
			if (d_rotationDegree < 0.f)
			{
				d_rotationDegree = 0.f;
			}
		}

		// Rotate left
		if (Input::held(GLFW_KEY_LEFT))
		{
			d_rotationDegree -= dtime * rateOfRotation;
			if (d_rotationDegree < -90.f)
			{
				d_rotationDegree = -90.f;
			}
		}
		else if (d_rotationDegree < 0.f)
		{
			d_rotationDegree += dtime * backRotation;
			if (d_rotationDegree > 0.f)
			{
				d_rotationDegree = 0.f;
			}
		}

		// Movement speed based on angle
		float dx = -d_rotationDegree * d_speedSide;

		// Restrict movement
		if (pos.x > minX && dx < 0.f)
		{
			dPos.x = -std::sqrt(-dx) * dtime;
			if (dPos.x + pos.x < minX)
			{
				dPos.x = minX - pos.x;
			}
		}
		else if (pos.x < maxX && dx > 0.f)
		{
			dPos.x = std::sqrt(dx) * dtime;
			if (dPos.x + pos.x > maxX)
			{
				dPos.x = maxX - pos.x;
			}
		}

		// Update rotation
		d_object->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(d_rotationDegree), glm::vec3(0.f, 0.f, 1.f)));

		// Update translation
		d_object->setTranslation(glm::translate(d_object->getTranslation(), dPos));
	}
}