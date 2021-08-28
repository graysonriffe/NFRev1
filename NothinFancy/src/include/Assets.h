#pragma once
#include <unordered_map>
#include <vector>

namespace nf {
	struct Asset {
		virtual ~Asset();
	};

	struct AGeometry : Asset {
		const float* m_vertexBufferData;
		const unsigned int* m_indexBufferData;
		const float* m_textureCoordinatesBufferData;
		~AGeometry() override;
	};

	struct ATexture : Asset {
		const void* m_data;
		~ATexture() override;
	};

	class AssetPack {
	public:
		AssetPack();

		void load(const char* packName);

		~AssetPack();
	private:
		std::unordered_map<std::string, Asset*> m_assets;
	};
}