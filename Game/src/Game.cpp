#include "NothinFancy.h"
#include "MainState.h"

int main(int argc, char* argv[]) {
	nf::Config conf = { 1280, 720, false, "Example Game"};
	nf::Application app(conf);
	//app.setWindowIcon(...);
	// app.setWindowCursor(...);

	MainState* test = new MainState(&app);
	app.addState(test, "Main State");
	app.addDefaultState("Main State");

	app.run();

	return 0;
}