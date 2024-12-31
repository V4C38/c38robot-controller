#include <SerialInterface.h>

SerialInterface::SerialInterface(std::shared_ptr<ArmState> armState)
    : armState(std::move(armState))
{
    if (!this->armState)
    {
        throw std::invalid_argument("SerialInterface: ArmState shared_ptr cannot be null");
    }

    serialPort = std::make_unique<QSerialPort>();
    QMetaObject::invokeMethod(this, [this]() {
        this->updateAvailableSerialPorts();
    }, Qt::QueuedConnection);

    connect(serialPort.get(), &QSerialPort::readyRead, this, &SerialInterface::readMessage);

    connect(serialPort.get(), &QSerialPort::errorOccurred, this, [](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError)
        {
            qDebug() << "SerialInterface: Serial port error:" << error;
        }
    });
}

bool SerialInterface::openSerialPort(const std::string& portName)
{
    serialPort->setPortName(QString::fromStdString(portName));
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite))
    {
        qDebug() << "SerialInterface: Failed to open serial port:" << QString::fromStdString(portName);
        return false;
    }

    qDebug() << "SerialInterface: Serial port opened successfully:" << QString::fromStdString(portName);
    return true;
}

void SerialInterface::closeSerialPort()
{
    if (serialPort && serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "SerialInterface: Serial port closed.";
    }
    serialPort.reset();
}

void SerialInterface::sendCommand(const Command& command)
{
    if (RuntimeSettings::instance().isDebugMode())
    {
        QJsonObject debugResponse = command.responseFormat();
        pendingCommands[command.getUuid()] = command.getCommandType();

        qDebug().noquote() << "SerialInterface: Simulated debug response for command:" << command.getCommandType();
        emit onMessageReceived(debugResponse);
        processMessage(debugResponse);
        return;
    }

    QJsonObject jsonCommand = command.toJson();
    pendingCommands[command.getUuid()] = command.getCommandType();
    sendCommand(jsonCommand);
}

void SerialInterface::sendCommand(const QJsonObject& command)
{
    if (!serialPort || !serialPort->isOpen())
    {
        qDebug() << "SerialInterface: Serial port is not open. Cannot send command.";
        return;
    }

    QJsonDocument doc(command);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n';

    qint64 bytesWritten = serialPort->write(data);
    if (bytesWritten == -1)
    {
        qDebug() << "SerialInterface: Failed to write to serial port.";
    }
    else
    {
        qDebug() << "SerialInterface: Command sent via serial:" << command;
        emit onCommandSent(command);
    }
}

void SerialInterface::readMessage()
{
    if (!serialPort || !serialPort->isOpen())
    {
        qDebug() << "SerialInterface: Serial port is not open. Cannot read response.";
        return;
    }

    QByteArray responseData = serialPort->readAll();
    QList<QByteArray> messages = responseData.split('\n');

    for (const QByteArray& messageData : messages)
    {
        if (messageData.trimmed().isEmpty())
            continue;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(messageData, &parseError);

        if (parseError.error != QJsonParseError::NoError)
        {
            qDebug() << "SerialInterface: Failed to parse message:" << parseError.errorString();
            continue;
        }

        QJsonObject message = jsonDoc.object();
        emit onMessageReceived(message);
        processMessage(message);
    }
}

void SerialInterface::processMessage(const QJsonObject& message)
{
    if (!message.contains("type"))
    {
        qDebug() << "SerialInterface: Invalid message received, missing type.";
        return;
    }

    QString type = message["type"].toString();

    if (type == "response")
    {
        if (!message.contains("uuid"))
        {
            qDebug() << "SerialInterface: Invalid response, missing UUID.";
            return;
        }

        QUuid responseUuid = QUuid::fromString(message["uuid"].toString());
        auto it = pendingCommands.find(responseUuid);
        if (it != pendingCommands.end())
        {
            QString commandType = it->second;
            pendingCommands.erase(it);
            qDebug() << "SerialInterface: Matched response for command:" << commandType;

            if (message.contains("stateUpdate") && message["stateUpdate"].isObject())
            {
                armState->updateFromJson(message["stateUpdate"].toObject());
                // qDebug() << "SerialInterface: Updated ArmState with response.";
            }
        }
        else
        {
            qDebug() << "SerialInterface: Unknown or unmatched response UUID:" << responseUuid;
        }

        if (message.contains("status"))
        {
            QString status = message["status"].toString();
            // qDebug() << "SerialInterface: Response status:" << status;
        }

        if (message.contains("message"))
        {
            QString msg = message["message"].toString();
            if (!msg.isEmpty())
            {
                qDebug() << "SerialInterface: Response message:" << msg;
            }
        }
    }
    else if (type == "update")
    {
        if (message.contains("stateUpdate") && message["stateUpdate"].isObject())
        {
            armState->updateFromJson(message["stateUpdate"].toObject());
            // qDebug() << "SerialInterface: Received unsolicited state update.";
        }
    }
    else if (type == "error")
    {
        if (message.contains("message"))
        {
            QString errorMessage = message["message"].toString();
            qDebug() << "SerialInterface: Error received:" << errorMessage;
        }
    }
    else
    {
        qDebug() << "SerialInterface: Unknown message type:" << type;
    }
}

void SerialInterface::updateAvailableSerialPorts()
{
    QStringList ports;
    QDir devDir("/dev");
    QFileInfoList portList = devDir.entryInfoList(QStringList() << "cu.*", QDir::System | QDir::Readable);

    for (const QFileInfo& portInfo : portList)
    {
        ports.append(portInfo.absoluteFilePath());
    }

    if (ports.isEmpty())
    {
        ports.append("SerialInterface: No available serial ports");
        qDebug() << "SerialInterface: No available serial ports found.";
    }

    emit onAvailableSerialPortsUpdated(ports);
}
