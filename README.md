# SimpleRunner

## Dependencies
project submodules :
- NVRHI
- GLFW
- GLM
- STB
- ImGui

project package dependencies :
- VulkanSDK (>= 1.4)

## Build
cmake build options : `GIT_SUBMODULE` `CMAKE_EXPORT_COMPILE_COMMANDS` `COMPILE_CONFIG_BUILD`

cmake presets : `debug` `release`

---

Setup project submodules if cmake doesn't work
```
git submodule update --init --recursive
```

<br>

build editor exemple
```
mkdir build

cmake --preset=debug
mingw32-make -C ./build -j8
```

<br>

Since there is no way to build the 'game' in the editor you can use `-DCOMPILE_CONFIG_BUILD=True` to build the 'game' executable *(it will recompile the engine)*

build game exemple
```shell
mkdir build

cmake --preset=debug -DCOMPILE_CONFIG_BUILD=True
mingw32-make -C ./build -j8
```


## Precompiled macros
if you are in editor build : `SR_EDITOR`

if you are in game build : `SR_BUILD`
