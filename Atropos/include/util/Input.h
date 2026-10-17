#pragma once

/**
 * \brief Edge and level triggered keyboard state.
 *
 * glfwGetKey only reports whether a key is down right now, which cannot tell a
 * fresh press from a key that is still held. Holding the key that ends a game
 * state would then trigger the transition again on every following frame. This
 * class compares the current state against the previous frame to recover the
 * press edge.
 */
class Input
{
public:
	/**
	* \brief Samples the current state of every key.
	* Must be called once per frame after the window events have been polled and
	* before any state reads the keyboard.
	*/
	static void poll();

	/**
	* \brief True on the frame the key went down, false otherwise.
	*/
	static bool pressed(int key);

	/**
	* \brief True while the key is held down.
	*/
	static bool held(int key);
};