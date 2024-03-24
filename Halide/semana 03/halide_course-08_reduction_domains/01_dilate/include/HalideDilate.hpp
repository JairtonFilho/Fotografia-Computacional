#ifndef HALIDE_DILATE_HPP
#define HALIDE_DILATE_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;


class HalideDilate : public Generator<HalideDilate> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 2};
        GeneratorParam<int> kernel_size{"kernel_size", 3};
        Output<Buffer<uint8_t>> img_output{"img_output", 2};

        void generate() {
            input_bound = BoundaryConditions::mirror_interior(img_input);
            Expr hk = (kernel_size / 2);
            RDom kernel(-hk, kernel_size, -hk, kernel_size);

            // kernel.where(pow(kernel.x, 2) + pow(kernel.y, 2) <= pow(hk, 2));

            img_output(x, y) = maximum(input_bound(x + kernel.x, y + kernel.y));
        }

        void schedule() {
            img_input.set_estimates({{0, 512}, {0, 512}});
            img_output.set_estimates({{0, 512}, {0, 512}});
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func input_bound{"input_bound"};
        Expr result;
};

#endif