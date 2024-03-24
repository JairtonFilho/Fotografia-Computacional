#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "hbpp.hpp"
#include "gamma_correction.h"
#include "gamma_correction_lut.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char ** argv) {
    if (argc < 3) {
        cout << "Usage: bin/main <path/to/input> <path/to/output>" << endl;
        return -1;
    }

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];

    Buffer<uint8_t> input = load_image(input_filename);
    Buffer<uint8_t> output(input.width(), input.height(), input.channels());

    hbpp::BenchmarkStats stats1 = hbpp::benchmark(50, 1, [&]{
        gamma_correction(input, output);
    });
    cout << "Per pixel: \n";
    cout << "Best time: " << stats1.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats1.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats1.worst_time_ms << " [ms]" << endl;
    cout << endl;

    hbpp::BenchmarkStats stats2 = hbpp::benchmark(50, 1, [&]{
        gamma_correction_lut(input, output);
    });
    cout << "LUT: \n";
    cout << "Best time: " << stats2.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats2.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats2.worst_time_ms << " [ms]" << endl;
    cout << endl;

    save_image(output, output_filename);

    return 0;
}