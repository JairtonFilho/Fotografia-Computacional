// g++ main.cpp -std=c++20 -fno-rtti -I$HALIDE_ROOT/include -I$HALIDE_ROOT/tools -Iinclude -Ibin -L$HALIDE_ROOT/lib  \
    -lHalide -lpthread -ldl -lz -ltinfo -ljpeg `libpng-config --cflags --ldflags` -o main

#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide;
using namespace Halide::Tools;
using namespace Halide::ConciseCasts;

#define FACTOR 16

int main() {
    Var x{"x"}, y{"y"}, c{"c"};
    Func pattern;
    pattern(x, y, c) = (
        127.5f * sin(x / FACTOR) * sin(y / FACTOR) + 127.5f
    );

    Buffer<float, 3> output = pattern.realize({320, 320, 3});
    save_image(output, "output.jpeg");
    return 0;
}