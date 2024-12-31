#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "ArmState.h"
#include "Commands.h"
#include "SerialInterface.h"
#include "RuntimeSettings.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabBar>
#include <QSplitter>
#include <QCheckBox>
#include <QStackedWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>

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
    explicit UserInterface(QWidget *parent = nullptr, 
        std::shared_ptr<ArmState> armState = nullptr,
        std::shared_ptr<SerialInterface> serialInterface = nullptr);

    int getSelectedDriver() const;

    QComboBox* getSerialPortComboBox();
    QString getSelectedSerialPort() const;


public slots:
    void updateArmState(const std::vector<JointData>& inJointData);
    void setJointData(const std::vector<JointData>& jointData);
    void updateAvailableSerialPorts(const QStringList& ports);
    void setDriverState(const bool isActive);

private slots:
    void onTabChanged(int index);
    void onSerialPortUpdateRequested();
    void onEmergencyStopRequested();
    void onHomingSequenceRequested();
    void onTestRequested();
    void onSelectAxisAngle(int axis, float angle);

    void setDebugMode(const bool enabled);
    void driverSelected(int index);
    void toggleDriverState();

private:
    void update3DRender(const std::vector<JointData>& inJointData);
    std::shared_ptr<ArmState> armState;
    std::shared_ptr<SerialInterface> serialInterface;

    int selectedDriver = 0;

    // Layout
    QVBoxLayout* mainLayout = nullptr;
    QSplitter *mainSplitter = nullptr;
    QHBoxLayout *mainHorizontalLayout = nullptr;
    QStackedWidget *stackedWidget = nullptr;
    QTabBar *tabBar = nullptr;

    // Main Control Section
    QComboBox* serialPortComboBox = nullptr;
    QPushButton* serialPortUpdateButton = nullptr;
    QPushButton* emergencyStopButton = nullptr;
    QComboBox* homingComboBox = nullptr;
    QPushButton* homingSequenceButton = nullptr;

    // Driver Section
    QVBoxLayout* driverLayout = nullptr;
    QLabel* driverLabel = nullptr;
    QComboBox* driverComboBox = nullptr;
    QPushButton* driverStartStopButton = nullptr;

    // Test Section
    QComboBox* testComboBox = nullptr;
    QPushButton* testButton = nullptr;
    QCheckBox* debugModeCheckBox = nullptr;

    // Preview Window (3D / Sliders)
    QVBoxLayout *previewLayout = nullptr;

    QWidget* viewContainer;
    Qt3DExtras::Qt3DWindow* view = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    QVector<Qt3DCore::QEntity*> jointEntities;
    QVector<Qt3DCore::QTransform*> jointTransforms;


    QVBoxLayout *sliderLayout = nullptr;
    QVector<QSlider*> axisSliders;
    QVector<QLineEdit*> axisValueLabels;

};

#undef slots
#undef signals

#endif // USERINTERFACE_H
