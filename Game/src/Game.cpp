#include "NothinFancy.h"
#include "MainState.h"

using namespace nf;

int main(int argc, char* argv[]) {
	Config conf = { 1280, 720, false, "Example Game"};
	Application app(conf);
	//app.setWindowIcon(...);
	// app.setWindowCursor(...);

	MainState* test = new MainState(&app);
	app.addState(test, "Main State");
	app.addDefaultState("Main State");

	app.run();

	return 0;
}