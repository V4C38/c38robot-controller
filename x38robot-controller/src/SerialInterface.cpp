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
    if (serialPort && serialPort->isOpen())
    {
        qDebug() << "SerialInterface: Closing currently open port before opening a new one.";
        closeSerialPort();
    }

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
    // Reset serialPort only if it is fully closed
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
        qDebug() << "SerialInterface: Serial port is not open. Cannot read message.";
        return;
    }

    // Append any newly received data to our buffer
    QByteArray newData = serialPort->readAll();
    serialBuffer.append(newData);

    // Attempt to split the buffer by newline characters
    while (true)
    {
        int newlineIndex = serialBuffer.indexOf('\n');
        if (newlineIndex < 0)
        {
            // No complete line yet; wait for more data
            break;
        }

        // Extract one line (without the newline) from the buffer
        QByteArray line = serialBuffer.left(newlineIndex);
        // Remove this line + newline from the buffer
        serialBuffer.remove(0, newlineIndex + 1);

        // Ignore empty or whitespace-only lines
        if (line.trimmed().isEmpty())
            continue;

        // Debug info (raw line)
        qDebug() << "SerialInterface: Complete line received:" << line;

        // Try to parse the line as JSON
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError)
        {
            qDebug() << "SerialInterface: Failed to parse message:" << parseError.errorString();
            continue;
        }

        // If successfully parsed, process it as a JSON object
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
            qDebug() << "SerialInterface: Response missing UUID.";
            return;
        }

        QUuid responseUuid = QUuid::fromString(message["uuid"].toString());
        if (responseUuid.isNull())
        {
            qDebug() << "SerialInterface: Invalid or empty UUID in response.";
            return;
        }

        auto it = pendingCommands.find(responseUuid);
        if (it != pendingCommands.end())
        {
            QString commandType = it->second;
            pendingCommands.erase(it);
            qDebug() << "SerialInterface: Matched response for command:" << commandType;

            if (message.contains("stateUpdate") && message["stateUpdate"].isObject())
            {
                armState->updateFromJson(message["stateUpdate"].toObject());
                qDebug() << "SerialInterface: Updated ArmState with response.";
            }
        }
        else
        {
            qDebug() << "SerialInterface: Unknown or unmatched response UUID:" << responseUuid;
        }

        if (message.contains("status"))
        {
            QString status = message["status"].toString();
            qDebug() << "SerialInterface: Response status:" << status;
        }

        if (message.contains("message"))
        {
            QString responseMessage = message["message"].toString();
            qDebug() << "SerialInterface: Response message:" << responseMessage;
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
