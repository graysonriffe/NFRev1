@page tutorial Tutorial
@tableofcontents

This tutorial aims to teach the basics of the engine and how to use it.

First, follow the steps on the @ref install page. Once the template MSVC project is setup,
you can begin here.

@section nfArchTut Engine Architecture

An NF app is made up of a set of states represented by the nf::Gamestate class. A gamestate
has a set of objects and programmed behavior. When an nf::Application is running,
it is either running a state or loading one. Below is an image that describes what the
main thread of an NF app might be typically doing in the program's lifetime.

@image html applifetime.png "The lifetime of a typical NF app" width=20%

Using the engine's architecture, you might not even write any functions that are called
from `main`. Most of the code that programs the engine's behavior should be called in
your state's [update function](@ref nf::Gamestate::update).

To allow a translate unit to use the engine, you must include `NothinFancy.h`. This
header contains every class and function.

@section createConfigTut Creating a Config

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

@section createAppTut Creating and Configuring an Application

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

@note If you never call setWindowIcon before running, the default window icon will be
set for you.

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

@section createGamestateTut Creating a Gamestate

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
The window can be closed with Alt + F4 or by the close button.

For more about gamestates, see @ref gamestates.

@section createEntitiesTut Adding 3D Objects

In NF, all 3D objects are represented by the nf::Entity class. All entities have a 3D
model, a position, rotation, scale, and type among others. The [type](@ref nf::Entity::Type)
of the entity dictates how the object behaves in the physics simulation.

@note At this point, it's probably a good idea to read the @ref obLifetime page. It discusses
the creation and destruction of different objects including entities.

To construct an entity, make it a member in your gamestate, or for dynamically created entites,
allocate it on the heap and add the pointer to a `std::vector` of `nf::Entity*` to keep
track of them.

~~~cpp
#include <vector>

class CustomGamestate : public nf::Gamestate {
private:
	nf::Entity entity1;
	nf::Entity entity2;
	
	std::vector<nf::Entity*> entities;
	
	//Rest of class definition...
};
~~~

In your gamestate's `onEnter` function, create the entity with
[create](@ref nf::Entity::create):

~~~cpp
void onEnter(float deltaTime) {
	//Places an immovable (default) cube at (0.0, 0.0, -5.0)
	entity1.create(nf::BaseAssets::cube, nf::Vec3(0.0, 0.0, -5.0));
	
	//Places a movable sphere at (3.0, 1.0, -4.0)
	entity2.create(nf::BaseAssets::sphere, nf::Vec3(3.0, 1.0, -4.0), nf::Entity::Type::DYNAMIC);
}
~~~

The last step is to render the objects by rendering them in your gamestate's `render`
function:

~~~cpp
void render(nf::Renderer& renderer) {
	renderer.render(entity1);
	renderer.render(entity2);
}
~~~

Once the app is run, you should see two grey objects when the state loads: a static cube
and a falling sphere.

@image html objects.png "Our scene so far" width=50%

Let's add another entity that will be our ground plane so that our sphere has a place to
land.

~~~cpp
floor.create(nf::BaseAssets::plane, nf::Vec3(0.0, -3.0, 0.0));

//This plane will be the same size of the cube by default, so let's scale it on every axis...
floor.setScale(10.0f);
~~~

Currently, none of the keys on our keyboard (other than Alt + F4) does anything, so let's
make escape close the app.

@section inputTut Keyboard Input

Every gamestate has a pointer member to the parent nf::Application called `app`. Use this
member to access the input functions.

In NF, there are two ways to check for key events:

- [isKeyHeld](@ref nf::Application::isKeyHeld) - Returns true for every frame that the
tested key is held for
- [isKeyPressed](@ref nf::Application::isKeyPressed) - Returns true for only the first
frame the key is down regardless of how long it stays down

Both functions take in a key code. The key codes provided by NF all start with `NFI_` and
an uppercase letter, a number, or word denoting a special key eg `NFI_W`, `NFI_5`,
and `NFI_SPACE`.

To quit the app when escape is pressed, add this to your `update` funciton:

~~~cpp
if (app->isKeyPressed(NFI_ESCAPE))
	app->quit();
~~~

`app->quit()` will cause the engine to shut down on the next frame and return from
`nf::Application::run`.

Everything is dark and grey, so let's add some basic lighting.

@section lightingTut Adding Lights

Naturally, the nf::Light class represents a light. It is constructed, created, and rendered
in the same way that an entity is.

~~~cpp
light.create(nf::Vec3(0.0, 5.0, 0.0), nf::Vec3(1.0)); //Creates a completely white light
~~~

Just as with entities, you must also render the light in your `render` function.

@image html basiclighting.png "Our scene with a light" width=50%

Let's go on to controlling the view.

@section controlCameraTut Controlling the Camera

Every gamestate has a pointer member to a nf::Camera called `camera`. Use this to control
the current camera.

Just like entites, the camera has a [type](@ref nf::Camera::Type) too. The type dictates
how the mouse interacts with moving the camea. By default, every gamestate starts with
the camera in `UI` mode which means that the mouse is free to move across the window without
affecting the camera in any way.

To change to the first person mode, write this in your `onEnter` function somewhere:

~~~cpp
camera->setType(nf::Camera::Type::FIRST_PERSON);
~~~

@note The current mouse sensitivity will be able to be changed in a future update.

Now onto actually moving the camera with the classic WASD keys.

Using our previous knowledge of keyboard input, we can write four consecutive `if`
statements for each of the movement keys:

~~~cpp
if (app->isKeyHeld(NFI_W))
	//Move forward
if (app->isKeyHeld(NFI_A))
	//Move left
if (app->isKeyHeld(NFI_S))
	//Move backward
if (app->isKeyHeld(NFI_D))
	//Move right
~~~

The functions for moving the camera is as follows:

- [move](@ref nf::Camera::move) - Moves the camera based off of an nf::Vec2 in (x, z)
- [moveZ](@ref nf::Camera::moveZ) - Moves the camera forward and backward with an offset
- [moveX](@ref nf::Camera::moveX) - Moves the camera left and right with an offset

Since the `offset` here will be called every frame, it's effectively a velocity, and when
we talk about velocities, it's important to discuss `update`'s only parameter, `deltaTime`.

Delta time in this context is the amount of time that the previous fame took to produce.
This time includes how long it takes to run your `update` and `render` functions as well
as everything else in the engine that gets a frame on screen. Why is this important?
Because it can cancel out framerate differences between different machines.

Let's say that Computer A is a modern-day gaming rig with 8 cores and an RTX 2060.
Computer A can run our game at a solid 60 FPS. Let's also say that Computer B is an
older laptop that struggles to run the engine smoothly. It runs our game at around 30 FPS.
If in our `update` function, we move the camera (or anything at all) by a set amount,
Computer A will move our view twice the distance than Computer B in any amount of real time
since there were twice the amount of frames drawn in that time.

The solution is to multiply any velocity you use with this delta time. You will have to change
your values around a little, but this will make speeds consistant across computers.

With that, we can now complete our movement logic:

~~~cpp
if (app->isKeyHeld(NFI_W))
	camera->moveZ(10.0f * deltaTime);
if (app->isKeyHeld(NFI_A))
	camera->moveX(-10.0f * deltaTime);
if (app->isKeyHeld(NFI_S))
	camera->moveZ(-10.0f * deltaTime);
if (app->isKeyHeld(NFI_D))
	camera->moveX(10.0f * deltaTime);
~~~

We are now able to move around the world.

@image html cameramovement.png "Our scene from a different angle" width=50%

Now let's take care of that black background.

@section createCubemapTut Adding a Cubemap (Skybox)

A world texture is represented by the nf::Cubemap class. Rendering this object will
display a texture in the world. We use the class the same way we use the others:

~~~cpp
cubemap.create(nf::BaseAssets::cubemap); //No position or type though
~~~

After rendering, our world will have a background.

@image html cubemap.png "Our scene with a background" width=50%

@note Cubemaps do not emit light onto your scene.

@section customAssetsTut Adding Your Assets

NF's asset system builds your assets into NFPacks that the engine reads at runtime. The
external tool `NFPackCreator.exe` creates these for you. For a complete guide, please
see @ref assets.

@section createUITut Creating a UI

@todo Lighting page?

@section debuggingTut Debugging Your App

NF has a number of @ref macros that you can use in your debug builds to help you develop
your application. These macros can log messages, throw errors, time functions, and pause
the engine.

~~~cpp
nf::Vec3 pos = camera->getPosition();
std::string posStr = std::to_string(pos.x) + (std::string)", " + std::to_string(pos.y) + (std::string)", " + std::to_string(pos.z);
NFLog("Current position: " + posStr);
//This will print the current position of the camera every frame.
~~~

@section packagingTut Packaging and Distributing Your App

@note Remember to only ever distribute your release build.

The only external prerequisite when running a NF app is the 2022 64-bit MSVC
redistributable. The installer is included in the `redist` folder in the engine download.

Other than that, a build can be very simple:

- **NFApp.exe** - The application binary which is named from the MSVC project
- **assets** - The folder which holds your NFPacks
  - **base.nfpack** - The NFPack that holds both critical and default assets

These are the only files you need to package in your build.