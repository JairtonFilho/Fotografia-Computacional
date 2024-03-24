#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "hbpp.hpp"
#include "sepia.h"

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
    Buffer<uint8_t> output(input.width(), input.height(), input.channels());

    sepia(input, output);

    hbpp::BenchmarkStats stats = hbpp::benchmark(100, 1, [&]{
        sepia(input, output);
    });
    cout << "Best time: " << stats.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats.worst_time_ms << " [ms]" << endl;
    cout << endl;

    save_image(output, path_output);

    return 0;
}