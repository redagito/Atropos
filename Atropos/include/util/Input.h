#pragma once

struct GLFWwindow;

/**
 * \brief Edge and level triggered keyboard state.
 * glfwGetKey reports only the current level, so the previous frame's state is
 * kept to recover the press edge.
 */
class Input
{
public:
	/**
	* \brief Samples the current state of every key.
	* Must run once per frame after glfwPollEvents and before any keyboard read.
	*/
	static void poll(GLFWwindow* window);

	static bool pressed(int key);

	static bool held(int key);
};