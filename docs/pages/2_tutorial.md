@page tutorial Tutorial
@tableofcontents

This tutorial aims to teach the basics of the engine and how to use it.

First, follow the steps on the @ref install page. Once the template MSVC project is setup,
you can begin here.

@section nfArch Engine Architecture

An NF app is made up of a set of states represented by the nf::Gamestate class. When
an nf::Application is running, it is either running a state or loading one. Below is an
image that describes what the main thread of an NF app would be typically doing in
the program's lifetime.

@image html applifetime.png "The lifetime of a typical NF app" width=20%

Using the engine's architecture, you might not even write any functions that are called
from `main`. Most of the code that programs the engine's behavior should be called in
your state's [update function](@ref nf::Gamestate::update).

To allow a translate unit to use the engine, you must include `NothinFancy.h`. This
header contains every class and function.

@section createConfig Creating a Config

The first step to creating an app is creating an nf::Config. A config describes how the
engine should display on the screen. nf::Config has these fields:

- `width` - The width of the window if `fullscreen` is set to `false`
- `height` - The height of the window if `fullscreen` is set to `false`
- `fullscreen` - `true` sets the display to the size of the monitor the app is
- `title` - The title of the window shown on the caption bar and taskbar
opened on

To create a 1280 by 720 window with a title of "NF Example", you would write:

~~~cpp
nf::Config conf;
conf.width = 1280;
conf.height = 720;
conf.fullscreen = false;
conf.title = "NF Example";

//Or...

nf::Config conf2{ 1280, 720, false, "NF Example" };
~~~

We then pass this config to an nf::Application

@section createApp Creating and Configuring an Application

The nf::Application class represents an instance of the engine. This is the point where
you will attach your states and run the engine.

@note In a program and on a single machine, there can only be one single instance of
this class at a time. Attempting to create mulitple will result in an error.

The constructor takes in the nf::Config we previously created:

~~~cpp
nf::Application app(conf);
~~~

Constructing an application doesn't do much. It merely allows you to access the member
functions to setup your application.

Here are some functions you might want to call at this point:

- [setWindowIcon](@ref nf::Application::setWindowIcon) - Sets the icon of the window
- [setWindowCursor](@ref nf::Application::setWindowCursor) - Sets the cursor's image
when it is visible and inside the window

And here are the functions you **must** call before an app can run:

- [addState](@ref nf::Application::addState) - Adds a state to an app so that it can
access it later by a user-defined string identifier
- [setDefaultState](@ref nf::Application::setDefaultState) - Sets the state to load
after the logo state exits

Once these functions have been called, the app can be run:

~~~cpp
CustomGamestate* customState = new CustomGamestate; //Inherits nf::Gamestate
app.addState(customState, "State 1"); //"State One" is this state's identifier.
app.setDefaultState("State 1"); //Will error without this
app.run(); //Blocks until exit
~~~

@section createGamestate Creating a Gamestate

To create a gamestate, you must create a class that publicly inherits nf::Gamestate
and overrides its four virtual functions:

- [onEnter](@ref nf::Gamestate::onEnter) - Called when the state is loading; Where member
objects are initialized
- [update](@ref nf::Gamestate::update) - Called every frame after loading is complete;
Where custom behavior is defined
- [render](@ref nf::Gamestate::render) - Called after update; Selects what to render
- [onExit](@ref nf::Gamestate::onExit) - Called when the state is unloaded

A gamestate class might look something like this:

~~~cpp
class CustomGamestate : public nf::Gamestate {
public:
	void onEnter() override;
	void update(float deltaTime) override; //Note the parameter
	void render(nf::Renderer& renderer) override;
	void onExit() override;
	
	//Implementations somewhere else...
	
private:
	//Member objects here...
};
~~~

Once an app has been setup and run with an empty state, you should be met with a black screen
after the logo state:

@image html blankapp.png "A blank gamestate" width=50%

Congratulations! You now have a basic NF app running. Now we can add objects to our world.

@section createEntities Adding 3D Objects



@section createUI Creating a UI

@section createCubemap Adding a Cubemap (Skybox)

@todo Lighting page?

@section packaging Packaging Your App