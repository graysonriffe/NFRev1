#pragma once
#include "Config.h"
#include <Windows.h>

namespace nf {
	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;
	private:
		Config m_currentConfig;
	};
}