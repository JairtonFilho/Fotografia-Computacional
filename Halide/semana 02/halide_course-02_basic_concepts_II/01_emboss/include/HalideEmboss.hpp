#ifndef HALIDE_EMBOSS_HPP
#define HALIDE_EMBOSS_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;


class HalideEmboss : public Generator<HalideEmboss> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        void generate() {
            input_bound(x, y, c) = i16(BoundaryConditions::mirror_interior(img_input)(x, y, c));
            result = u8_sat(input_bound(x, y, c)
                            - input_bound(x, y - 1, c) + input_bound(x, y + 1, c)
                            - input_bound(x - 1, y, c) + input_bound(x + 1, y, c)
                            - 2 * input_bound(x - 1, y - 1, c) + 2 * input_bound(x + 1, y + 1, c));
            img_output(x, y, c) = result;
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func input_bound{"input_bound"};
        Expr result;
};

#endif