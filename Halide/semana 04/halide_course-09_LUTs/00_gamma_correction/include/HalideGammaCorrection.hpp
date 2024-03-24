#ifndef HALIDE_GAMMA_HPP
#define HALIDE_GAMMA_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

enum Mode {
    PER_PIXEL = 0,
    LUT
};

class HalideGammaCorrection : public Generator<HalideGammaCorrection> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};

        GeneratorParam<enum Mode> mode{"mode", PER_PIXEL,
            {
                {"per_pixel", PER_PIXEL},
                {"lut", LUT}
            }
        };
        GeneratorParam<float> gamma{"gamma", 1};

        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        void generate() {
            if (mode == PER_PIXEL) {
                img_input_f32(x, y, c) = f32(img_input(x, y, c)) / 255.0f;
                img_output(x, y, c) = u8_sat(255.0f * pow(img_input_f32(x, y, c), gamma));
            } else {
                lut(i) = u8_sat(255.0f * pow(f32(i) / 255.0f, gamma));
                img_output(x, y, c) = lut(img_input(x, y, c));
            }
        }

        void schedule() {
            Var yo{"yo"}, xo{"xo"}, yi{"yi"}, xi{"xi"}, tiles{"tiles"};
            int vector_size = get_target().natural_vector_size(UInt(8));
            if (mode == PER_PIXEL) {
                img_output
                    .compute_root()
                    // .reorder(c, x, y)
                    // .bound(c, 0, 3)
                    // .unroll(c)
                    // .tile(x, y, xo, yo, xi, yi, vector_size, 4 * vector_size)
                    // .fuse(yo, xo, tiles)
                    // .parallel(tiles)
                    // .vectorize(xi)
                ;
            } else {
                lut.compute_root();
                img_output
                    .compute_root()
                //     .reorder(c, x, y)
                //     .bound(c, 0, 3)
                //     .unroll(c)
                //     .split(y, yo, yi, vector_size * 4)
                //     .parallel(yo)
                ;
            }
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"}, i{"i"};
        Func img_input_f32{"img_input_f32"};
        Func lut{"lut"};
        
};

#endif