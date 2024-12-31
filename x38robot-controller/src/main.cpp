#include <QApplication>
#include "UserInterface.h"

#ifdef slots
#undef slots
#endif

#include "AbstractProcessor.h"
#include "ImageProcessor.h"
# include "ArmState.h"
#include "IKSolver.h"

#include <thread>
#include <atomic>

// QT
#include <QDir>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>


int serialPortFD = -1;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize components
    std::atomic<bool> isDebugMode{false};
    std::thread processorThread;
    std::unique_ptr<AbstractProcessor> processor = std::make_unique<ImageProcessor>();

    QString configPath = QDir(QCoreApplication::applicationDirPath()).filePath("../config/robot_arm_config.ini");
    std::shared_ptr<ArmState> armState = std::make_shared<ArmState>(nullptr, configPath.toStdString());
    std::shared_ptr<SerialInterface> serialInterface = std::make_shared<SerialInterface>(armState);
    std::shared_ptr<UserInterface> userInterface = std::make_shared<UserInterface>(nullptr, armState, serialInterface);
    IKSolver ikSolver;

    QObject::connect(serialInterface.get(), &SerialInterface::onAvailableSerialPortsUpdated,
                    userInterface.get(), &UserInterface::updateAvailableSerialPorts);

    userInterface->show();
    return app.exec();
}
