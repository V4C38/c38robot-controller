#include "include/main.h"
#include <opencv2/opencv.hpp>
#include <iostream>


int main() {
    // Initialize video capture (default camera)
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera stream!" << std::endl;
        return -1;
    }

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera stream!" << std::endl;
        return -1;
    }

    // Create a display window
    cv::namedWindow("Webcam Video", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Webcam Video", 100, 100);  // Position window at (100, 100)

    while (true) {
        cv::Mat frame;
        cap >> frame;  // Capture a frame

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured!" << std::endl;
            break;
        }

        cv::imshow("Webcam Video", frame);  // Display the frame in the window

        // Exit loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

