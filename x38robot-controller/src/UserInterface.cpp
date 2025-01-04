#ifndef emit
#define emit
#endif

#include "UserInterface.h"
#include <QDebug>


QComboBox* UserInterface::getSerialPortComboBox() {
    return serialPortComboBox;
}

UserInterface::UserInterface(QWidget *parent, 
std::shared_ptr<ArmState> armState, std::shared_ptr<SerialInterface> serialInterface)
    : QMainWindow(parent), armState(std::move(armState)), serialInterface(std::move(serialInterface))
{
    if (!this->armState)
    {
        throw std::invalid_argument("ArmState pointer cannot be null");
    }
    if (!this->serialInterface)
    {
        throw std::invalid_argument("SerialInterface pointer cannot be null");
    }
    
    // -------------------------------------------------------------------
    // Connect Signals
    // -------------------------------------------------------------------
    connect(this->armState.get(), &ArmState::onUpdated, 
            this, &UserInterface::updateArmState);

    connect(this->armState.get(), &ArmState::onLoadedJointData, 
            this, &UserInterface::setJointData);

    // -------------------------------------------------------------------
    // Widgets
    // -------------------------------------------------------------------
    // Container widget
    setWindowTitle("C38 Robot Controller");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setMinimumSize(800, 700);

    // Main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // -------------------------------------------------------------------
    // Left Layout: Tab Bar and StackedWidget
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Tab Bar
    tabBar = new QTabBar(this);
    tabBar->addTab("Main Controls");
    tabBar->addTab("Driver");
    tabBar->addTab("Testing");
    connect(tabBar, &QTabBar::currentChanged, this, &UserInterface::onTabChanged);
    leftLayout->addWidget(tabBar);

    // StackedWidget
    stackedWidget = new QStackedWidget(this);
    leftLayout->addWidget(stackedWidget);

    // Add left layout to the main layout
    mainLayout->addLayout(leftLayout, 1);

    // -------------------------------------------------------------------
    // Main Controls section
    QWidget *mainControlsWidget = new QWidget();
    QVBoxLayout *mainControlsLayout = new QVBoxLayout(mainControlsWidget);
    mainControlsLayout->setAlignment(Qt::AlignTop);

    QLabel *serialPortLabel = new QLabel("Serial Port:", this);
    serialPortComboBox = new QComboBox(this);
    serialPortComboBox->addItem("Select Serial Port");
    connect(serialPortComboBox, &QComboBox::currentIndexChanged, this, &UserInterface::onSerialPortSelected);
    serialPortUpdateButton = new QPushButton("Update Serial Ports", this);
    connect(serialPortUpdateButton, &QPushButton::clicked, this, &UserInterface::onSerialPortUpdateRequested);
    connect(&RuntimeSettings::instance(), &RuntimeSettings::debugModeChanged, this, &UserInterface::setDebugMode);
    mainControlsLayout->addWidget(serialPortLabel);
    mainControlsLayout->addWidget(serialPortComboBox);
    mainControlsLayout->addWidget(serialPortUpdateButton);


    // Homing Command
    QVBoxLayout *homingLayout = new QVBoxLayout();
    QLabel *homingLabel = new QLabel("Homing Command", this);
    homingComboBox = new QComboBox(this);
    homingComboBox->addItem("All");
    homingComboBox->addItem("Axis 0");
    homingComboBox->addItem("Axis 1");
    homingComboBox->addItem("Axis 2");
    homingComboBox->addItem("Axis 3");
    homingComboBox->addItem("Axis 4");
    homingComboBox->addItem("Axis 5");

    homingLayout->addWidget(homingLabel);
    homingLayout->addWidget(homingComboBox);

    homingSequenceButton = new QPushButton("Run Homing Sequence", this);
    homingLayout->addWidget(homingSequenceButton);
    connect(homingSequenceButton, &QPushButton::clicked, this, &UserInterface::onHomingSequenceRequested);

    mainControlsLayout->addLayout(homingLayout);
    stackedWidget->addWidget(mainControlsWidget);

    // Add Emergency Stop Button
    emergencyStopButton = new QPushButton("Emergency Stop", this);
    mainControlsLayout->addWidget(emergencyStopButton);
    connect(emergencyStopButton, &QPushButton::clicked, this, &UserInterface::onEmergencyStopRequested);


    // -------------------------------------------------------------------
    // Processor Section
    QWidget *driverWidget = new QWidget();
    QVBoxLayout *driverLayout = new QVBoxLayout(driverWidget);
    driverLayout->setAlignment(Qt::AlignTop);

    QLabel *driverLabel = new QLabel("Driver selection", this);
    driverComboBox = new QComboBox(this);
    driverComboBox->addItem("Hand tracking driver");
    driverComboBox->addItem("Manual set via socket driver");
    driverLayout->addWidget(driverLabel);
    driverLayout->addWidget(driverComboBox);

    driverStartStopButton = new QPushButton("Start Driver", this);
    driverLayout->addWidget(driverStartStopButton);
    connect(driverStartStopButton, &QPushButton::clicked, this, &UserInterface::toggleDriverState);

    stackedWidget->addWidget(driverWidget);

    // -------------------------------------------------------------------
    // Test Controls section
    QWidget *testWidget = new QWidget();
    QVBoxLayout *testLayout = new QVBoxLayout(testWidget);
    testLayout->setAlignment(Qt::AlignTop);

    QLabel *testLabel = new QLabel("Test Selection", this);

    // Debug Mode Checkbox
    debugModeCheckBox = new QCheckBox("Enable Debug Mode", this);
    testLayout->addWidget(debugModeCheckBox);
    connect(debugModeCheckBox, &QCheckBox::toggled, [](bool enabled) {
        RuntimeSettings::instance().setDebugMode(enabled); });
    connect(&RuntimeSettings::instance(), &RuntimeSettings::debugModeChanged, 
        debugModeCheckBox, &QCheckBox::setChecked);

    testComboBox = new QComboBox(this);
    testComboBox->addItem("Axis 0");
    testComboBox->addItem("Axis 1");
    testComboBox->addItem("Axis 2");
    testComboBox->addItem("Axis 3");
    testComboBox->addItem("Axis 4");
    testComboBox->addItem("Axis 5");
    testComboBox->addItem("Jogging 1");
    testComboBox->addItem("Jogging 2");
    testLayout->addWidget(testLabel);
    testLayout->addWidget(testComboBox);

    testButton = new QPushButton("RunTest", this);
    testLayout->addWidget(testButton);
    connect(testButton, &QPushButton::clicked, this, &UserInterface::onTestRequested);

    stackedWidget->addWidget(testWidget);


    // -------------------------------------------------------------------
    // Right Layout: 3D Window + Sliders
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // 3D Window
    view = new Qt3DExtras::Qt3DWindow();
    viewContainer = QWidget::createWindowContainer(view, this);
    viewContainer->setMinimumSize(QSize(300, 200));
    viewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightLayout->addWidget(viewContainer, 5);

    // Proper FrameGraph Setup
    auto *frameGraphRoot = new Qt3DRender::QRenderSurfaceSelector();
    frameGraphRoot->setSurface(view);

    auto *clearBuffers = new Qt3DRender::QClearBuffers(frameGraphRoot);
    clearBuffers->setClearColor(QColor(200, 200, 200));
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    auto *renderSettings = new Qt3DRender::QRenderSettings(frameGraphRoot);

    view->setActiveFrameGraph(frameGraphRoot);
    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    // Camera setup
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 500.0f);
    camera->setPosition(QVector3D(0, 50, 100));
    camera->setViewCenter(QVector3D(0, 0, 0));
    auto *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(camera);


    // Add directional light
    auto *lightEntity = new Qt3DCore::QEntity(rootEntity);
    auto *light = new Qt3DRender::QDirectionalLight(lightEntity);
    light->setWorldDirection(QVector3D(-1.0f, -1.0f, -1.0f));
    light->setColor(QColor(255, 255, 255));
    light->setIntensity(1.0f);
    lightEntity->addComponent(light);

    // Create a ground plane
    auto *gridMesh = new Qt3DExtras::QPlaneMesh();
    gridMesh->setWidth(20.0f);
    gridMesh->setHeight(20.0f);
    gridMesh->setMeshResolution(QSize(20, 20));

    auto *gridTransform = new Qt3DCore::QTransform();
    gridTransform->setTranslation(QVector3D(0.0f, -0.1f, 0.0f));
    gridTransform->setRotation(QQuaternion::fromEulerAngles(90.0f, 0.0f, 0.0f));

    auto *gridMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    gridMaterial->setDiffuse(QColor(150, 150, 150));

    auto *gridEntity = new Qt3DCore::QEntity(rootEntity);
    gridEntity->addComponent(gridMesh);
    gridEntity->addComponent(gridMaterial);
    gridEntity->addComponent(gridTransform);

    // Sliders
    QWidget* slidersContainer = new QWidget(this);
    slidersContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    sliderLayout = new QVBoxLayout(slidersContainer);
    sliderLayout->setContentsMargins(10, 10, 10, 10);
    sliderLayout->setSpacing(10);

    rightLayout->addWidget(slidersContainer, 2);
    mainLayout->addLayout(rightLayout, 2);

    rightLayout->addLayout(sliderLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    // Update JointData after object initialization is complete
    QMetaObject::invokeMethod(this, [this]() {
        this->setJointData(this->armState->getAllJointData());
    }, Qt::QueuedConnection);
}


void UserInterface::onTabChanged(int index)
{
    if (index >= 0 && index < stackedWidget->count())
    {
        stackedWidget->setCurrentIndex(index);
    }
}

void UserInterface::setDebugMode(const bool enabled)
{
    qDebug() << "UserInterface: Debug mode set to:" << enabled;
}

void UserInterface::onSerialPortSelected(int index)
{
    if (index == 0)
    {
        qDebug() << "UserInterface: No serial port selected.";
        return;
    }

    QString selectedPort = serialPortComboBox->currentText();
    qDebug() << "UserInterface: Selected serial port:" << selectedPort;

    if (serialInterface)
    {
        bool success = serialInterface->openSerialPort(selectedPort.toStdString());
        if (success)
        {
            qDebug() << "UserInterface: Successfully opened serial port.";
        }
        else
        {
            qDebug() << "UserInterface: Failed to open serial port.";
        }
    }
}

// -------------------------------------------------------------------
// Commands
// -------------------------------------------------------------------

void UserInterface::setDriverState(const bool isActive)
{
    // Update UI with driver state elements
}

void UserInterface::toggleDriverState()
{
    // Toggle Driver State
}

void UserInterface::onSelectAxisAngle(int axis, float angle)
{
    auto command = SetAxisAngleCommand(axis, angle);
    if (serialInterface)
    {
        serialInterface->sendCommand(command);
    }
    else
    {
        qDebug() << "Request Command failed: Serial interface not available.";
    }
}

void UserInterface::onHomingSequenceRequested()
{
    int selectedHomingMode = homingComboBox->currentIndex() - 1; // -1 for "All" option
    auto command = HomingSequenceCommand(selectedHomingMode);
    if (serialInterface)
    {
        serialInterface->sendCommand(command);
    }
    else
    {
        qDebug() << "Request Command failed: Serial interface not available.";
    }
}

void UserInterface::onEmergencyStopRequested()
{
    auto command = EmergencyStopCommand();
    if (serialInterface)
    {
        serialInterface->sendCommand(command);
    }
    else
    {
        qDebug() << "Request Command failed: Serial interface not available.";
    }
}

void UserInterface::onTestRequested()
{
    int selectedTest = testComboBox->currentIndex();
    auto command = RunTestCommand(selectedTest);
    if (serialInterface)
    {
        serialInterface->sendCommand(command);
    }
    else
    {
        qDebug() << "Request Command failed: Serial interface not available.";
    }
}

void UserInterface::driverSelected(int index)
{
    selectedDriver = index;
}

int UserInterface::getSelectedDriver() const 
{
    return selectedDriver;
}

QString UserInterface::getSelectedSerialPort() const
{
    return serialPortComboBox->currentText();
}

void UserInterface::onSerialPortUpdateRequested()
{
    if (serialInterface != nullptr)
    {
        serialInterface->updateAvailableSerialPorts();
    }
}

void UserInterface::updateAvailableSerialPorts(const QStringList& ports)
{
    serialPortComboBox->clear();
    serialPortComboBox->addItem("Select Serial Port");
    serialPortComboBox->addItems(ports);
}

void UserInterface::updateArmState(const std::vector<JointData>& inJointData)
{
    for (int i = 0; i < axisSliders.size() && i < inJointData.size(); ++i)
    {
        // Update slider position
        axisSliders[i]->setValue(static_cast<int>(inJointData[i].currentAngle));

        // Update value label
        axisValueLabels[i]->setText(QString::number(inJointData[i].currentAngle, 'f', 1) + "°");
    }
    update3DRender(inJointData);
}

void UserInterface::setJointData(const std::vector<JointData>& jointData)
{
    qDebug() << "setJointData called. Joint count =" << jointData.size();
    // Clear old entities
    for (auto* e : jointEntities)
    {
        if (e) e->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
        delete e;
    }
    jointEntities.clear();
    jointTransforms.clear();

    // Clear old sliders and labels
    QLayoutItem* item;
    while ((item = sliderLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
    axisSliders.clear();
    axisValueLabels.clear();

    QVector3D startPosition(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < (int)jointData.size(); ++i)
    {
        const auto& joint = jointData[i];
        // Create horizontal layout for each joint
        QHBoxLayout* jointLayout = new QHBoxLayout();
        jointLayout->setSpacing(10);

        // Name label
        QLabel* nameLabel = new QLabel(QString("Joint %1").arg(i), this);
        nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        jointLayout->addWidget(nameLabel);

        // Angle display
        QLineEdit* angleDisplay = new QLineEdit(QString::number(joint.currentAngle, 'f', 3), this);
        angleDisplay->setReadOnly(true);
        angleDisplay->setMaximumWidth(60); // Fixed width for angle display
        angleDisplay->setAlignment(Qt::AlignRight);
        jointLayout->addWidget(angleDisplay);
        axisValueLabels.push_back(angleDisplay);

        // Slider
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(static_cast<int>(joint.minAngle), static_cast<int>(joint.maxAngle));
        slider->setValue(static_cast<int>(joint.currentAngle));
        slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Slider scales dynamically
        jointLayout->addWidget(slider);
        axisSliders.push_back(slider);

        connect(slider, &QSlider::valueChanged, [this, angleDisplay, i](int val) {
            angleDisplay->setText(QString::number(val, 'f', 3));
            onSelectAxisAngle(i, static_cast<float>(val));
        });

        sliderLayout->addLayout(jointLayout);

        // Create joint entity
        auto* jointEntity = new Qt3DCore::QEntity(rootEntity);
        auto* jointMesh = new Qt3DExtras::QCylinderMesh();
        jointMesh->setRadius(0.2f);
        jointMesh->setLength(joint.dHParameters.a);

        auto* jointMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
        jointMaterial->setDiffuse(Qt::blue);

        auto* jointTransform = new Qt3DCore::QTransform();
        jointTransform->setTranslation(startPosition);

        jointEntity->addComponent(jointMesh);
        jointEntity->addComponent(jointMaterial);
        jointEntity->addComponent(jointTransform);

        jointEntities.push_back(jointEntity);
        jointTransforms.push_back(jointTransform);

        // Update start position for next joint
        startPosition.setX(startPosition.x() + joint.dHParameters.a);
    }

    QMetaObject::invokeMethod(this, [this, jointData]() {
        update3DRender(jointData);
    }, Qt::QueuedConnection);
}

void UserInterface::update3DRender(const std::vector<JointData>& jointData)
{
    qDebug() << "Requested update3DRender";
    if ((int)jointData.size() != jointEntities.size())
        qDebug() << "update3DRender: size mismatch. jointData.size()=";
        return;

    QVector3D currentPosition(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < (int)jointData.size(); ++i)
    {
        const auto &joint = jointData[i];
        auto *transform = jointTransforms[i];

        // Calculate the new position based on DH parameters
        currentPosition.setX(currentPosition.x() + joint.dHParameters.a * std::cos(joint.dHParameters.theta * M_PI / 180.0f));
        currentPosition.setZ(currentPosition.z() + joint.dHParameters.a * std::sin(joint.dHParameters.theta * M_PI / 180.0f));
        currentPosition.setY(currentPosition.y() + joint.dHParameters.d);

        // Update the transform
        transform->setTranslation(currentPosition);
        transform->setRotation(QQuaternion::fromEulerAngles(
            joint.dHParameters.alpha,
            joint.dHParameters.theta,
            0.0f
        ));
    }
}
