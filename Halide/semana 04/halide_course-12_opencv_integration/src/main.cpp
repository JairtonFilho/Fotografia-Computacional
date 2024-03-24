#include <iostream>
#include <opencv2/opencv.hpp>
#include "HalideBuffer.h"
#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "gradient.h"

using std::cout;
using std::string;
using std::endl;

using namespace Halide::Runtime;
using namespace Halide::Tools;

template <typename T>
void mat2buffer(cv::Mat& mat, Buffer<T>& buffer)
{
    int n_channels = mat.channels();
    if (n_channels == 1)
    {
        for (int j = 0; j < mat.rows; j++)
            for (int i = 0; i < mat.cols; i++)
                buffer(i, j) = mat.at<T>(j, i);
    }
    else if (n_channels == 3)
    {
        for (int j = 0; j < mat.rows; j++)
        {
            for (int i = 0; i < mat.cols; i++)
            {
                buffer(i, j, 0) = mat.at<T>(j, 3 * i);
                buffer(i, j, 1) = mat.at<T>(j, 3 * i + 1);
                buffer(i, j, 2) = mat.at<T>(j, 3 * i + 2);
            }
        }
    }
}

template <typename T>
void buffer2mat(const Buffer<T>& buffer, cv::Mat& mat)
{
    
    int n_channels = mat.channels();
    if (n_channels == 1)
    {
        for (int j = 0; j < mat.rows; j++)
        {
            for (int i = 0; i < mat.cols; i++)
            {
                mat.at<T>(j, i) = buffer(i, j);
            }
        }
    }
    else if (n_channels == 3)
    {
        for (int j = 0; j < mat.rows; j++)
        {
            for (int i = 0; i < mat.cols; i++)
            {
                mat.at<T>(j, 3 * i + 0) = buffer(mat.cols - i - 1, j, 0);
                mat.at<T>(j, 3 * i + 1) = buffer(mat.cols - i - 1, j, 1);
                mat.at<T>(j, 3 * i + 2) = buffer(mat.cols - i - 1, j, 2);
            }
        }
    }
}


int main(int argc, char ** argv) {
    if (argc > 1) {
        printf("Usage: ./main\n");
        return 0;
    }
    
    cv::Mat cv_input;
    cv_input = cv::imread("inputs/monarch.png");

    Buffer<uint8_t> input(cv_input.cols, cv_input.rows, cv_input.channels());
    Buffer<uint8_t> output(cv_input.cols, cv_input.rows);
    cv::Mat output_image(cv::Size(input.width(), input.height()), CV_8U);

    mat2buffer(cv_input, input);
    gradient(input, output);
    buffer2mat(output, output_image);

    cv::namedWindow("Result", cv::WINDOW_NORMAL);
    while (true) {
        cv::imshow("Result", output_image);
        char c = char(cv::waitKey(1));
        if(c == 27)
            break;
    }
    cv::destroyAllWindows();
    
    cv::imwrite("output.png", output_image);
    return 0;
}

