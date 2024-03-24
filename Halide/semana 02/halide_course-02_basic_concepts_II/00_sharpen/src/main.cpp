#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "sharpen_simple.h"
#include "sharpen_full.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char ** argv) {
    if (argc < 4) {
        cout << "Usage: bin/main <path/to/input> <path/to/output/simple> <path/to/output/full>\n";
        return -1;
    }

    const string path_input = argv[1];
    const string path_output_simple = argv[2];
    const string path_output_full = argv[3];

    Buffer<uint8_t> input = load_image(path_input);
    Buffer<uint8_t> output_simple = Buffer<uint8_t>::make_with_shape_of(input);
    Buffer<uint8_t> output_full = Buffer<uint8_t>::make_with_shape_of(input);

    sharpen_simple(input, output_simple);
    sharpen_full(input, output_full);

    save_image(output_simple, path_output_simple);
    save_image(output_full, path_output_full);

    return 0;
}