#include "MainState.h"

void MainState::onEnter() {
	Log("MainState onEnter!");
	camera->setType(nf::Camera::Type::FIRST_PERSON);
	ap.load("example.nfpack");
	test.create(ap["spec.obj"]);
	test.setPosition(0.0, 0.0, -5.0);
	plane.create(nf::BaseAssets::plane);
	plane.setPosition(0.0, -2.0, 0.0);
	plane.setScale(10.0);
	text.create("This is a test text.", nf::Vec2(0.1, 0.025), nf::Vec3(0.8));
	text.centered(true);
	uiTex.create(nf::BaseAssets::logo, nf::Vec2(0.025, 0.025), 0.5);
	button.create(nf::Vec2(0.8, 0.025), "Reset");
	button2.create(nf::Vec2(0.6, 0.025), "Play Sound");
	light.create(nf::Vec3(-5.0, 10.0, 5.0), nf::Vec3(1.0, 1.0, 1.0), 1.0);
	cm.create(nf::BaseAssets::cubemap);

	sound.create(ap["sound.wav"]);
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

	float offset = 0.5f;
	if (app->isKeyHeld(NFI_UP))
		yrot += offset;
	if (app->isKeyHeld(NFI_DOWN))
		yrot -= offset;
	if (app->isKeyHeld(NFI_LEFT))
		xrot += offset;
	if (app->isKeyHeld(NFI_RIGHT))
		xrot -= offset;
	test.setPosition(-xrot / 10.0, 0.0, -yrot / 10.0 - 5.0);

	text.setText("FPS: " + std::to_string(app->getFPS()));

	if (button.isClicked())
		app->changeState("Main State");
	if (button2.isClicked())
		sound.play();

	if (app->isKeyPressed(NFI_ESCAPE))
		app->quit();
}

void MainState::render(nf::Renderer& renderer) {
	renderer.render(test);
	renderer.render(plane);
	renderer.render(light);
	renderer.render(text);
	renderer.render(uiTex);
	renderer.render(button);
	renderer.render(button2);
	renderer.render(cm);
}

void MainState::onExit() {
	Log("MainState onExit!");
	xrot = yrot = 0.0f;
}