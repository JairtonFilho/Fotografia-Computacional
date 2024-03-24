#ifndef HALIDE_SHARPEN_HPP
#define HALIDE_SHARPEN_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

enum Mode {
    SIMPLE = 0,
    FULL
};

class HalideSharpen : public Generator<HalideSharpen> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        GeneratorParam<enum Mode> mode{"mode", SIMPLE,
            {
                {"simple", SIMPLE},
                {"full", FULL}
            }
        };

        void generate() {
            input_bound(x, y, c) = i16(BoundaryConditions::mirror_interior(img_input)(x, y, c));

            switch (mode)
            {
                case SIMPLE:
                    result = u8_sat(5 * input_bound(x, y, c)
                                        - input_bound(x, y - 1, c) - input_bound(x, y + 1, c)
                                        - input_bound(x - 1, y, c) - input_bound(x + 1, y, c));
                    break;

                case FULL:
                    result = u8_sat(9 * input_bound(x, y, c)
                                        - input_bound(x, y - 1, c) - input_bound(x, y + 1, c)
                                        - input_bound(x - 1, y, c) - input_bound(x + 1, y, c)
                                        - input_bound(x - 1, y - 1, c) - input_bound(x - 1, y + 1, c)
                                        - input_bound(x + 1, y - 1, c) - input_bound(x + 1, y + 1, c));
                    break;

                default:
                    result = 0;
                    break;
            }

            img_output(x, y, c) = result;
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func input_bound{"input_bound"};
        Expr result;
};

#endif