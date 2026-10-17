# Atropos

Atropos is an old university project for a computer graphics course which I am releasing now as open source under the [MIT license](http://opensource.org/licenses/MIT).

It includes a simple forward renderer written in OpenGL 3.3 core and some gameplay mechanics. It represents my first attempt to write a realtime renderer.

## Screenshots

![atropos screen1.png](https://github.com/redagito/Atropos/blob/master/Screenshot/screen1.png)
![atropos screen2.png](https://github.com/redagito/Atropos/blob/master/Screenshot/screen2.png)

## Features

* Simple OpenGL forward renderer
* Shadow Mapping with Poisson filtering
* State based gameplay system

## Gameplay

The game starts with an empty scene (white screen as no scene objects are defined) and with start sound playing.

To start the game, press ENTER which will perform a state transition into the game state.

On death (getting hit too much) or win (survive a certain distance) the game transitions into either WIN or LOSE state.

From there, by pressing enter, the game is restarted.

## Assets & Copyright

All assets are either produced by myself or used  under the fair use license.

If you are the copyright holder of an asset, contact me and I will remove/replace the asset.

## Versions

* `v1` is the original version of the project, preserved as it was before the CMake/Conan build system and the OpenAL sound backend.

## Building

The project builds with CMake (3.20+) and Conan 2.x. On Windows it uses the Visual Studio generator:

```
conan install . -s build_type=Debug -s compiler.cppstd=23 --output-folder=build --build=missing
cmake --preset conan-default
cmake --build --preset conan-debug
```

Run the demo with `build/ConsoleLauncher/Debug/consolelauncher.exe`. Run the unit tests with `ctest --test-dir build -C Debug`.