#pragma once
#include <QObject>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUuid>
#include <unordered_map>
#include <memory>

#include <ArmState.h>
#include <Commands.h>

// Make QUuid hashable
namespace std
{
    template<>
    struct hash<QUuid>
    {
        size_t operator()(const QUuid& uuid) const noexcept
        {
            return qHash(uuid); // Use Qt's qHash function
        }
    };
}

class SerialInterface : public QObject
{
    Q_OBJECT

public:
    explicit SerialInterface(std::shared_ptr<ArmState> armState);
    bool openSerialPort(const std::string& portName);
    void closeSerialPort();

    void sendCommand(const Command& command);
    void sendCommand(const QJsonObject& command);

signals:
    void OnCommandSent(const QJsonObject& command);
    void OnResponseReceived(const QJsonObject& response);

public slots:
    void setDebugMode(bool enabled);

private:
    std::unique_ptr<QSerialPort> serialPort;
    std::shared_ptr<ArmState> armState = nullptr;
    std::atomic<bool> isDebugMode{false};

    // Command tracking
    std::unordered_map<QUuid, QString> pendingCommands;

    void readResponse();
    void processResponse(const QJsonObject& response);
};
