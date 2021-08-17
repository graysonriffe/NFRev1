#include "NothinFancy.h"

using namespace nf;

int main(int argc, char* argv[]) {

	//TODO: Argument parser

	Config conf = { 1280, 720, false, "Test"};
	Application app(conf);
	//app.setWindowIcon(...);
	// app.setWindowCursor(...);
	//Configure states
	// app.addDefaultState(...);
	app.run();

	return 0;
}