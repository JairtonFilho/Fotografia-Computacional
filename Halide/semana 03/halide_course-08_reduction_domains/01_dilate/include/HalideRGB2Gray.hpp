#ifndef HALIDE_RGB2GRAY_HPP
#define HALIDE_RGB2GRAY_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideRGB2Gray : public Generator<HalideRGB2Gray> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 2};

        void generate() {
            RDom r(0, 3);
            img_output(x, y) = u8_sat(
                0.299f * i32(img_input(x, y, 0)) + 0.587f * i32(img_input(x, y, 1)) + 0.114f * i32(img_input(x, y, 2))
            );
        }

        void schedule() {
            img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
            img_output.set_estimates({{0, 512}, {0, 512}});
        }

    private:
        Var x{"x"}, y{"y"};
};

#endif