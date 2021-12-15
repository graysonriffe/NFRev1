@page install Installation
@tableofcontents

This page details how to get started with the engine by using the template project.

@section prereq Prerequisites

In order to develop an NF application, you must first have the following:

- Visual Studio 2022 or later (MSVC v143 or later)
- A 64 bit computer as **NF is 64 bit only**
- A compatible graphics card (See @ref issues)
- Apt C++ knowledge
- The engine itself

The current version of the engine can be downloaded [here](http://graysonriffe.com/nf/nf.zip).

@note Windows Security may first need to scan some of the included binaries and your
build output before they can be run. You should never get a message from Windows Security
that a virus has been found in any of these files.

Inside the zip file, you will find:

- **example** - The example app complete with the source and a build
- **manual** - An offline version of this manual
  - **index.html** - The homepage
- **redist** - MSVC Redistributable (See @ref packagingTut)
- **template** - A template MSVC project already setup to work with the engine

---

@section projSetup Template Setup

In the `template` folder, you will find:

- **assets** - Contains assets (See @ref assets)
- **bin** - Output directory for both configurations
- **dep** - Contains the engine
- **src** - The source folder with a `main.cpp` file
- **template.sln** - The Visual Studio solution

`template.sln` can be opened in Visual Studio 2022. This, as well as the name of the
project (`NFApp` by default), can be renamed at any time.

@note The template project is setup to build *Debug* and *Release* configurations. The *Debug*
configuration includes a console window with debug output. The *Release* configuration does
not have this console window. By default, the current configuration can be changed in the
top toolbar.

Once the solution has been opened, you can find the `main.cpp` file inside the `src` folder
as shown below.

@image html blanktemplate.png "The template opened in Visual Studio" width=70%

To build the project, go to Build -> Build Solution or Build Project. The latter will not build
your assets. You can also also hit the default keyboard shortcut of `Ctrl-Shift-B` to build
both your app and assets.

@note When the "Build Assets" project is built, the resulting NFPacks are moved to
the output `assets` directory next to your .exe for you.

NF is a **statically-linked** library. This means that your build will not
rely on any external dlls other than the MSVC redistributable.