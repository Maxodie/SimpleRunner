mkdir build

cmake --preset=debug
mingw32-make -C ./build -j12
