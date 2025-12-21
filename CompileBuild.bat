mkdir build

cmake --preset=debug -DCOMPILE_CONFIG_BUILD=True
mingw32-make -C ./build -j12
