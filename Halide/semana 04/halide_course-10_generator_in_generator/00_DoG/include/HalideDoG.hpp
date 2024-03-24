#ifndef HALIDE_DOG_HPP
#define HALIDE_DOG_HPP

#include "Halide.h"
#include "HalideRGB2Gray.hpp"
#include "gaussian.hpp"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideDoG : public Generator<HalideDoG> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input_dog", 3};

        GeneratorParam<float> sigma{"sigma", 1.0f};
        GeneratorParam<float> k{"k", 1.6f};
        GeneratorParam<float> roughness{"roughness", 1.0f};

        Output<Buffer<uint8_t>> img_output{"img_output_dog", 2};

        void generate() {
            input_bound = BoundaryConditions::mirror_interior(img_input);

            rgb2gray = create<HalideRGB2Gray>();
            rgb2gray->apply(input_bound);

            Expr lk_width = get_kernel_width(sigma.value());
            Expr hk_width = get_kernel_width(k.value() * sigma.value());

            Expr low_sigma = sigma;
            Expr high_sigma = k * low_sigma;
            weights_low(i) = gaussian(i, low_sigma);
            weights_high(i) = gaussian(i, high_sigma);

            RDom rd_low(-(lk_width / 2), lk_width, -(lk_width / 2), lk_width);
            RDom rd_high(-(hk_width / 2), hk_width, -(hk_width / 2), hk_width);

            Expr pixel_low = rgb2gray->img_output(x + rd_low.x, y + rd_low.y);
            Expr sum_low = sum(weights_low(rd_low.x) * weights_low(rd_low.y), "sum_low");
            filtered_low(x, y) = sum(
                pixel_low * weights_low(rd_low.x) * weights_low(rd_low.y), "sum_conv_low"
            ) / sum_low;
            

            Expr pixel_high = rgb2gray->img_output(x + rd_high.x, y + rd_high.y);
            Expr sum_high = sum(weights_high(rd_high.x) * weights_high(rd_high.y), "sum_high");
            filtered_high(x, y) = sum(
                pixel_high * weights_high(rd_high.x) * weights_high(rd_high.y), "sum_conv_high"
            ) / sum_high;

            img_output(x, y) = u8_sat(
                select(
                    (filtered_low(x, y) - roughness * filtered_high(x, y)) <= 0.0f, 
                    255.0f, 
                    0.0f
                )
            );
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
                img_output.set_estimates({{0, 512}, {0, 512}});
            }
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"}, i{"i"};
        Func input_bound{"input_bound"};
        Func weights_low{"weights_low"}, weights_high{"weights_high"};
        Func filtered_low{"filtered_low"}, filtered_high{"filtered_high"};
        std::unique_ptr<HalideRGB2Gray> rgb2gray;
};

#endif