#ifndef HALIDE_RGB2GRAY_HPP
#define HALIDE_RGB2GRAY_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideRGB2Gray : public Generator<HalideRGB2Gray> {
    public:
        Input<Func> img_input{"img_input_rgb2gray", UInt(8), 3};
        Output<Func> img_output{"img_output_rgb2gray", Float(32), 2};

        void generate() {
            Expr r = f32(img_input(x, y, 0)) / 255.0f;
            Expr g = f32(img_input(x, y, 1)) / 255.0f;
            Expr b = f32(img_input(x, y, 2)) / 255.0f;
            img_output(x, y) = 0.299f * r + 0.587f * g + 0.114f * b;
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
                img_output.set_estimates({{0, 512}, {0, 512}});
                return;
            }

            img_output
                .compute_root()
                .split(y, yo, yi, 32)
                .parallel(yo)
                .split(x, xo, xi, 8)
                .vectorize(xi)
            ;
        }

    private:
        Var x{"x"}, y{"y"}, yo{"yo"}, yi{"yi"}, xo{"xo"}, xi{"xi"};
};

#endif