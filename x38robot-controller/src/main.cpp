#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
    ImageProcessor processor;

    // Initialize hand tracking system
    processor.initialize_hand_tracking();

    // Open the webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            break;
        }

        // Process frame
        cv::Mat processed_frame = processor.process_video_frame(frame);
        cv::imshow("Hand Tracking", processed_frame);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
