#include <QApplication>
#include "UserInterface.h"

#ifdef slots
#undef slots
#endif

#include "AbstractProcessor.h"
#include "ImageProcessor.h"
#include "IKSolver.h"
#include <thread>
#include <atomic>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize components
    std::atomic<bool> isProcessing{false};
    std::thread processorThread;
    std::unique_ptr<AbstractProcessor> processor = std::make_unique<ImageProcessor>();
    IKSolver ikSolver;

    UserInterface ui;


    // ----------------------------------------------------------------------------------------------
    // UI Interaction events
    // ----------------------------------------------------------------------------------------------
    // Start/Stop ui button pressed
    QObject::connect(&ui, &UserInterface::toggleIsProcessing, [&]() 
    {
        if (isProcessing.load()) 
        {
            // Stop processing
            processor->stop();

            // ToDo: Add procession on separate Thread
            isProcessing.store(false);
            ui.setIsProcessing(false);
        } 
        else
        {
            // Start processing
            ui.setIsProcessing(true);

            // ToDo: Add procession on separate Thread
            processor->initialize(&ikSolver);
            processor->run();

            isProcessing.store(true);
        }
    });

    // Homing Sequence Request
    QObject::connect(&ui, &UserInterface::homingSequenceRequested, [&]() {
        ikSolver.requestHomingSequence();
    });
    // ----------------------------------------------------------------------------------------------

    ui.show();
    return app.exec();
}
