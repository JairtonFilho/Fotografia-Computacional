#ifndef HALIDE_RGB2CMY_HPP
#define HALIDE_RGB2CMY_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

enum MergeMode {
    UPDATE = 0,
    SELECT,
    MUX
};

class HalideRGB2CMY : public Generator<HalideRGB2CMY> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        GeneratorParam<enum MergeMode> merge_mode{"merge_mode", UPDATE,
            {
              {"update", UPDATE},
              {"select", SELECT},
              {"mux", MUX}
            }
        };

        void generate() {
            Expr r, g, b;
            r = img_input(x, y, 0);
            g = img_input(x, y, 1);
            b = img_input(x, y, 2);

            // Trasform image
            Expr cc, m, yy;
            cc = 255 - r;
            m = 255 - g;
            yy = 255 - b;

            // Cast to uint8 and merge channels into result image
            switch (merge_mode)
            {
                case UPDATE:
                    img_output(x, y, c) = u8(0);
                    img_output(x, y, 0) = u8_sat(cc);
                    img_output(x, y, 1) = u8_sat(m);
                    img_output(x, y, 2) = u8_sat(yy);
                    break;

                case SELECT:
                    img_output(x, y, c) = select(c == 0, u8_sat(cc), c == 1, u8_sat(m), u8_sat(yy));
                    break;

                default:
                    img_output(x, y, c) = u8_sat(mux(c, {cc, m, yy}));
                    break;
            }

        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
};

#endif