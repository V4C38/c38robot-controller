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

// POSIX serial port includes
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Include for directory listing
#include <QDir>
#include <QFileInfoList>

int serialPortFD = -1;

// Function to open the serial port
bool openSerialPort(const std::string& portName) {
    serialPortFD = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPortFD == -1) {
        perror("Unable to open serial port");
        return false;
    }

    // Configure serial port
    struct termios options;
    tcgetattr(serialPortFD, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(serialPortFD, TCSANOW, &options);
    return true;
}

// Function to close the serial port
void closeSerialPort() {
    if (serialPortFD != -1) {
        close(serialPortFD);
        serialPortFD = -1;
    }
}

// Function to write data to the serial port
void writeToSerialPort(const std::string& data) {
    if (serialPortFD != -1) {
        write(serialPortFD, data.c_str(), data.size());
    }
}

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
    // Start/Stop UI button pressed
    QObject::connect(&ui, &UserInterface::toggleIsProcessing, [&]() 
    {
        if (isProcessing.load()) 
        {
            // Stop processing
            processor->stop();
            isProcessing.store(false);
            ui.setIsProcessing(false);
        } 
        else
        {
            // Start processing
            ui.setIsProcessing(true);
            processor->initialize(&ikSolver);
            processor->run();
            isProcessing.store(false);
            ui.setIsProcessing(false);
        }
    });

    // Homing Sequence Request
    QObject::connect(&ui, &UserInterface::homingSequenceRequested, [&](int selectedHomingMode) {
        std::string command = "RunHomingSequence " + std::to_string(selectedHomingMode) + "\n";
        writeToSerialPort(command);
    });

    // Test Command Request
    QObject::connect(&ui, &UserInterface::testRequested, [&](int selectedTestMode) {
        std::string command = "RunTest " + std::to_string(selectedTestMode) + "\n";
        writeToSerialPort(command);
    });

    // Serial Port Selection: When a serial port is selected
    QObject::connect(ui.getSerialPortComboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
        closeSerialPort();

        std::string selectedPort = ui.getSelectedSerialPort().toStdString();
        if (selectedPort != "Select Serial Port" && !selectedPort.empty()) {
            if (openSerialPort(selectedPort)) {
                qDebug() << "Serial port opened: " << QString::fromStdString(selectedPort);
            } else {
                qWarning() << "Failed to open serial port: " << QString::fromStdString(selectedPort);
            }
        }
    });

    // Populate available serial ports (specific to macOS)
    QDir devDir("/dev");
    QFileInfoList portList = devDir.entryInfoList(QStringList() << "cu.*", QDir::System | QDir::Readable);
    for (const QFileInfo &portInfo : portList) {
        ui.getSerialPortComboBox()->addItem(portInfo.absoluteFilePath());
    }

    if (ui.getSerialPortComboBox()->count() == 1) {  // Only "Select Serial Port" is present
        ui.getSerialPortComboBox()->addItem("No available serial ports");
    }

    ui.show();
    return app.exec();
}
