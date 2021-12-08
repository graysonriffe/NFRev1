@page gamestates Gamestate System
@tableofcontents

This page details NF's state system and how to use it.

@section gamestatesOverview Overview

Before your nf::Application is run, you must first construct every one of the gamestates
you want to use in that app **on the heap**. This is because having gamestates with many
objects could result in a stack overflow.

~~~cpp
YourState* state = new YourState;
~~~

When the nf::Application's destructor is called, `delete` is called on every added gamestate
for your convenience.

@section enterAndExit onEnter and onExit

These two functions are meant to load / initialize and "reset" your state respectively.

The `onEnter` function is called as the loading screen is shown. When it returns, the screen
fades into your scene. If you do not specify a starting position or rotation for the `camera`,
it will start at (0.0, 0.0, 0.0) facing -Z (forwards).

The `onExit` function is different. How you use this function is up to you based off of
what your states are and if you want to connect them in some way, but if you want a
state to behave in the same way every time you start it, you must
**reset scalar members of your state that `onEnter` doesn't set itself** and
**also `delete` any dynamically created objects created while the state was runnng**.

@section obLifetime Object Lifetime