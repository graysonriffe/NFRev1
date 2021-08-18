#include "NothinFancy.h"
#include "MainState.h"

using namespace nf;

int main(int argc, char* argv[]) {

	//TODO: Argument parser

	Config conf = { 1280, 720, false, "Test"};
	Application app(conf);
	//app.setWindowIcon(...);
	// app.setWindowCursor(...);
	//Configure states
	// app.addDefaultState(...);

	MainState* test = new MainState;
	app.addState("Main State", test);
	app.addDefaultState("Main State");

	app.run();

	return 0;
}