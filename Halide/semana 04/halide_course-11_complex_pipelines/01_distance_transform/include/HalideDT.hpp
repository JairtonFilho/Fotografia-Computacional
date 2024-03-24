#ifndef HALIDE_DT_HPP
#define HALIDE_DT_HPP

#include "Halide.h"
#include <iostream>

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideDT : public Generator<HalideDT> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        
        GeneratorParam<int> iterations{"iterations", 50};

        Output<Buffer<int32_t>> img_output{"img_output", 2};

        void generate() {
            img_bound = BoundaryConditions::constant_exterior(img_input, 0); 
            Expr r = f32(img_bound(x, y, 0)) / 255.0f;
            Expr g = f32(img_bound(x, y, 1)) / 255.0f;
            Expr b = f32(img_bound(x, y, 2)) / 255.0f;
            img_gray(x, y) = 0.299f * r + 0.587f * g + 0.114f * b;
            
            thresholded(x, y) = select(img_gray(x, y) > 0.25f, 1.0f, 0.0f);

            RDom se(-1, 3, -1, 3);
            eroded(x, y) = thresholded(x, y);
            aux(x, y) = thresholded(x, y);
            for (int i = 0; i < iterations; i++) {
                Func erosion{"erosion_" + std::to_string(i)};                
                erosion(x, y) = minimum(eroded(x + se.x, y + se.y));
                eroded = erosion;
                aux(x, y) += eroded(x, y);
            }

            img_output(x, y) = i32(aux(x, y));
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
                img_output.set_estimates({{0, 512}, {0, 512}});
            }
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func img_bound{"img_bound"}, img_gray{"img_gray"}, thresholded{"thresholded"};
        Func aux{"aux"}, eroded{"eroded"};
};

#endif