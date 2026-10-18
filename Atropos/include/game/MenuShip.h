#pragma once

class Renderable;
class IRenderSystem;

/** \brief Model shown by the non playing states; show() does nothing once one is displayed. */
class MenuShip
{
public:
	/** \brief Shows a model centred in front of the camera. Blocks until it is ready. */
	void show(IRenderSystem* renderSystem, const char* modelName);

	void update(float dtime);

private:
	void apply(const float& rotationDegrees);

	Renderable* d_ship = nullptr; /**< Displayed model, null while nothing is shown. */
	float d_time = 0.f; /**< Accumulated animation time. */
};