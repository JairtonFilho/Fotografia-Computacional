#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"

using std::cout;
using std::endl;
using namespace Halide;
using namespace Halide::ConciseCasts;

int main(int argc, char ** argv) {
    if (argc < 3) {
        cout << "Usage: bin/main <path/to/input> <path/to/output>" << endl;
        return -1;
    }

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];

    Buffer<uint8_t> input = Tools::load_image(input_filename);
    Buffer<uint8_t> output(input.width(), input.height(), input.channels());

    Var x{"x"}, y{"y"}, c{"c"};
    Func input_f32{"input_f32"};
    input_f32(x, y, c) = f32(input(x, y, c)); // Convert to float32

    Expr r, g, b;
    r = input_f32(x, y, 0);
    g = input_f32(x, y, 1);
	b = input_f32(x, y, 2);

    // Transform image
    Expr nr, ng, nb;
    nr = (r * 0.393f) + (g * 0.796f) + (b * 0.189f);
    ng = (r * 0.349f) + (g * 0.686f) + (b * 0.164f);
    nb = (r * 0.272f) + (g * 0.534f) + (b * 0.131f);

    // Cast to uint8 and merge channels into result image
    Func sepia{"sepia"};
    sepia(x, y, c) = u8(0); // Need to have a pure definition before indexing
    sepia(x, y, 0) = u8_sat(nr);
    sepia(x, y, 1) = u8_sat(ng);
    sepia(x, y, 2) = u8_sat(nb);

    output = sepia.realize({input.width(), input.height(), input.channels()});

    Tools::save_image(output, output_filename);

    return 0;
}