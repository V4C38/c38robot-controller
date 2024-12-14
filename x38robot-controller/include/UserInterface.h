#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "ArmState.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSplitter>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongMaterial>

#ifndef slots
#define slots Q_SLOTS
#endif

#ifndef signals
#define signals Q_SIGNALS
#endif

class UserInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserInterface(QWidget *parent = nullptr);

    void setIsProcessing(bool InIsProcessing);
    int getSelectedProcessor() const;
    QComboBox* getSerialPortComboBox();
    QString getSelectedSerialPort() const;

signals:
    void homingSequenceRequested(int selectedHomingMode);
    void emergencyStopRequested();
    void testRequested(int selectedTestMode);
    void toggleIsProcessing();

public slots:
    void update3DRender(const std::vector<JointData>& jointData);

private slots:
    void onToggleIsProcessing();
    void onEmergencyStopRequested();
    void onHomingSequenceRequested();
    void onTestRequested();
    void onProcessorSelected(int index);

private:
    int selectedProcessor = 0;
    QVBoxLayout* mainLayout;

    // 3D window members
    Qt3DExtras::Qt3DWindow* view; 
    QWidget* viewContainer;
    Qt3DCore::QEntity* rootEntity = nullptr;

    // Title
    QLabel* titleLabel;

    // Processor Commands
    QVBoxLayout* processorLayout;
    QLabel* processorLabel;
    QComboBox* processorComboBox;
    QPushButton* startStopButton;

    // Homing Command
    QComboBox* homingComboBox;
    QPushButton* homingSequenceButton;

    // Test Command
    QComboBox* testComboBox;
    QPushButton* testButton;

    // Serial Port Dropdown
    QComboBox* serialPortComboBox;
    QPushButton* emergencyStopButton;
};

#undef slots
#undef signals

#endif // USERINTERFACE_H
