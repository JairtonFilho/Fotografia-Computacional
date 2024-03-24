#ifndef HALIDE_SEPIA_HPP
#define HALIDE_SEPIA_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideSepia : public Generator<HalideSepia> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};

        GeneratorParam<int> custom_schedule{"custom_schedule", -1};

        Output<Buffer<uint8_t>> img_output{"img_output", 3};

        void generate() {
            img_input_f32(x, y, c) = f32(img_input(x, y, c)); // Convert to float32

            Expr r, g, b;
            r = img_input_f32(x, y, 0);
            g = img_input_f32(x, y, 1);
            b = img_input_f32(x, y, 2);

            // Transform image
            Expr nr, ng, nb;
            nr = (r * 0.393f) + (g * 0.796f) + (b * 0.189f);
            ng = (r * 0.349f) + (g * 0.686f) + (b * 0.164f);
            nb = (r * 0.272f) + (g * 0.534f) + (b * 0.131f);

            // Cast to uint8 and merge channels into result image
            img_output(x, y, c) = u8_sat(mux(c, {nr, ng, nb}));
        }

        void schedule() {
            int vector_size = get_target().natural_vector_size(UInt(8));
            switch (custom_schedule) {
                case 0:
                    img_output.parallel(y);
                    break;

                case 1:
                    img_output
                        .reorder(c, x, y)
                        .bound(c, 0, 3)
                        .unroll(c)
                    ;
                    break;
                
                case 2:
                    img_output
                        .reorder(c, x, y)
                        .bound(c, 0, 3).unroll(c)
                        .parallel(y)
                    ;
                    break;

                case 3:
                    img_output
                        .reorder(c, x, y)
                        .bound(c, 0, 3).unroll(c)
                        .split(y, yo, yi, 8 * vector_size).parallel(yo)
                    ;
                    break;

                case 4:
                    img_output
                        .reorder(c, x, y)
                        .bound(c, 0, 3).unroll(c)
                        .split(y, yo, yi, 8 * vector_size).parallel(yo)
                        .split(x, xo, xi, 2 * vector_size).vectorize(xi)
                    ;
                    break;

                case 5:
                    img_output
                        .split(y, yo, yi, 8 * vector_size).parallel(yo)
                        .split(x, xo, xi, 2 * vector_size).vectorize(xi)
                    ;
                    break;
                
                default:
                    break;
            }
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"}, yi{"yi"}, yo{"yo"}, xi{"xi"}, xo{"xo"};
        Func img_input_f32{"img_input_f32"};
};

#endif