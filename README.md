![atropos screen1.png](https://raw.githubusercontent.com/redagito/Atropos/master/Screenshot/screen1.png)

# Atropos

Atropos is an old university project for a computer graphics course which I am releasing now as open source under the [MIT license](http://opensource.org/licenses/MIT).

It includes a simple forward renderer originally written in OpenGL 3.3 core and some gameplay mechanics. It represents my first attempt to write a realtime renderer.

## Features

* Simple OpenGL forward renderer
* Shadow Mapping with Poisson filtering
* State based gameplay system
* Asynchronous resource loading

![atropos screen2.png](https://raw.githubusercontent.com/redagito/Atropos/master/Screenshot/screen2.png)

## Assets & Copyright

All assets are either produced by myself or used  under the fair use license.

If you are the copyright holder of an asset, contact me and I will remove/replace the asset.

## Versions

* `v1` is the original version of the project, preserved as it was before modernization work.

## Building

The project builds with CMake (4.0+) and Conan 2.x. On Windows it uses the Visual Studio generator.

The build requires a conan profile with c++23 set.

```
conan install . -s build_type=Debug --build=missing
conan install . -s build_type=Release --build=missing
cmake --preset conan-default
```

Build and test either configuration:

```
cmake --build --preset conan-debug
ctest --test-dir build -C Debug

cmake --build --preset conan-release
ctest --test-dir build -C Release
```