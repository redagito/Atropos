# Atropos

Atropos is an old university project for a computer graphics course which I am releasing now as open source under the [MIT license](http://opensource.org/licenses/MIT).

It includes a simple forward renderer written in OpenGL 3.3 core and some gameplay mechanics. It represents my first attempt to write a realtime renderer and I will NOT update this project as it only serves as an historic reference.

## Screenshots

![atropos screen1.png](https://github.com/redagito/Atropos/blob/master/Screenshot/screen1.png)
![atropos screen2.png](https://github.com/redagito/Atropos/blob/master/Screenshot/screen2.png)

## Features

* Simple OpenGL forward renderer
* Shadow Mapping with Poisson filtering
* State based gameplay system
* Uses [Irrklang](http://www.ambiera.com/irrklang/) for sound replay

## Gameplay

The game starts with an empty scene (white screen as no scene objects are defined) and with start sound playing.

To start the game, press ENTER which will perform a state transition into the game state.

On death (getting hit too much) or win (survive a certain distance) the game transitions into either WIN or LOSE state.

From there, by pressing enter, the game is restarted.

## Assets & Copyright

All assets are either produced by myself or used  under the fair use license.

If you are the copyright holder of an asset, contact me and I will remove/replace the asset.

## Building

Open the solution file with Visual Studio 2019 (Community Edition) and build, it should work out of the box.

## TODO List

- Change build system to CMake / Conan
- Replace irrklang with OpenAL (or other cross-platform sound library)
- Replace external dependencies with Conan packages
