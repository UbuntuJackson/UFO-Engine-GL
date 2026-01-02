import os

build_fail = os.system(
    'cd build && cmake .. -DSDL_VIDEO=ON -DSDL_X11=ON -DSDL_TESTS=OFF -DCMAKE_CXX_FLAGS="-ggdb -O0" && make -j16'
)

if build_fail:
    print("Build failed")
else:
    runtime_fail = os.system(
        'cd build cmake .. -DCMAKE_CXX_FLAGS="-ggdb -O0" && make -j6 && gdb OUT'
    )
    # runtime_fail = os.system("cd build && make -j6 && ./OUT")

    if runtime_fail:
        print("Runtime fail")
