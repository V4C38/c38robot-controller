#include "Commands.h"
#include "ArmState.h"

#include <QJsonDocument>
#include <stdexcept>

// Command base class implementation
Command::Command(const QString& commandType)
    : uuid(QUuid::createUuid()), commandType(commandType) {}

QJsonObject Command::toJson() const
{
    QJsonObject json;
    json["uuid"] = uuid.toString();
    json["command"] = commandType;
    return json;
}

QJsonObject Command::generateDebugResponse() const
{
    QJsonObject json;
    json["debugMode"] = true;
    json["uuid"] = getUuid().toString();
    json["command"] = getCommandType();
    json["message"] = QString("Simulated response for command '%1'").arg(getCommandType());
    return json;
}

QString Command::toString() const
{
    QJsonDocument doc(toJson());
    return QString(doc.toJson(QJsonDocument::Compact));
}

// GetStateCommand implementation
GetStateCommand::GetStateCommand() : Command("getState") {}

QJsonObject GetStateCommand::toJson() const
{
    return Command::toJson(); // No additional fields
}

// EmergencyStopCommand implementation
EmergencyStopCommand::EmergencyStopCommand() : Command("emergencyStop") {}

QJsonObject EmergencyStopCommand::toJson() const
{
    return Command::toJson(); // No additional fields
}

// HomingSequenceCommand implementation
HomingSequenceCommand::HomingSequenceCommand(int axis)
    : Command("homingSequence"), axis(axis) {}

QJsonObject HomingSequenceCommand::toJson() const
{
    QJsonObject json = Command::toJson();
    if (axis != -1)
    {
        json["axis"] = axis;
    }
    return json;
}

// SetAxisAngleCommand implementation
SetAxisAngleCommand::SetAxisAngleCommand(int axis, float angle)
    : Command("setAxisAngle"), axis(axis), angle(angle) {}

QJsonObject SetAxisAngleCommand::toJson() const
{
    QJsonObject json = Command::toJson();
    json["axis"] = axis;
    json["angle"] = angle;
    return json;
}

// SetArmStateCommand implementation
SetArmStateCommand::SetArmStateCommand(const ArmState* armState)
    : Command("setArmState"), armState(armState)
{
    if (!armState)
    {
        throw std::invalid_argument("ArmState pointer cannot be null");
    }
}

QJsonObject SetArmStateCommand::toJson() const
{
    QJsonObject json = Command::toJson();
    if (armState)
    {
        json["armState"] = armState->toJson();
    }
    return json;
}

// Factory method implementation
std::unique_ptr<Command> createCommand(const QJsonObject& json)
{
    if (!json.contains("command"))
    {
        qDebug() << "Invalid command JSON: Missing 'command' key" << QJsonDocument(json).toJson(QJsonDocument::Compact);
        return nullptr;
    }

    QString command = json["command"].toString();
    if (command == "getState")
    {
        return std::make_unique<GetStateCommand>();
    }
    else if (command == "emergencyStop")
    {
        return std::make_unique<EmergencyStopCommand>();
    }
    else if (command == "homingSequence")
    {
        int axis = json.contains("axis") ? json["axis"].toInt() : -1;
        return std::make_unique<HomingSequenceCommand>(axis);
    }
    else if (command == "setAxisAngle")
    {
        if (json.contains("axis") && json.contains("angle"))
        {
            return std::make_unique<SetAxisAngleCommand>(json["axis"].toInt(), json["angle"].toDouble());
        }
        else
        {
            qDebug() << "Invalid 'setAxisAngle' JSON: Missing 'axis' or 'angle'" << QJsonDocument(json).toJson();
        }
    }
    else
    {
        qDebug() << "Unsupported command:" << command << QJsonDocument(json).toJson();
    }

    return nullptr;
}
