#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "histogram_equalization.h"
#include "rgb2gray.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char ** argv) {
    if (argc < 3) {
        cout << "Usage: bin/main <path/to/input> <path/to/output>\n";
        return -1;
    }

    const string path_input = argv[1];
    const string path_output = argv[2];

    Buffer<uint8_t> input = load_image(path_input);
    Buffer<uint8_t> input_gray(input.width(), input.height());
    Buffer<uint8_t> output(input.width(), input.height());

    rgb2gray(input, input_gray);
    histogram_equalization(input_gray, output);
    save_image(output, path_output);

    return 0;
}