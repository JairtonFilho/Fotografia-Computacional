#ifndef HALIDE_GRADIENT_HPP
#define HALIDE_GRADIENT_HPP

#include "Halide.h"
#include <iostream>

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideGradient : public Generator<HalideGradient> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 3};
        Input<uint> threshold{"threshold"};

        GeneratorParam<bool> output_orientation{"output_orientation", false};

        Output<Buffer<uint8_t>> img_output{"img_output", 2};   

        void configure() {
            if (output_orientation) {
                img_output_orientation = add_output<Buffer<uint8_t>>("img_output_orientation", 2);
            }
        }     

        void generate() {
            // Padding com 0's
            img_bound = BoundaryConditions::constant_exterior(img_input, 0); 

            // Conversão para grayscale 
            img_gray(x, y) = 0.299f * i32(img_bound(x, y, 0)) + 0.587f * i32(img_bound(x, y, 1)) + 0.114f * i32(img_bound(x, y, 2));

            // Sobel vertical
            // Multiplicar o threshold por 4 ao invés de normalizar
            //  1  2  1          1
            //  0  0  0 * 1/4 =  0 * 1 2 1 * 1/4
            // -1 -2 -1         -1
            vert_gradient_intm(x, y) = img_gray(x, y - 1) - img_gray(x, y + 1);
            vert_gradient(x, y) = vert_gradient_intm(x - 1, y) + 2 * vert_gradient_intm(x, y) + vert_gradient_intm(x + 1, y);

            // Sobel horizontal
            // Multiplicar o threshold por 4 ao invés de normalizar
            // 1 0 -1          1
            // 2 0 -2 * 1/4 =  2 * 1 0 -1 * 1/4
            // 1 0 -1          1
            horiz_gradient_intm(x, y) = img_gray(x, y - 1) + 2 * img_gray(x, y) + img_gray(x, y + 1);
            horiz_gradient(x, y) = horiz_gradient_intm(x - 1, y) - horiz_gradient_intm(x + 1, y);

            // Ao invés de tirar a raiz quadrada da magnitude, elevar o threshold ao quadrado
            Expr grad_v = vert_gradient(x, y);
            Expr grad_h = horiz_gradient(x, y);

            // Magnitude -> sqrt(gv**2 + gh**2)
            // Elevar thresold ao quadrado ao invés de sqrt
            magnitude(x, y) = (grad_v * grad_v) + (grad_h * grad_h);

            thresh() = pow(i32(threshold), 2) * 4 * 4;

            // Limiarização + conversão para uint8
            img_output(x, y) = 255 * u8(magnitude(x, y) < thresh());

            if (output_orientation) {
                (*img_output_orientation)(x, y) = u8_sat(
                    127.5f * (Halide::atan2(grad_v, grad_h) / (float(M_PI) * 0.5f)) + 127.5f
                );
                
            }
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 512}, {0, 512}, {0, 3}});
                img_output.set_estimates({{0, 512}, {0, 512}});
                if (output_orientation)
                    (*img_output_orientation).set_estimates({{0, 512}, {0, 512}});
            }
            img_output.print_loop_nest();
            if (output_orientation)
                img_output_orientation->print_loop_nest();
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Var xi{"xi"}, xo{"xo"}, yi{"yi"}, yo{"yo"}, tile_index{"tile_index"};
        Func img_bound{"img_bound"}, img_gray{"img_gray"};
        Func vert_gradient_intm{"vert_gradient_intm"}, vert_gradient{"vert_gradient"};
        Func horiz_gradient_intm{"horiz_gradient_intm"}, horiz_gradient{"horiz_gradient"};
        Func magnitude{"magnitude"};
        Func thresh{"thresh"};

        Output<Buffer<uint8_t>> * img_output_orientation;
};

#endif