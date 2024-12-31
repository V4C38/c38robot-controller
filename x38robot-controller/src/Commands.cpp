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
    json["type"] = "command"; 
    json["uuid"] = uuid.toString();
    json["command"] = commandType;
    return json;
} 

QJsonObject Command::responseFormat() const
{
    QJsonObject response;
    response["type"] = "response";
    response["uuid"] = uuid.toString();
    response["command"] = commandType;
    response["status"] = "success";
    // Keep "message" as an optional field, but do not set default values
    response["message"] = QJsonValue(); 
    return response;
}

QString Command::toString() const
{
    QJsonDocument doc(toJson());
    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonObject Command::createStateUpdate(const QMap<int, float>& updatedAxes) const
{
    QJsonObject stateUpdate;
    QJsonObject axes;

    for (auto it = updatedAxes.begin(); it != updatedAxes.end(); ++it)
    {
        axes[QString::number(it.key())] = it.value();
    }

    stateUpdate["axes"] = axes;
    return stateUpdate;
}

// HomingSequenceCommand implementation
HomingSequenceCommand::HomingSequenceCommand(int axis)
    : Command("homingSequence"), axis(axis) {}

QJsonObject HomingSequenceCommand::toJson() const
{
    QJsonObject json = Command::toJson();
    json["axis"] = axis;
    return json;
}

QJsonObject HomingSequenceCommand::responseFormat() const
{
    QJsonObject response = Command::responseFormat();
    response["message"] = QString("Homing sequence executed for axis %1.").arg(axis);
    return response;
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

QJsonObject SetAxisAngleCommand::responseFormat() const
{
    QJsonObject response = Command::responseFormat();
    QMap<int, float> updatedAxes;
    updatedAxes[axis] = angle;
    response["stateUpdate"] = createStateUpdate(updatedAxes);
    return response;
}

// GetStateCommand implementation
GetStateCommand::GetStateCommand() : Command("getState") {}

QJsonObject GetStateCommand::toJson() const
{
    return Command::toJson();
}

QJsonObject GetStateCommand::responseFormat() const
{
    // Keep stateUpdate empty; actual data is expected from the microcontroller
    QJsonObject response = Command::responseFormat();
    response["stateUpdate"] = QJsonObject(); 
    return response;
}

// EmergencyStopCommand implementation
EmergencyStopCommand::EmergencyStopCommand() : Command("emergencyStop") {}

QJsonObject EmergencyStopCommand::toJson() const
{
    return Command::toJson();
}

QJsonObject EmergencyStopCommand::responseFormat() const
{
    QJsonObject response = Command::responseFormat();
    response["message"] = "Emergency stop.";
    return response;
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

QJsonObject SetArmStateCommand::responseFormat() const
{
    QJsonObject response = Command::responseFormat();
    return response;
}

// RunTestCommand implementation
RunTestCommand::RunTestCommand(int testIndex)
    : Command("runTest"), testIndex(testIndex) {}

QJsonObject RunTestCommand::toJson() const
{
    QJsonObject json = Command::toJson();
    json["testIndex"] = testIndex;
    return json;
}

QJsonObject RunTestCommand::responseFormat() const
{
    QJsonObject response = Command::responseFormat();
    return response;
}

// -------------------------------------------------------------------
// Factory
// -------------------------------------------------------------------
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
        int axis = json["axis"].toInt();
        return std::make_unique<HomingSequenceCommand>(axis);
    }
    else if (command == "setAxisAngle")
    {
        int axis = json["axis"].toInt();
        float angle = json["angle"].toDouble();
        return std::make_unique<SetAxisAngleCommand>(axis, angle);
    }
    else if (command == "runTest")
    {
        int testIndex = json["testIndex"].toInt();
        return std::make_unique<RunTestCommand>(testIndex);
    }
    else
    {
        qDebug() << "Unsupported command:" << command << QJsonDocument(json).toJson();
    }

    return nullptr;
}
