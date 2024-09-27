#include "ImageProcessor.h"
#include <pybind11/embed.h>  // Ensure this is included for embedding Python
#include <iostream>

namespace py = pybind11;

const std::string PYTHON_SCRIPT_PATH = "/Users/johannestscharn/Repositories/c38robot-controller/x38robot-controller/Scripts/hand_tracking.py";

ImageProcessor::ImageProcessor() {
    static py::scoped_interpreter guard{};  // Ensure Python interpreter is initialized once
}

ImageProcessor::~ImageProcessor() {
    // Python interpreter will automatically finalize
}

void ImageProcessor::initialize_hand_tracking() {
    try {
        // Add the 'Scripts' directory to Python's sys.path
        py::exec("import sys\nsys.path.append('" + PYTHON_SCRIPT_PATH.substr(0, PYTHON_SCRIPT_PATH.rfind('/')) + "')");

        // Import the hand_tracking Python module and store the apply_hand_tracking function
        py::object hand_tracking = py::module::import("hand_tracking");
        process_frame_py = hand_tracking.attr("apply_hand_tracking");  // Update to 'apply_hand_tracking'
    } catch (const std::exception &e) {
        std::cerr << "Error initializing hand tracking: " << e.what() << std::endl;
        throw;
    }
}


cv::Mat ImageProcessor::process_video_frame(const cv::Mat& frame) {
    try {
        py::array np_frame = mat_to_numpy(frame);
        py::object result_frame = process_frame_py(np_frame);
        return numpy_to_mat(result_frame.cast<py::array>());
    } 
    catch (const std::exception &e) {
        std::cerr << "Error processing video frame: " << e.what() << std::endl;
        return frame;  // Return the original frame if processing fails
    }
}


// ------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------
py::array ImageProcessor::mat_to_numpy(const cv::Mat& mat) {
    if (!mat.isContinuous()) {
        throw std::runtime_error("cv::Mat must be continuous for conversion.");
    }

    int channels = mat.channels();
    std::vector<std::size_t> shape = { (std::size_t)mat.rows, (std::size_t)mat.cols, (std::size_t)channels };
    std::vector<std::size_t> strides = { (std::size_t)(mat.step[0]), (std::size_t)(mat.step[1]), (std::size_t)(1) };

    return py::array(py::buffer_info(
        mat.data,
        sizeof(uint8_t),
        py::format_descriptor<uint8_t>::format(),
        3,
        shape,
        strides
    ));
}

cv::Mat ImageProcessor::numpy_to_mat(const py::array_t<uint8_t>& input) {
    py::buffer_info buf_info = input.request();
    if (buf_info.ndim != 3 || buf_info.shape[2] != 3) {
        throw std::runtime_error("Incompatible buffer dimensions!");
    }
    return cv::Mat(buf_info.shape[0], buf_info.shape[1], CV_8UC3, (void*)buf_info.ptr);
}
