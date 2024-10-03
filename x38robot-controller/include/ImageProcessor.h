#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "AbstractProcessor.h"
#include "IKSolver.h"
#include <opencv2/opencv.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class ImageProcessor : public AbstractProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();

    void initialize(IKSolver* InIKSolver) override;
    void run() override;
    void stop() override;
    bool isActive() const override;

    // Processes the video frame and returns the processed frame
    std::pair<cv::Mat, std::tuple<float, float, float>> process_video_frame(const cv::Mat& frame);

private:
    bool bIsActive = false;

    const int updateRate = 5;  // Send delta every n frames
    int framesSinceUpdate = 0;
    std::tuple<float, float, float> accumulatedDelta;

    // Python conversion helpers
    py::object process_frame_py;
    py::array mat_to_numpy(const cv::Mat& mat);
    cv::Mat numpy_to_mat(const py::array_t<uint8_t>& input);

    IKSolver* ikSolver = nullptr;
    std::tuple<float, float, float> prevCoordinates;
};

#endif // IMAGEPROCESSOR_H
