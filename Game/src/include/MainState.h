#pragma once
#include "NothinFancy.h"

class MainState : public nf::Gamestate {
public:
	void onEnter() override;

	void update(float deltaTime) override;
	void render(nf::Renderer& renderer) override;

	void onExit() override;
private:
	nf::Camera::Type currCamType;

	nf::AssetPack ap;
	nf::Entity test;
	nf::Entity plane;
	nf::Text text;
	nf::UITexture uiTex;
	nf::Button button;
	nf::Button button2;
	nf::Light light;
	nf::Light light2;
	nf::Light light3;
	nf::Cubemap cm;

	nf::Sound sound;
	nf::Sound sound2;

	std::vector<nf::Entity*> entities;
};