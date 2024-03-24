// mkdir -p outputs bin
// g++ src/prototype.cpp -std=c++20 -I../3rdparty/hbpp/include `pkg-config --cflags --libs opencv4` -o bin/prototype
// bin/prototype ../inputs/monarch.png outputs/result.png
// if .so not found: sudo touch /etc/ld.so.conf.d/opencv.conf -> write opencv lib path to this file -> sudo ldconfig -v | grep "opencv"

#include <iostream>
#include "hbpp.hpp"
#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;
using std::string;

void sepia(cv::Mat &img_input, cv::Mat &img_output);

int main(int argc, char ** argv) {
    if (argc < 3) {
        cout << "Usage: bin/prototype <path/to/input> <path/to/output>\n";
        return -1;
    }

    const string path_input = argv[1];
    const string path_output = argv[2];

    cv::Mat img_input = cv::imread(path_input, cv::IMREAD_COLOR);

    cv::Mat img_output;

    hbpp::BenchmarkStats stats = hbpp::benchmark(50, 1, [&]{
        sepia(img_input, img_output);
    });
    cout << "Best time: " << stats.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats.mean_time_ms << endl;
    cout << "Worst time: " << stats.worst_time_ms << " [ms]" << endl;
    cout << endl;

    cv::imshow("OpenCV result", img_output);
    char c = (char)cv::waitKey(20000);
    if( c == 27 ) { cv::destroyAllWindows(); }

    cv::imwrite(path_output, img_output);
}

void sepia(cv::Mat &img_input, cv::Mat &img_output) {
    cv::Mat kernel = (
        cv::Mat_<float>(3, 3) <<
                                    0.272, 0.534, 0.131,
                                    0.349, 0.686, 0.168,
                                    0.393, 0.769, 0.189
    );

    cv::transform(img_input, img_output, kernel);
}
