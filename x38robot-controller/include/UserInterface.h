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
    void testRequested(int selectedTestMode);
    void toggleIsProcessing();

private slots:
    // Slots (Button Interactions)
    void onToggleIsProcessing();
    void onHomingSequenceRequested();
    void onTestRequested();
    void onProcessorSelected(int index);

private:
    int selectedProcessor = 0;

    QVBoxLayout* mainLayout;

    // Title
    QLabel* titleLabel;

    QPushButton* homingSequenceButton;
    QPushButton* testButton;

    // Dropdowns
    QVBoxLayout* processorLayout;
    QLabel* processorLabel;
    QComboBox* processorComboBox;

    // Homing Dropdown
    QComboBox* homingComboBox;

    // Test Dropdown
    QComboBox* testComboBox;

    // Serial Port Dropdown
    QComboBox* serialPortComboBox;

    // Start/Stop Button
    QPushButton* startStopButton;
};

#undef slots
#undef signals

#endif // USERINTERFACE_H
