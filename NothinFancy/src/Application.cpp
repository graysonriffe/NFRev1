#include "Application.h"
#include "Utility.h"
#include <iostream>

namespace nf {
	DEBUGINIT;

	Application::Application(Config& config) :
		m_currentConfig(config)
	{
		std::cin.get();
	}
}