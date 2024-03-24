#ifndef HALIDE_HISTEQ_HPP
#define HALIDE_HISTEQ_HPP

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideHistEq : public Generator<HalideHistEq> {
    public:
        Input<Buffer<uint8_t>> img_input{"img_input", 2};
        Output<Buffer<uint8_t>> img_output{"img_output", 2};

        void generate() {
            RDom r(img_input);
            histogram(i) = 0;
            histogram(img_input(r.x, r.y)) += 1;

            RDom rc(1, 255);
            cumulative_hist(i) = 0;
            cumulative_hist(0) = histogram(0);
            cumulative_hist(rc) = cumulative_hist(rc - 1) + histogram(rc);

            map(i) = u8(
                cumulative_hist(i) * (255.0f / cumulative_hist(255))
            );

            img_output(x, y) = map(img_input(x, y));
        }
        
        void schedule() {
            img_input.set_estimates({{0, 512}, {0, 512}});
            img_output.set_estimates({{0, 512}, {0, 512}});
        }

    private:
        Var x{"x"}, y{"y"}, c{"c"}, i{"i"};
        Func histogram{"histogram"}, cumulative_hist{"cumulative_hist"};
        Func map{"map"};
};

#endif