#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void cookTexture(const std::string& in, std::string& out) {
	stbi_set_flip_vertically_on_load(true);

	int x, y, channels;
	unsigned char* texture = stbi_load_from_memory((unsigned char*)&in[0], (int)in.size(), &x, &y, &channels, 0);

	out.append((char*)&x, sizeof(x));
	out.append((char*)&y, sizeof(y));
	out.append((char*)&channels, sizeof(channels));
	out.append((char*)texture, x * y * channels);

	stbi_image_free(texture);
}