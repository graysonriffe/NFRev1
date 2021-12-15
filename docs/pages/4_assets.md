@page assets Asset System
@tableofcontents

This page details NF's asset system and how to work with it.

The following table shows the currently supported file types that the asset builder
will accept.

Asset Type | Supported File Type(s)
-|-
Model | Wavefront `.obj` / `.mtl`
Font | TrueType `.ttf`
Texture (Models, UI, Buttons, Cubemaps) | `.png` or `.jpg`
Audio | `.wav` or Ogg Vorbis `.ogg`

@section buildAssets How to Build Your Assets

This is the basic workflow when working with assets:

1. Create a folder in your project's `asset` folder. The name of the folder will be the
name of the output pack in the form `foldername.nfpack`.
2. Place your assets anywhere inside this folder, including in any subfolders.
3. Build the packs by either building the "Build Assets" project in the template or by
manually running `NFAssetBuilder.exe` by clicking on it. If you choose this method, you
must also manually copy the needed packs to your app's `assets` directory.
4. In your code, create an nf::AssetPack object and call the [load](@ref nf::AssetPack::load)
function while passing in the name of the pack, including the extension.
5. Call the [get](@ref nf::AssetPack::get) function with the name of the asset file
to retrieve an Asset pointer to use in various `create` functions.

@section customModels Models

To import a custom model into NF, it must first meet some requirements:

- Format must be a Wavefront `.obj` file
- The model must include both texture coordinate and normal data
- All faces must be triangles
- There must exist a `.mtl` file with the same name as the `.obj` file somewhere in the
pack folder that contains material information

Using Blender can fill all of these requirements easily. This tutorial uses Blender v3.0.0.

Let's say I have this teapot I want to place in a scene:

@image html modeltut1.png "My starting model" width=50%

It has enough detail already, but I want it to look smooth, so I'll make sure smooth
shading is turned on for this object. This makes every part of the model smooth, but I
don't want that on the handle of the lid. I want to see an edge there. The solution here
is to turn on Object Data -> Normals -> "Auto Smooth" in the properties editor
and adjust the degree threshold until I get what I want.

This should be the result:

@image html modeltut2.png "Our smooth model with a few sharp edges" width=50%

Now that that's done, I want to create the material this teapot will have. I want to
apply a texture, so I'm going to unwrap the model and texture it. Blender materials
translate directly into NF materials, so feel free to add multiple materials on the same
model that use different textures.

To add a color texture to a material, direct an "Image Texture" to the "Base Color"
input of the default Principled BSDF. This can either be done through the properties editor
or node editor.

@todo Talk about specular and normal maps

@image html modeltut3.png "Our textured model" width=50%

I am now ready to export this model, so first make sure your model is selected, then
go to File -> Export -> Wavefront (.obj). This will bring up another window where we will
set some important options.

Under the "Geometry" heading, make sure these are checked. The model will not import into
the engine unless all of these are checked.

@image html exportsettings.png "Important Export Settings" width=10%

Now that we have a pair of `.obj` and `.mtl` files, we can move them into our pack folder.
Back in the code, once we have a loaded nf::AssetPack, we can create an entity with our
new model like so:

~~~cpp
//ap is the nf::AssetPack that has our NFPack loaded
entity2.create(ap.get("teapot.obj"), nf::Vec3(0.0)); //Reference asset by name, including extension
~~~

Lastly, don't forget to build your assets before running.

Used in the tutorial app, our model looks like this:

@image html custommodel.png "Our teapot in the engine" width=50%

@section customFonts Fonts

@section customButtons Button Textures

@section customCubemaps Cubemaps

@section customSounds Sounds