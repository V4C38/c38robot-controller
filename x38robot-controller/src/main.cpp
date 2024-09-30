#include "ImageProcessor.h"
#include "IKSolver.h"

int main(int argc, char* argv[]) {
    ImageProcessor imageProcessor;
    IKSolver IKSolver;

    imageProcessor.initialize(&IKSolver);
    imageProcessor.runHandTracking();

    return 0;
}
