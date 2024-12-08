#ifndef emit
#define emit
#endif

#include "UserInterface.h"

QComboBox* UserInterface::getSerialPortComboBox() {
    return serialPortComboBox;
}

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent) {

    // Create and configure central/container widget
    setWindowTitle("C38 Robot Controller");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setMinimumSize(400, 300);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Title
    titleLabel = new QLabel("C38 Robot Controller", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Serial port selector (dropdown at the top)
    QLabel *serialPortLabel = new QLabel("Serial Port:", this);
    serialPortComboBox = new QComboBox(this);
    serialPortComboBox->addItem("Select Serial Port");  // Placeholder for serial port selection
    mainLayout->addWidget(serialPortLabel);
    mainLayout->addWidget(serialPortComboBox);

    // Processor selection dropdown (Renamed from End-effector manipulation method to Processor)
    QHBoxLayout *processorLayout = new QHBoxLayout();
    QLabel *processorLabel = new QLabel("Processor:", this);
    processorComboBox = new QComboBox(this);
    processorComboBox->addItem("Hand tracking image processor");
    processorComboBox->addItem("Manual set via socket processor");
    processorLayout->addWidget(processorLabel);
    processorLayout->addWidget(processorComboBox);
    mainLayout->addLayout(processorLayout);

    connect(processorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserInterface::onProcessorSelected);

    // Start/Stop button 
    startStopButton = new QPushButton("Start Processing", this);
    mainLayout->addWidget(startStopButton);
    connect(startStopButton, &QPushButton::clicked, this, &UserInterface::onToggleIsProcessing);


    // Spacer above Homing and Test Commands
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addSpacerItem(spacer);

    // Homing and Test Command Section Title
    QLabel *commandTitle = new QLabel("Homing and Test Commands", this);
    commandTitle->setAlignment(Qt::AlignCenter);
    commandTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(commandTitle);

    // Emergency Stop button
    emergencyStopButton = new QPushButton("Emergency Stop", this);
    mainLayout->addWidget(emergencyStopButton);
    connect(emergencyStopButton, &QPushButton::clicked, this, &UserInterface::onEmergencyStopRequested);

    // Homing and Test button side by side with dropdowns
    QHBoxLayout *homingTestLayout = new QHBoxLayout();

    // Homing Dropdown + Button
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

    homingSequenceButton = new QPushButton("RunHomingSequence", this);
    homingLayout->addWidget(homingSequenceButton);
    connect(homingSequenceButton, &QPushButton::clicked, this, &UserInterface::onHomingSequenceRequested);
    homingTestLayout->addLayout(homingLayout);

    // Test Dropdown + Button
    QVBoxLayout *testLayout = new QVBoxLayout();
    QLabel *testLabel = new QLabel("Test Command", this);
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
    homingTestLayout->addLayout(testLayout);

    mainLayout->addLayout(homingTestLayout);
}

void UserInterface::onToggleIsProcessing()
{
    emit toggleIsProcessing();
}

void UserInterface::onHomingSequenceRequested()
{
    int selectedHomingMode = homingComboBox->currentIndex();  // Get the selected homing mode
    int homingIndex = -1;  // Default to -1 for "All"
    if (selectedHomingMode > 0) {
        homingIndex = selectedHomingMode - 1; 
        }

    emit homingSequenceRequested(homingIndex);
}


void UserInterface::onEmergencyStopRequested()
{
    emit emergencyStopRequested();
}

void UserInterface::onTestRequested()
{
    int selectedTest = testComboBox->currentIndex();  // Get the selected test mode
    emit testRequested(selectedTest);  // Pass the test mode to the signal
}

void UserInterface::onProcessorSelected(int index)
{
    selectedProcessor = index;
}

int UserInterface::getSelectedProcessor() const 
{
    return selectedProcessor;
}

QString UserInterface::getSelectedSerialPort() const
{
    return serialPortComboBox->currentText();  // Return selected serial port
}

void UserInterface::setIsProcessing(bool InIsProcessing)
{
    if (InIsProcessing)
    {
        startStopButton->setText("Stop Processing");
        homingSequenceButton->setEnabled(false);
        processorComboBox->setEnabled(false);
        testButton->setEnabled(false);
        homingComboBox->setEnabled(false);
        testComboBox->setEnabled(false);
        serialPortComboBox->setEnabled(false);
    }
    else
    {
        startStopButton->setText("Start Processing");
        homingSequenceButton->setEnabled(true);
        processorComboBox->setEnabled(true);
        testButton->setEnabled(true);
        homingComboBox->setEnabled(true);
        testComboBox->setEnabled(true);
        serialPortComboBox->setEnabled(true);
    }
}
