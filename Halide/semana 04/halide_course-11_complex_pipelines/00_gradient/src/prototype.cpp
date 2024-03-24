// mkdir -p outputs bin
// g++ src/prototype.cpp -std=c++2a -O3 -I../3rdparty/hbpp/include `pkg-config --cflags --libs opencv4` -o bin/prototype
// bin/prototype ../inputs/monarch.png 50 outputs/result.png
// if .so not found: sudo touch /etc/ld.so.conf.d/opencv.conf -> write opencv lib path to this file -> sudo ldconfig -v | grep "opencv"

#include <iostream>
#include "hbpp.hpp"
#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;
using std::string;

void gradient(cv::Mat &img_input, uint32_t threshold, cv::Mat &img_output);

int main(int argc, char ** argv) {
    if (argc < 4) {
        cout << "Usage: bin/prototype <path/to/input> <threshold> <path/to/output>\n";
        return -1;
    }

    const string path_input = argv[1];
    const int threshold = atoi(argv[2]);
    const string path_output = argv[3];

    cv::Mat img_input = cv::imread(path_input, cv::IMREAD_COLOR);

    cv::Mat img_output;

    hbpp::BenchmarkStats stats = hbpp::benchmark(50, 1, [&]{
        gradient(img_input, threshold, img_output);
    });
    cout << "Best time: " << stats.best_time_ms << " [ms]" << endl;
    cout << "Mean time: " << stats.mean_time_ms << " [ms]" << endl;
    cout << "Worst time: " << stats.worst_time_ms << " [ms]" << endl;
    cout << endl;

    cv::imshow("OpenCV result", img_output);
    char c = (char)cv::waitKey(20000);
    if( c == 27 ) { cv::destroyAllWindows(); }

    cv::imwrite(path_output, img_output);
}

void gradient(cv::Mat &img_input, uint32_t threshold, cv::Mat &img_output) {
    int32_t k_vert[3][3] = {
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1}
    };
    cv::Mat kernel_vert(3, 3, CV_32S, k_vert);

    int32_t k_horiz[3][3] = {
        {1,  0, -1},
        {2,  0, -2},
        {1,  0, -1}
    };
    cv::Mat kernel_horiz(3, 3, CV_32S, k_horiz);

    cv::Mat img_gray;
    cv::cvtColor(img_input, img_gray, cv::COLOR_BGR2GRAY);
    img_gray.convertTo(img_gray, CV_32F);

    cv::Mat gradient_vert;
    cv::filter2D(img_gray, gradient_vert, -1, kernel_vert, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);

    cv::Mat gradient_horiz;
    cv::filter2D(img_gray, gradient_horiz, -1, kernel_horiz, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);

    cv::multiply(gradient_vert, gradient_vert, gradient_vert);
    cv::multiply(gradient_horiz, gradient_horiz, gradient_horiz);

    cv::Mat mag;
    cv::add(gradient_horiz, gradient_vert, mag);

    cv::Mat result;
    cv::threshold(mag, result, (threshold * threshold) * 4 * 4, 255, cv::THRESH_BINARY_INV);

    cv::convertScaleAbs(result, img_output);
}
