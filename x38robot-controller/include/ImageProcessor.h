#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();

    // Initializes Python environment and loads the hand tracking script
    void initialize_hand_tracking();

    // Processes the video frame and returns the processed frame
    cv::Mat process_video_frame(const cv::Mat& frame);

private:
    py::object process_frame_py;
    py::array mat_to_numpy(const cv::Mat& mat);
    cv::Mat numpy_to_mat(const py::array_t<uint8_t>& input);
};

#endif // IMAGEPROCESSOR_H
