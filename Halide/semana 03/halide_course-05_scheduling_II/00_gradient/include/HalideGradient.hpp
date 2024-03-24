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
            int vector_size = get_target().natural_vector_size(Float(32));

            switch (custom_schedule) {
                case 0:
                    // for y:
                    // | for x:
                    // | | img_gray(x, y)
                    // for y:
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root();
                    img_gray.compute_root();
                    break;

                case 1:
                    // allocate img_gray
                    // for y:
                    // | for y:
                    // | | for x:
                    // | | | img_gray(x, y)
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    break;

                case 2:
                    // allocate img_gray
                    // for y:
                    // | for y:
                    // | | for x:
                    // | | | img_gray(x, y)
                    // | for y:
                    // | | for x:
                    // | | | horiz_gradient(x, y)
                    // | for y:
                    // | | for x:
                    // | | | vert_gradient(x, y)
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, y);
                    vert_gradient.compute_at(img_output, y);
                    break;

                case 3:
                    // allocate img_gray
                    // allocate horiz_gradient
                    // allocate vert_gradient
                    // for y:
                    // | for y:
                    // | | for x:
                    // | | | img_gray(x, y)
                    // | for y:
                    // | | for x:
                    // | | | horiz_gradient(x, y)
                    // | | | vert_gradient(x, y)
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, y).store_root();
                    vert_gradient
                        .compute_at(img_output, y)
                        .store_root()
                        .compute_with(horiz_gradient, x);
                    break;

                case 4:
                    // allocate img_gray
                    // allocate horiz_gradient
                    // allocate vert_gradient
                    // for y:
                    // | for y:
                    // | | for x:
                    // | | | img_gray(x, y)
                    // | for x:
                    // | | for y:
                    // | | | for x:
                    // | | | | horiz_gradient(x, y)
                    // | | | | vert_gradient(x, y)
                    // | | img_output(x, y)
                    img_output.compute_root();
                    img_gray.compute_at(img_output, y).store_root();
                    horiz_gradient.compute_at(img_output, x).store_root();
                    vert_gradient
                        .compute_at(img_output, x)
                        .store_root()
                        .compute_with(horiz_gradient, x);
                    break;

                case 5:
                    // for x:
                    // | for y:
                    // | | img_gray(x, y)
                    // for x:
                    // | for y:
                    // | | img_output(x, y)
                    img_output.compute_root().reorder(y, x);
                    img_gray.compute_root().reorder(y, x);
                    break;
                
                case 6:
                    // parallel y:
                    // | for x:
                    // | | img_gray(x, y)
                    // parallel y:
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root().parallel(y);
                    img_gray.compute_root().parallel(y);
                    break;
            
                case 7:
                    // for y:
                    // | for x:
                    // | | img_gray(x, y)
                    // for y:
                    // | for xo:
                    // | | vectorized xi:
                    // | | img_output(x, y)
                    img_output.compute_root().split(x, xo, xi, vector_size).vectorize(xi);
                    img_gray.compute_root();
                    break;
                
                case 8:
                    // for y:
                    // | for x:
                    // | | img_gray(x, y)
                    // parallel yo:
                    // | for yi:
                    // | | for xo:
                    // | | | vectorized xi:
                    // | | | | img_output(x, y)
                    img_output
                        .compute_root()
                        .split(y, yo, yi, 16).parallel(yo)
                        .split(x, xo, xi, vector_size).vectorize(xi)
                    ;
                    img_gray.compute_root();
                    break;

                case 9:    
                    // parallel yo:
                    // | allocate img_gray
                    // | for yi:
                    // | | for y:
                    // | | | for x:
                    // | | | | img_gray(x, y)
                    // | | for xo:
                    // | | | vectorized xi:
                    // | | | | img_output(x, y)
                    img_output
                        .compute_root()
                        .split(y, yo, yi, 512).parallel(yo)
                        .split(x, xo, xi, vector_size).vectorize(xi)
                    ;
                    img_gray.compute_at(img_output, yi).store_at(img_output, yo);
                    break;

                case 10:
                    // parallel yo,xo as tile_index:
                    // | for yi:
                    // | | for xi:
                    // | | | img_gray(x, y)
                    // parallel y
                    // | for x:
                    // | | img_output(x, y)
                    img_output.compute_root().parallel(y);
                    img_gray.compute_root()
                        .tile(x, y, xo, yo, xi, yi, 8*vector_size, 8*vector_size)
                        .fuse(xo, yo, tile_index)
                        .parallel(tile_index);
                    break;
                
                case 11:
                    // Schedule mais rápido que encontrei
                    img_output
                        .compute_root()
                        .parallel(y)
                    ;
                    img_gray
                        .compute_at(img_output, y)
                        .store_at(img_output, y)
                        .split(x, xo, xi, vector_size)
                        .vectorize(xi)
                    ;
                    horiz_gradient
                        .store_at(img_output, y)
                        .compute_at(img_output, y)
                    ;
                    vert_gradient
                        .store_at(img_output, y)
                        .compute_at(img_output, y)
                        .compute_with(horiz_gradient, x)
                    ;
                
                case 12:
                    // Schedule mais veloz obtido através dos autoschedulers de Halide
                    img_output
                        .compute_root()
                        .split(x, xo, xi, 32)
                        .vectorize(xi)
                        .parallel(y)
                    ;
                
                default:
                    break;
            }
            img_output.print_loop_nest();
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"};
        Var xi{"xi"}, xo{"xo"}, yi{"yi"}, yo{"yo"}, tile_index{"tile_index"};
        Func img_bound{"img_bound"}, img_gray{"img_gray"};
        Func vert_gradient_intm{"vert_gradient_intm"}, vert_gradient{"vert_gradient"};
        Func horiz_gradient_intm{"horiz_gradient_intm"}, horiz_gradient{"horiz_gradient"};
        Func magnitude{"magnitude"};
        Func thresh{"thresh"};
};

#endif