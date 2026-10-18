#pragma once

class IEngine
{
public:
	virtual ~IEngine();

	/** \brief Initializes engine and subsystems; throws std::runtime_error on failure. */
	virtual void init(const char* configFile) = 0;

	virtual void run() = 0;

	virtual void cleanup() = 0;
};
