#include "ImageProcessor.h"
#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

const std::string PYTHON_SCRIPT_PATH = "/Users/johannestscharn/Repositories/c38robot-controller/x38robot-controller/Scripts/hand_tracking.py";

ImageProcessor::ImageProcessor() {
    static py::scoped_interpreter guard{};
}

ImageProcessor::~ImageProcessor() {
    // Python interpreter finalizes automatically
}

bool ImageProcessor::getIsActive() const{
    return isActive;
}

void ImageProcessor::initialize(IKSolver* InIKSolver){
    ikSolver = InIKSolver;
}

void ImageProcessor::runHandTracking() {
    if (ikSolver == nullptr) {
        std::cerr << "Error: IK Solver not initialized!" << std::endl;
        return;
    }
    // ----------------------------------------------------------------------------------------------
    // Python setup using pybind11
    // ----------------------------------------------------------------------------------------------
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
    // ----------------------------------------------------------------------------------------------

    // Open the webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return;
    }

    cv::namedWindow("Hand Tracking", cv::WINDOW_AUTOSIZE); // Ensure window creation
    cv::Mat frame;
    isActive = true;
    framesSinceUpdate = 0;
    prevCoordinates = std::make_tuple(0.0f, 0.0f, 0.0f);
    accumulatedDelta = std::make_tuple(0.0f, 0.0f, 0.0f);

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            break;
        }

        // Process the frame and get both the image and the index tip coordinates
        auto [processed_frame, currentCoordinates] = this->process_video_frame(frame);

        // Calculate Delta
        auto [current_x, current_y, current_z] = currentCoordinates;
        auto [prev_x, prev_y, prev_z] = prevCoordinates;
        std::tuple<float, float, float> delta = std::make_tuple(
            current_x - prev_x,
            current_y - prev_y,
            current_z - prev_z
        );

        // Accumulate the delta
        auto [acc_x, acc_y, acc_z] = accumulatedDelta;
        accumulatedDelta = std::make_tuple(
            acc_x + std::get<0>(delta),
            acc_y + std::get<1>(delta),
            acc_z + std::get<2>(delta)
        );
        framesSinceUpdate++;


        // Pass delta to the IKSolver
        if (framesSinceUpdate >= updateRate){
            ikSolver->solve(delta);
            framesSinceUpdate = 0;
            accumulatedDelta = std::make_tuple(0.0f, 0.0f, 0.0f);
        }

        prevCoordinates = currentCoordinates;


        cv::imshow("Hand Tracking", processed_frame);
        
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    cap.release();
    cv::destroyAllWindows();
}


std::pair<cv::Mat, std::tuple<float, float, float>> ImageProcessor::process_video_frame(const cv::Mat& frame) {
    try {
        py::array np_frame = mat_to_numpy(frame);

        // Process frame with Python
        py::object result = process_frame_py(np_frame);

        // Extract processed image and index tip coordinates from the Python result
        auto result_tuple = result.cast<std::tuple<py::array, std::tuple<float, float, float>>>();

        py::array processed_np_frame = std::get<0>(result_tuple);
        std::tuple<float, float, float> index_tip_coordinates = std::get<1>(result_tuple);
        cv::Mat processed_frame = numpy_to_mat(processed_np_frame);
        return {processed_frame, index_tip_coordinates};
    } 
    catch (const std::exception &e) {
        std::cerr << "Error processing video frame: " << e.what() << std::endl;
        return {frame, {0.0f, 0.0f, 0.0f}};  // Return the original frame if processing fails
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
