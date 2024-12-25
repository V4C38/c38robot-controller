#include <SerialInterface.h>

SerialInterface::SerialInterface(std::shared_ptr<ArmState> armState)
    : armState(std::move(armState))
{
    if (!this->armState)
    {
        throw std::invalid_argument("ArmState shared_ptr cannot be null");
    }

    // Initialize the serial port
    serialPort = std::make_unique<QSerialPort>();

    // Connect readyRead signal to a slot for reading responses
    connect(serialPort.get(), &QSerialPort::readyRead, this, &SerialInterface::readResponse);

    // Handle serial port errors
    connect(serialPort.get(), &QSerialPort::errorOccurred, this, [](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError) {
            qDebug() << "Serial port error:" << error;
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
        qDebug() << "Failed to open serial port:" << portName.c_str();
        return false;
    }

    qDebug() << "Serial port opened successfully:" << portName.c_str();
    return true;
}

void SerialInterface::closeSerialPort()
{
    if (serialPort && serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "Serial port closed.";
    }
    serialPort.reset();
}

void SerialInterface::sendCommand(const Command& command)
{
    if (isDebugMode.load())
    {
        QJsonObject debugResponse = command.generateDebugResponse();
        emit OnResponseReceived(debugResponse);
        return;
    }

    QJsonObject jsonCommand = command.toJson();
    pendingCommands[command.getUuid()] = command.getCommandType(); // Track pending command
    sendCommand(jsonCommand);
}

void SerialInterface::sendCommand(const QJsonObject& command)
{
    if (!serialPort || !serialPort->isOpen())
    {
        qDebug() << "Serial port is not open. Cannot send command.";
        return;
    }

    QJsonDocument doc(command);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n'; // Add delimiter for parsing

    qint64 bytesWritten = serialPort->write(data);
    if (bytesWritten == -1)
    {
        qDebug() << "Failed to write to serial port.";
    }
    else
    {
        qDebug() << "Command sent via serial:" << command;
        emit OnCommandSent(command);
    }
}

void SerialInterface::readResponse()
{
    if (!serialPort || !serialPort->isOpen())
    {
        qDebug() << "Serial port is not open. Cannot read response.";
        return;
    }

    QByteArray responseData = serialPort->readAll();
    QList<QByteArray> messages = responseData.split('\n'); // Split concatenated responses

    for (const QByteArray& message : messages)
    {
        if (message.trimmed().isEmpty())
            continue;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message, &parseError);

        if (parseError.error != QJsonParseError::NoError)
        {
            qDebug() << "Failed to parse response:" << parseError.errorString();
            continue;
        }

        QJsonObject response = jsonDoc.object();
        qDebug() << "Response received:" << response;

        emit OnResponseReceived(response);
        processResponse(response);
    }
}

void SerialInterface::processResponse(const QJsonObject& response)
{
    // Check if the response contains a UUID
    if (!response.contains("uuid"))
    {
        qDebug() << "Response missing UUID:" << response;
        return;
    }

    // Extract the UUID from the response
    QUuid responseUuid = QUuid::fromString(response["uuid"].toString());

    // Check if the UUID matches a pending command
    auto it = pendingCommands.find(responseUuid);
    if (it != pendingCommands.end()) // If the UUID is found
    {
        QString commandType = it->second; // Retrieve the command type
        pendingCommands.erase(it); // Remove processed command
        qDebug() << "Matched response for command:" << commandType;
    }
    else
    {
        qDebug() << "Unknown or unmatched response UUID:" << responseUuid;
    }

    // Handle state update if present in the response
    if (response.contains("stateUpdate") && response["stateUpdate"].isObject())
    {
        armState->updateFromJson(response["stateUpdate"].toObject());
        qDebug() << "Updated ArmState from response.";
    }
    else if (!response.contains("stateUpdate"))
    {
        qDebug() << "No state update found in response:" << response;
    }
    else
    {
        qDebug() << "Invalid state update format in response:" << response;
    }
}


void SerialInterface::setDebugMode(bool enabled)
{
    isDebugMode.store(enabled);
    qDebug() << "Debug mode set to:" << enabled;
}
