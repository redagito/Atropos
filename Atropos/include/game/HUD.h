#pragma once

#include <glm/glm.hpp>

#include <string>

class UI;

/**
* \brief Shared head-up display drawing, so the states only decide what to show.
*/
namespace HUD
{
	/** \brief Draws the frame rate readout in the top right corner. */
	void drawFps(UI& ui);

	/**
	* \brief Draws text over a translucent grey panel sized to fit it, so a
	* readout stays legible over a bright or busy scene.
	*/
	void drawLabel(UI& ui, const glm::vec2& position, const std::string& text, float scale, const glm::vec4& color);

	/** \brief Draws text horizontally centred in the interface. */
	void drawCentredText(UI& ui, float y, const std::string& text, float scale, const glm::vec4& color);

	/**
	* \brief Draws a filled bar with a dark track behind it, centred horizontally.
	* \param fraction clamped to 0..1.
	*/
	void drawBar(UI& ui, float y, float height, float fraction, const glm::vec4& fill);
}