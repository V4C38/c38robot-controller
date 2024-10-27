#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

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

private slots:
    // Slots (Button Interactions)
    void onToggleIsProcessing();
    void onEmergencyStopRequested();
    void onHomingSequenceRequested();
    void onTestRequested();
    void onProcessorSelected(int index);

private:
    int selectedProcessor = 0;

    QVBoxLayout* mainLayout;

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

    // Serial Port Dropdown
    QPushButton* emergencyStopButton;

};

#undef slots
#undef signals

#endif // USERINTERFACE_H
