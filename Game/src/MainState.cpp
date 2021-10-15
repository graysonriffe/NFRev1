#include "MainState.h"

void MainState::onEnter() {
	Log("MainState onEnter!");
	camera->setType(nf::Camera::Type::FIRST_PERSON);
	ap.load("example.nfpack");
	test.create(ap["2mats.obj"]);
	test.setPosition(0.0, 0.05, -5.0);
	plane.create(nf::BaseAssets::cube);
	plane.setPosition(0.0, -1.0, 0.0);
	plane.setScale(50.0, 0.05, 50.0);
	text.create("This is a test text.", nf::Vec2(0.1, 0.025), nf::Vec3(0.8));
	text.centered(true);
	uiTex.create(nf::BaseAssets::logo, nf::Vec2(0.025, 0.025), 0.5);
	button.create(nf::Vec2(0.8, 0.025), "Reset");
	button2.create(nf::Vec2(0.6, 0.025), "Play Sound");
	light.create(nf::Vec3(0.0, 20.0, 0.0), nf::Vec3(1.0, 1.0, 1.0));
	light2.create(nf::Vec3(-10.0, 20.0, -10.0), nf::Vec3(1.0, 1.0, 1.0));
	light3.create(nf::Vec3(10.0, 20.0, 10.0), nf::Vec3(1.0, 1.0, 1.0));
	cm.create(nf::BaseAssets::cubemap);

	sound.create(ap["sound.wav"]);
	sound.setEntity(test);

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
				entities.push_back(new nf::Entity);
				entities.back()->create(ap["2mats.obj"]);
				entities.back()->setPosition(5.0 + x * 2.1, 0.05, -5.0 + y * 2.1);
		}
	}
}

void MainState::update(double deltaTime) {
	if (app->isKeyPressed(NFI_U))
		camera->setType(nf::Camera::Type::UI);
	if (app->isKeyPressed(NFI_P))
		camera->setType(nf::Camera::Type::FIRST_PERSON);

	double speed = 5.0;
	if (camera->getType() == nf::Camera::Type::FIRST_PERSON) {
		if (app->isKeyHeld(NFI_SHIFT))
			speed = 20.0;
		else
			speed = 5.0;
		if (app->isKeyHeld(NFI_W))
			camera->moveForward(speed * deltaTime);
		if (app->isKeyHeld(NFI_S))
			camera->moveBackward(speed * deltaTime);
		if (app->isKeyHeld(NFI_D))
			camera->moveRight(speed * deltaTime);
		if (app->isKeyHeld(NFI_A))
			camera->moveLeft(speed * deltaTime);
	}

	static double offset = 0.0;
	if (app->isKeyHeld(NFI_UP))
		offset += 2.0 * deltaTime;
	if (app->isKeyHeld(NFI_DOWN))
		offset -= 2.0 * deltaTime;
	test.setRotation(0.0, 0.0, -offset * 20.0);

	light.setPosition(nf::Vec3(std::sin(circle) * 10.0, 5.0, std::cos(circle) * 10.0));
	circle += 2.0 * deltaTime;

	text.setText("FPS: " + std::to_string(app->getFPS()));

	if (button.isClicked())
		app->changeState("Main State");
	if (button2.isClicked() || app->isKeyHeld(NFI_SPACE))
		sound.play(true);

	if (app->isKeyPressed(NFI_O))
		sound.stop();

	if (app->isKeyPressed(NFI_ESCAPE))
		app->quit();
}

void MainState::render(nf::Renderer& renderer) {
	renderer.render(test);
	renderer.render(plane);
	renderer.render(light);
	renderer.render(light2);
	renderer.render(light3);
	renderer.render(text);
	renderer.render(uiTex);
	renderer.render(button);
	renderer.render(button2);
	renderer.render(cm);

	for (nf::Entity* curr : entities)
		renderer.render(*curr);
}

void MainState::onExit() {
	Log("MainState onExit!");
	circle = 0.0;

	entities.clear();
}