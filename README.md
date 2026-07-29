#UFO-Engine-GL

Compiling

Linux:

Build with bash:
```bash

    cd build && cmake .. -DUFO_ENGINE_STUDIO=ON -DSDL_VIDEO=ON -DSDL_X11_XTEST=OFF -DSDL_X11=ON -DSDL_TESTS=OFF -DCMAKE_CXX_FLAGS="-Os -O3 -Wextra -Wall -Wextra" && make -j16

```

Requirements:
python (version 3)

Cross compilation on linux, for windows:
  cmake .. -DCMAKE_CXX_FLAGS="-O0 -Os" -DUFO_ENGINE_STUDIO=ON -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -GNinja && ninja -j16

Compilation for windows, I think:
  cmake .. -DCMAKE_CXX_FLAGS="-O0 -Os" -DUFO_ENGINE_STUDIO=ON -GNinja && ninja -j16

For making games
-Make sure mingw g++, gdb, ninja-build and cmake are available from within the build folder when pressing compile in the editor.
