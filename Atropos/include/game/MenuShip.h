#pragma once

class Renderable;
class IRenderSystem;

/**
 * \brief A model displayed in the centre of the view by a non playing state.
 *
 * Used to give the load, win and lose screens something to show. The model is
 * placed once and then only rotates, so calling show again while it is already
 * visible does nothing.
 */
class MenuShip
{
public:
	/**
	* \brief Shows a model centred in front of the camera.
	*
	* Does nothing if a model is already shown. Loading the model blocks until
	* it is ready, so only call this once the resources have finished loading.
	*/
	void show(IRenderSystem* renderSystem, const char* modelName);

	/**
	* \brief Advances the idle rotation.
	*/
	void update(float dtime);

private:
	/**
	* \brief Applies the current transform to the model.
	*/
	void apply(const float& rotationDegrees);

	Renderable* d_ship = nullptr; /**< Displayed model, null while nothing is shown. */
	float d_time = 0.f; /**< Accumulated animation time. */
};