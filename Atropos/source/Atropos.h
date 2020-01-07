#pragma once

#include "engine/IEngine.h"

/**
* \brief Creation and destruction of engine interface.
* TODO: Create engine as dynamic lib instead of static.
*/
extern "C"
{
	/**
	* \brief Creates engine interface object.
	*/
	IEngine* createEngine();

	/**
	* \brief Destroys previously created engine object.
	*/
	void destroyEngine(IEngine*);
}