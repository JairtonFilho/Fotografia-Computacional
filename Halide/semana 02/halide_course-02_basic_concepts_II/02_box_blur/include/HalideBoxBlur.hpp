#ifndef HALIDE_BOXBLUR_HPP
#define HALIDE_BOXBLUR_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;


class HalideBoxBlur : public Generator<HalideBoxBlur> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        void generate() {
            input_bound(x, y, c) = f32(BoundaryConditions::mirror_interior(img_input)(x, y, c));
            total = (input_bound(x, y, c) + input_bound(x, y - 1, c) + input_bound(x, y + 1, c)
                      + input_bound(x - 1, y, c) + input_bound(x + 1, y, c) + input_bound(x - 1, y - 1, c) 
                      + input_bound(x - 1, y + 1, c) + input_bound(x + 1, y - 1, c) + input_bound(x + 1, y + 1, c));
            result = u8_sat(total / 9.0f);
            img_output(x, y, c) = result;
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func input_bound{"input_bound"};
        Expr total, result;
};

#endif