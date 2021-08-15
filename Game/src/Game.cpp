#include "NothinFancy.h"

using namespace nf;

int main(int argc, char* argv[]) {

	//TODO: Argument parser

	Config conf = { 1280, 720, false, "Test"};
	Application app(conf);
	//app.setWindowIcon(...);
	// app.setWindowCursor(...);
	//Create game states, load some assets, launch loop that continually updates and renders the current state and switches states when appropriate.
	app.showWindow(true);
	app.startLoop();

	return 0;
}