#ifndef emit
#define emit
#endif

#include "UserInterface.h"

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent){

    // Create and configure central/container widget
    setWindowTitle("C38 Robot Controller");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setMinimumSize(300, 200);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Title
    titleLabel = new QLabel("C38 Robot Controller", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);


    // Processor selection dropdown
    QHBoxLayout *processorLayout = new QHBoxLayout();
    QLabel *processorLabel = new QLabel("End-effector manipulation method:", this);
    processorComboBox = new QComboBox(this);
    processorComboBox->addItem("Hand tracking image processor");
    processorComboBox->addItem("Manual set via socket processor");
    processorLayout->addWidget(processorLabel);
    processorLayout->addWidget(processorComboBox);
    mainLayout->addLayout(processorLayout);  
    
    connect(processorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserInterface::onProcessorSelected);
    
    // Initiate Homing Sequence Button
    homingSequenceButton = new QPushButton("Run homing sequence", this);
    mainLayout->addWidget(homingSequenceButton);

    connect(homingSequenceButton, &QPushButton::clicked, this, &UserInterface::onHomingSequenceRequested);

    // Start/Stop button
    startStopButton = new QPushButton("Start Processing", this);
    mainLayout->addWidget(startStopButton);

    connect(startStopButton, &QPushButton::clicked, this, &UserInterface::onToggleIsProcessing);
}

void UserInterface::onToggleIsProcessing()
{
    emit toggleIsProcessing();
}

void UserInterface::onHomingSequenceRequested()
{
    emit homingSequenceRequested();
}

void UserInterface::onProcessorSelected(int index)
{
    selectedProcessor = index;
}

int UserInterface::getSelectedProcessor() const 
{
    return selectedProcessor;
}

void UserInterface::setIsProcessing(bool InIsProcessing)
{
    if (InIsProcessing)
    {
        startStopButton->setText("Stop Processing");
        homingSequenceButton->setEnabled(false);
        processorComboBox->setEnabled(false);
    }
    else
    {
        startStopButton->setText("Start Processing");
        homingSequenceButton->setEnabled(true);
        processorComboBox->setEnabled(true);
    }
}