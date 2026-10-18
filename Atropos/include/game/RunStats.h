#pragma once

/**
* \brief Values that outlive the state that produced them, so the end screens
* can show what the run scored. Reset by the engine on init.
*/
struct RunStats
{
	int score = 0; /**< Enemies destroyed this run. */
};