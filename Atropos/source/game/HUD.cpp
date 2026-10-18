#include "game/HUD.h"

#include "renderer/ui/UI.h"

#include <algorithm>
#include <string>

namespace
{
	constexpr float TEXT_SCALE = 1.f;
	const glm::vec4 TEXT_COLOR(1.f, 1.f, 1.f, 1.f);
	const glm::vec4 ACCENT_COLOR(0.6f, 0.9f, 1.f, 1.f);
	const glm::vec4 TRACK_COLOR(0.14f, 0.14f, 0.16f, 1.f);
	const glm::vec4 HEALTH_COLOR(0.85f, 0.15f, 0.15f, 1.f);

	// The bar covers two fifths of the width, leaving the scene visible either side.
	constexpr float BAR_WIDTH_FRACTION = 0.4f;
	constexpr float BAR_HEIGHT = 14.f;
	constexpr float BAR_BOTTOM_MARGIN = 20.f;
}

void HUD::drawFps(UI& ui)
{
	const std::string text = "FPS " + std::to_string(ui.getFps());
	const float width = ui.measureText(text, TEXT_SCALE);
	ui.drawText(glm::vec2(static_cast<float>(ui.getWidth()) - width - 16.f, 14.f), text, TEXT_SCALE, TEXT_COLOR);
}

void HUD::drawCentredText(UI& ui, float y, const std::string& text, float scale, const glm::vec4& color)
{
	const float width = ui.measureText(text, scale);
	ui.drawText(glm::vec2((static_cast<float>(ui.getWidth()) - width) * 0.5f, y), text, scale, color);
}

void HUD::drawBar(UI& ui, float y, float height, float fraction, const glm::vec4& fill)
{
	const float clamped = std::clamp(fraction, 0.f, 1.f);
	const float barWidth = static_cast<float>(ui.getWidth()) * BAR_WIDTH_FRACTION;
	const float left = (static_cast<float>(ui.getWidth()) - barWidth) * 0.5f;

	ui.drawRect(glm::vec2(left, y), glm::vec2(barWidth, height), TRACK_COLOR);
	ui.drawRect(glm::vec2(left, y), glm::vec2(barWidth * clamped, height), fill);
}