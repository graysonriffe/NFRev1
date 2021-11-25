#include "MainState.h"

void MainState::onEnter() {
	NFLog("MainState onEnter!");
	currCamType = nf::Camera::Type::FIRST_PERSON;
	camera->setType(currCamType);
	ap.load("example.nfpack");
	test.create(ap.get("2mats.obj"), nf::Entity::Type::DYNAMIC);
	test.setPosition(nf::Vec3(0.0, 1.5, -5.0));
	plane.create(ap.get("env.obj"), nf::Entity::Type::MAP);
	plane.setScale(20.0);
	plane.setPosition(0.0, -20.0, 0.0);
	text.create("", nf::Vec2(0.1, 0.025), nf::Vec3(0.8));
	text.centered(true);
	gravText.create("", nf::Vec2(0.025, 0.2), nf::Vec3(0.8), 1.0f, 0.5f);
	uiTex.create(nf::BaseAssets::logo, nf::Vec2(0.025, 0.025), 0.5);
	button.create(nf::Vec2(0.8, 0.025), "Reset");
	button2.create(nf::Vec2(0.6, 0.025), "Play Sound");
	light.create(nf::Vec3(0.0, 20.0, 0.0), nf::Vec3(1.0, 1.0, 1.0));
	light2.create(nf::Vec3(-10.0, 20.0, -10.0), nf::Vec3(1.0, 1.0, 1.0));
	light3.create(nf::Vec3(10.0, 20.0, 10.0), nf::Vec3(1.0, 1.0, 1.0));
	cm.create(nf::BaseAssets::cubemap);

	sound.create(ap.get("sound.wav"));
	sound.setEntity(test);
	sound2.create(ap.get("test.ogg"));
	sound2.setVolume(3.0);
	sound2.setEntity(test);

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				entities.push_back(new nf::Entity);
				entities.back()->create(nf::BaseAssets::cone, nf::Entity::Type::DYNAMIC);
				entities.back()->setPosition(nf::Vec3(5.0 + x * 2.05, 1.0 + y * 2.05, -5.0 + z * 2.05));
			}
		}
	}

	grav = 2.0f;
	setGravity(grav);

	amb = 0.1f;

	camera->setPosition(-20.0, 15.0, 0.0);
	camera->setRotation(85.0, -30.0);
}

void MainState::update(float deltaTime) {
	if (app->isKeyPressed(NFI_E)) {
		currCamType = currCamType == nf::Camera::Type::FIRST_PERSON ? nf::Camera::Type::UI : nf::Camera::Type::FIRST_PERSON;
		camera->setType(currCamType);
	}

	float speed;
	if (camera->getType() == nf::Camera::Type::FIRST_PERSON) {
		if (app->isKeyHeld(NFI_SHIFT))
			speed = 20.0f;
		else
			speed = 5.0f;
		speed *= deltaTime;
		if (app->isKeyHeld(NFI_W))
			camera->moveZ(speed);
		if (app->isKeyHeld(NFI_S))
			camera->moveZ(-speed);
		if (app->isKeyHeld(NFI_D))
			camera->moveX(speed);
		if (app->isKeyHeld(NFI_A))
			camera->moveX(-speed);
	}

	text.setText("FPS: " + std::to_string(app->getFPS()));

	if (button.isClicked() || app->isKeyPressed(NFI_R))
		app->changeState("Main State");

	if (button2.isClicked() || app->isKeyPressed(NFI_SPACE))
		sound.play();
	if (app->isKeyPressed(NFI_M))
		sound2.play();

	if (app->isKeyPressed(NFI_O)) {
		sound.stop();
		sound2.stop();
	}

	if (camera->getType() == nf::Camera::Type::FIRST_PERSON && (app->isMouseClicked(NFI_LEFTMOUSE) || app->isMouseHeld(NFI_RIGHTMOUSE))) {
		entities.push_back(new nf::Entity);
		entities.back()->create(nf::BaseAssets::sphere, nf::Entity::Type::DYNAMIC);
		entities.back()->setPosition(camera->getPosition() + camera->getRotation() * 5.0);
		entities.back()->setVelocity(camera->getRotation() * 100.0f);
		entities.back()->setMass(1000.0f);
	}

	if (app->isKeyHeld(NFI_UP)) {
		grav += 0.05f;
		setGravity(grav);
	}
	if (app->isKeyHeld(NFI_DOWN)) {
		grav -= 0.05f;
		setGravity(grav);
	}
	if (app->isKeyPressed(NFI_T)) {
		grav = 2.0f;
		setGravity(1.0f);
	}
	gravText.setText("Gravity Scale: " + std::to_string(grav));

	if (app->isKeyHeld(NFI_LEFT))
		amb -= 0.01f;
	if (app->isKeyHeld(NFI_RIGHT))
		amb += 0.01f;
	if (amb >= 0.0f)
		setAmbientLight(amb);

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
	renderer.render(gravText);
	renderer.render(uiTex);
	renderer.render(button);
	renderer.render(button2);
	renderer.render(cm);

	for (nf::Entity* curr : entities)
		renderer.render(*curr);
}

void MainState::onExit() {
	NFLog("MainState onExit!");

	entities.clear();
}