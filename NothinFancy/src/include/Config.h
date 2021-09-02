#pragma once

namespace nf {
	struct Config {
	public:
		unsigned int width;
		unsigned int height;
		bool fullscreen;
		const char* title;
	};
}