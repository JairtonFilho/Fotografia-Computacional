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

        GeneratorParam<int> custom_schedule{"custom_schedule", 2};

        Output<Buffer<uint8_t>> img_output{"img_output", 2};        

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
        }

        void schedule() {
            switch (custom_schedule) {
                case 0:
                    /*
                    // Implícito:
                    img_output.compute_root();
                    thresh.compute_inline();
                    magnitude.compute_inline();
                    horiz_gradient.compute_inline();
                    horiz_gradient_intm.compute_inline();
                    vert_gradient.compute_inline();
                    vert_gradient_intm.compute_inline();
                    img_gray.compute_inline();
                    */
                    break;
                
                case 1:
                    img_output.reorder(y, x);

                    /*
                    Passar ao reorder as variáveis em ordem de mais interna à mais externa!
                    */
                    break;

                case 2:
                    img_output.compute_root();
                    img_gray.compute_root();
                    break;

                case 3:
                    img_output.compute_root();
                    magnitude.compute_root();
                    img_gray.compute_root();
                    break;

                case 4:
                    img_output.compute_root();
                    magnitude.compute_root();
                    horiz_gradient.compute_root();
                    vert_gradient.compute_root();
                    img_gray.compute_root();
                    break;

                case 5:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y);
                    break;

                case 6:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, x);
                    break;

                case 7:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    break;

                case 8:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, x).store_root();
                    break;

                case 9:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, y);
                    vert_gradient.compute_at(img_output, y);
                    break;

                case 10:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, y).store_root();
                    vert_gradient.compute_at(img_output, y).store_root();
                    break;

                case 11:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, x).store_root();
                    vert_gradient.compute_at(img_output, x).store_root();
                    break;

                case 12:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, y).store_root();
                    vert_gradient
                        .compute_at(img_output, y)
                        .store_root()
                        .compute_with(horiz_gradient, x);
                    break;

                case 13:
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, x).store_root();
                    vert_gradient
                        .compute_at(img_output, x)
                        .store_root()
                        .compute_with(horiz_gradient, x);;
                    break;

                default:
                    break;
            }

            img_output.print_loop_nest();
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Func img_bound{"img_bound"}, img_gray{"img_gray"};
        Func vert_gradient_intm{"vert_gradient_intm"}, vert_gradient{"vert_gradient"};
        Func horiz_gradient_intm{"horiz_gradient_intm"}, horiz_gradient{"horiz_gradient"};
        Func magnitude{"magnitude"};
        Func thresh{"thresh"};
};

#endif