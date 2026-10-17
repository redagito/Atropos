#include "game/MenuShip.h"

#include "renderer/RenderSystem.h"
#include "renderer/Renderable.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

void MenuShip::show(IRenderSystem* renderSystem, const char* modelName)
{
	if (d_ship != nullptr)
	{
		return;
	}

	d_time = 0.f;
	d_ship = renderSystem->add(modelName);
	if (d_ship == nullptr)
	{
		return;
	}
	// A menu model must not throw a shadow onto nothing.
	d_ship->setShadow(false);
	apply(0.f);
}

void MenuShip::update(float dtime)
{
	if (d_ship == nullptr)
	{
		return;
	}
	d_time += dtime;
	apply(d_time * 30.f);
}

void MenuShip::apply(const float& rotationDegrees)
{
	// The camera looks at the middle of the play area, so the model is centred
	// there and enlarged to read as a display rather than a game object.
	const glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 5.f, 0.f));
	const glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(rotationDegrees), glm::vec3(0.f, 1.f, 0.f));
	const glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(1.6f));

	d_ship->setTranslationMatrix(translation);
	d_ship->setRotationMatrix(rotation);
	d_ship->setScaleMatrix(scale);
	d_ship->setModelMatrix(translation * scale * rotation);
}