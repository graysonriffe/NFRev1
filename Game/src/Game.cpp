#include "NothinFancy.h"
#include "MainState.h"

int main(int argc, char* argv[]) {
	
	//New scope to test Application destructor
	{
		nf::Config conf = { 1280, 720, false, "Example Game" };
		nf::Application app(conf);
		//app.setWindowIcon(...);
		// app.setWindowCursor(...);

		//Has to be on the heap for some reason
		MainState* test = new MainState;
		app.addState(test, "Main State");
		app.setDefaultState("Main State");

		app.run();

		delete test;
	}

	return 0;
}