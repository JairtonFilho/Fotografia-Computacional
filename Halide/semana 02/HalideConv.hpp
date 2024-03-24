#ifndef HALIDE_CONV_HPP
#define HALIDE_CONV_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;


class HalideConv : public Generator<HalideConv> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        void generate() {
            input_bound(x, y, c) = f32(BoundaryConditions::mirror_interior(img_input)(x, y, c));
            int min_val = -1;
            int extent = 3;

            weight(i, j) = f32(0);
            weight(0, 0) = 9.0f; // sharpen kernel as example
            // you can fill up weight using a two for loops after initial definition
            Expr weighted_sum, total;
            weighted_sum = f32(0);
            total = f32(0);
            for (int i_ = min_val; i_ < min_val + extent; i_++) {
                for (int j_ = min_val; j_ < min_val + extent; j_++) {
                    weighted_sum += input_bound(x + i_, y + j_, c) * weight(i_, j_);
                    total += weight(i_, j_);
                }
            }
           
            Expr result;
            result = u8_sat(weighted_sum / total);
            img_output(x, y, c) = result;
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"}, i{"i"}, j{"j"};
        Func input_bound{"input_bound"};
        Func weight{"weight"};
};

#endif