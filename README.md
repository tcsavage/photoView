# Photo View

View Raw photographs and apply filters.



## Building

First pull Git submodules:

```
git submodule update --init --recursive
```

### If you're running NixOS:

```
nix-shell
configRelease
build
```

### Other Linux distributions:

Photo View required the following additional dependencies:

- CMake
- A recent GCC supporting C++20 (tested on 10.2.0)
- Qt >= 5.15
- LibRaw >= 0.20.2
- OpenImageIO >= 2.0
- OpenCL headers and ICD

Configure and build the project using CMake like so:

```
cmake -Bbuild -S. -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
