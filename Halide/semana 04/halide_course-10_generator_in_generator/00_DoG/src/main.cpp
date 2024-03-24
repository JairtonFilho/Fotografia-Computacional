#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "dog.h"

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
    Buffer<uint8_t> output(input.width(), input.height());

    dog(input, output);

    save_image(output, path_output);

    return 0;
}