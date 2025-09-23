import os

build_fail = os.system("cd build && cmake .. -DSDL_TESTS=ON -DCMAKE_CXX_FLAGS=\"-ggdb\" && make -j16")

if build_fail:
    print("Build failed")
else:
    runtime_fail = os.system("cd build && ./OUT")

    if runtime_fail:
        print("Runtime fail")