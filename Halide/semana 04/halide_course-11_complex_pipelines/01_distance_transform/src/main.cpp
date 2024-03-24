#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "hbpp.hpp"
#include "distance_transform.h"

using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

void save_distance_transform(Buffer<int32_t> dt, string path_to_out) {
    Buffer<uint8_t> to_save = Buffer<uint8_t>(dt.width(), dt.height());
    int32_t max_val = -1;
    dt.for_each_value([&](int32_t val){
        if (val > max_val)
            max_val = val;
    });
    to_save.for_each_element([&](int x, int y){
        to_save(x, y) = uint8_t(255.0f * float(dt(x, y)) / max_val);
    });
    save_image(to_save, path_to_out);
}

int main(int argc, char ** argv) {
    if (argc < 3) {
        cout << "Usage: bin/main <path/to/input> <path/to/output>\n";
        return -1;
    }

    const string path_input = argv[1];
    const string path_output = argv[2];

    Buffer<uint8_t> input = load_image(path_input);
    Buffer<int32_t> output(input.width(), input.height());

    hbpp::BenchmarkStats stats = hbpp::benchmark(3, 1, [&]{
        distance_transform(input, output);
    });
    cout << "Best time: " << stats.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats.worst_time_ms << " [ms]" << endl;
    cout << endl;

    save_distance_transform(output, path_output);

    return 0;
}