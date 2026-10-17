#include "util/Input.h"

#include <glfw/glfw3.h>

#include <unordered_map>
#include <unordered_set>

namespace
{
	std::unordered_map<int, bool> d_held; /**< Keys that are currently down. */
	std::unordered_set<int> d_pressed; /**< Keys that went down this frame. */
}

void Input::poll(GLFWwindow* window)
{
	d_pressed.clear();

	if (window == nullptr)
	{
		return;
	}

	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
	{
		const bool down = glfwGetKey(window, key) == GLFW_PRESS;
		if (down && !d_held[key])
		{
			d_pressed.insert(key);
		}
		d_held[key] = down;
	}
}

bool Input::pressed(int key)
{
	return d_pressed.count(key) != 0;
}

bool Input::held(int key)
{
	const auto it = d_held.find(key);
	return it != d_held.end() && it->second;
}