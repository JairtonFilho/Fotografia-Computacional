#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "hbpp.hpp"
#include "gradient.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char ** argv) {
    if (argc < 5) {
        cout << "Usage: bin/main <path/to/input> <threshold> <path/to/magnitude> <path/to/orientation>\n";
        return -1;
    }

    const string path_input = argv[1];
    const int threshold = atoi(argv[2]);
    const string path_magnitude = argv[3];
    const string path_orientation = argv[4];

    Buffer<uint8_t> input = load_image(path_input);
    Buffer<uint8_t> magnitude(input.width(), input.height());
    Buffer<uint8_t> orientation(input.width(), input.height());

    hbpp::BenchmarkStats stats = hbpp::benchmark(100, 1, [&]{
        gradient(input, threshold, magnitude, orientation);
    });
    cout << "Best time: " << stats.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats.worst_time_ms << " [ms]" << endl;
    cout << endl;

    save_image(magnitude, path_magnitude);
    save_image(orientation, path_orientation);

    return 0;
}