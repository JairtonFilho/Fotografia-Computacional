#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"

using std::cout;
using std::cin;
using std::endl;
using namespace Halide;
using namespace Halide::ConciseCasts;

int main(int argc, char ** argv) {
    if (argc < 4) {
        cout << "Usage: bin/main <path/to/input> <gamma> <path/to/output>" << endl;
        return -1;
    }

    const std::string input_filename = argv[1];
    const float gamma = atof(argv[2]);
    const std::string output_filename = argv[3];

    if (gamma < 0) {
    	cout << "Gamma out of proper range!" << endl;
    	return -1;
    }

    Buffer<uint8_t> input = Tools::load_image(input_filename);
    Buffer<uint8_t> output(input.width(), input.height(), input.channels());

    Var x{"x"}, y{"y"}, c{"c"};
    Func input_f32{"input_f32"};
    input_f32(x, y, c) = f32(input(x, y, c)); // Convert to float32

    // Transform image
    Expr rn, gn, bn;
    rn = input_f32(x, y, 0) / 255.f;
    gn = input_f32(x, y, 1) / 255.f;
    bn = input_f32(x, y, 2) / 255.f;

    Expr rt, gt, bt;
    rt = pow(rn, gamma);
    gt = pow(gn, gamma);
    bt = pow(bn, gamma);

    // Cast to uint8 and merge channels into result image
    Func gamma_correction{"gamma_correction"};
    gamma_correction(x, y, c) = u8(0); // Need to have a pure definition before indexing
    gamma_correction(x, y, 0) = u8_sat(255.0f * clamp(rt, 0, 1));
    gamma_correction(x, y, 1) = u8_sat(255.0f * clamp(rt, 0, 1));
    gamma_correction(x, y, 2) = u8_sat(255.0f * clamp(rt, 0, 1));

    output = gamma_correction.realize({input.width(), input.height(), input.channels()});
    Tools::save_image(output, output_filename);

    return 0;
}