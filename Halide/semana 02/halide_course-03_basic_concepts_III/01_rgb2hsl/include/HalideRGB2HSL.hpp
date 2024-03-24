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

class HalideRGB2HSL : public Generator<HalideRGB2HSL> {
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
            input_f32(x, y, c) = f32(img_input(x, y, c)) / 255.0f;

            Expr r, g, b;
            r = input_f32(x, y, 0);
            g = input_f32(x, y, 1);
            b = input_f32(x, y, 2);

            Expr Vmax = max(r, max(g, b));
            Expr Vmin = min(r, min(g, b));

            Expr diff = Vmax - Vmin;
            Expr sum_ = Vmax + Vmin;

            // Trasform image
            Expr hh_, hh, ss, ll;
            ll = 255.0f * (sum_ / 2.0f);

            ss = 255.0f * select(ll < 127.5f, diff / sum_, diff / (2.0f - sum_));

            hh_ = select(
                diff == 0, 0,
                Vmax  == r, ((g - b) / diff) * 60.0f,
                Vmax  == g, ((b - r) / diff) * 60.0f + 120.0f,
                ((r - g) / diff) * 60.0f + 240.0f
            );

            hh = 0.5f * select(hh_ > 0, hh_, hh_ + 360.0f);

            // Cast to uint8 and merge channels into result image
            switch (merge_mode)
            {
                case UPDATE:
                    img_output(x, y, c) = u8(0);
                    img_output(x, y, 0) = u8_sat(hh);
                    img_output(x, y, 1) = u8_sat(ss);
                    img_output(x, y, 2) = u8_sat(ll);
                    break;

                case SELECT:
                    img_output(x, y, c) = select(c == 0, u8_sat(hh), c == 1, u8_sat(ss), u8_sat(ll));
                    break;

                default:
                    img_output(x, y, c) = u8_sat(mux(c, {hh, ss, ll}));
                    break;
            }

        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func input_f32{"input_f32"};
};

#endif