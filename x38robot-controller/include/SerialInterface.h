#pragma once
#include <QObject>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUuid>
#include <QDir>
#include <QFileInfo>
#include <unordered_map>
#include <memory>
#include <ArmState.h>
#include <Commands.h>
#include <RuntimeSettings.h>

namespace std
{
    template<>
    struct hash<QUuid>
    {
        size_t operator()(const QUuid& uuid) const noexcept
        {
            return qHash(uuid);
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
    void onCommandSent(const QJsonObject& command);
    void onMessageReceived(const QJsonObject& response);
    void onAvailableSerialPortsUpdated(const QStringList& ports);

public slots:
    void updateAvailableSerialPorts();

private:
    std::unique_ptr<QSerialPort> serialPort = nullptr;
    std::shared_ptr<ArmState> armState = nullptr;

    // Command tracking
    std::unordered_map<QUuid, QString> pendingCommands;

    void readMessage();
    void processMessage(const QJsonObject& message);
};
