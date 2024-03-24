#ifndef HALIDE_GRADIENT_HPP
#define HALIDE_GRADIENT_HPP

#include "Halide.h"
#include <iostream>

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideGradient : public Generator<HalideGradient> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 2};   

        void generate() {
            img_bound = BoundaryConditions::constant_exterior(img_input, 0); 
            img_gray(x, y) = (1 / 3.0f) * (
                i32(img_bound(x, y, 0)) + i32(img_bound(x, y, 1)) + i32(img_bound(x, y, 2))
            );
            vert_gradient_intm(x, y) = img_gray(x, y - 1) - img_gray(x, y + 1);
            vert_gradient(x, y) = vert_gradient_intm(x - 1, y) + 2 * vert_gradient_intm(x, y) + vert_gradient_intm(x + 1, y);
            horiz_gradient_intm(x, y) = img_gray(x, y - 1) + 2 * img_gray(x, y) + img_gray(x, y + 1);
            horiz_gradient(x, y) = horiz_gradient_intm(x - 1, y) - horiz_gradient_intm(x + 1, y);

            Expr grad_v = vert_gradient(x, y);
            Expr grad_h = horiz_gradient(x, y);
            img_output(x, y) = u8_sat(
                127.5f * (Halide::atan2(grad_v, grad_h) / (float(M_PI) * 0.5f)) + 127.5f
            );
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
                img_output.set_estimates({{0, 512}, {0, 512}});
            }
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func img_bound{"img_bound"}, img_gray{"img_gray"};
        Func vert_gradient_intm{"vert_gradient_intm"}, vert_gradient{"vert_gradient"};
        Func horiz_gradient_intm{"horiz_gradient_intm"}, horiz_gradient{"horiz_gradient"};
};

#endif