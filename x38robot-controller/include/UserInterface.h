#ifndef USERINTERFACE_H
#define USERINTERFACE_H

// Override the definitions of slots and signals in Python (Pybind11)
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

    int getSelectedProcessor() const;
    void setIsProcessing(bool InIsProcessing);

signals:
    void homingSequenceRequested();
    void toggleIsProcessing();

private slots:
    // Slots (Button Interactions)
    void onToggleIsProcessing();
    void onHomingSequenceRequested();
    void onProcessorSelected(int index);

private:
    int selectedProcessor = 0;

    QVBoxLayout* mainLayout;

    // Title
    QLabel* titleLabel;

    QPushButton* homingSequenceButton;

    // Dropdown
    QVBoxLayout* processorLayout;
    QLabel* processorLabel;
    QComboBox* processorComboBox;

    // Start/Stop Button
    QPushButton* startStopButton;

};

#undef slots
#undef signals

#endif // USERINTERFACE_H