#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "rgb2hsl_update.h"
#include "rgb2hsl_select.h"
#include "rgb2hsl_mux.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char ** argv) {
    if (argc < 5) {
        cout << "Usage: bin/main <path/to/input> <path/to/output/update> <path/to/output/select> <path/to/output/mux>\n";
        return -1;
    }

    const string path_input = argv[1];
    const string path_output_update = argv[2];
    const string path_output_select = argv[3];
    const string path_output_mux = argv[4];

    Buffer<uint8_t> input = load_image(path_input);
    Buffer<uint8_t> output_update = Buffer<uint8_t>::make_with_shape_of(input);
    Buffer<uint8_t> output_select = Buffer<uint8_t>::make_with_shape_of(input);
    Buffer<uint8_t> output_mux = Buffer<uint8_t>::make_with_shape_of(input);

    rgb2hsl_update(input, output_update);
    rgb2hsl_select(input, output_select);
    rgb2hsl_mux(input, output_mux);

    save_image(output_update, path_output_update);
    save_image(output_select, path_output_select);
    save_image(output_mux, path_output_mux);

    return 0;
}