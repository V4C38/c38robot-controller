#include "Commands.h"
#include <QJsonDocument>
#include <stdexcept>

// -------------------------------------------------------------------
// Command (base)
// -------------------------------------------------------------------
Command::Command(const QString& commandType)
    : uuid(QUuid::createUuid()), commandType(commandType)
{
}

QJsonObject Command::toJson() const
{
    // Base JSON includes type, UUID, and command
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
    return response;
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

// -------------------------------------------------------------------
// HomingSequenceCommand
// -------------------------------------------------------------------
HomingSequenceCommand::HomingSequenceCommand(int axis)
    : Command("homingSequence"), axis(axis)
{
}

QJsonObject HomingSequenceCommand::toJson() const
{
    QJsonObject baseJson = Command::toJson();

    // Wrap parameters in a subobject
    QJsonObject paramObj;
    paramObj["axis"] = axis;

    baseJson["parameters"] = paramObj;
    return baseJson;
}

QJsonObject HomingSequenceCommand::responseFormat() const
{
    QJsonObject resp = Command::responseFormat();
    resp["message"] = QString("Homing sequence executed for axis %1.").arg(axis);
    return resp;
}

// -------------------------------------------------------------------
// SetAxisAngleCommand
// -------------------------------------------------------------------
SetAxisAngleCommand::SetAxisAngleCommand(int axis, float angle)
    : Command("setAxisAngle"), axis(axis), angle(angle)
{
}

QJsonObject SetAxisAngleCommand::toJson() const
{
    QJsonObject baseJson = Command::toJson();

    // Wrap parameters
    QJsonObject paramObj;
    paramObj["axis"] = axis;
    paramObj["angle"] = angle;

    baseJson["parameters"] = paramObj;
    return baseJson;
}

QJsonObject SetAxisAngleCommand::responseFormat() const
{
    QJsonObject resp = Command::responseFormat();

    // Include a stateUpdate with the new angle
    QMap<int, float> updatedAxes;
    updatedAxes[axis] = angle;
    resp["stateUpdate"] = createStateUpdate(updatedAxes);
    return resp;
}

// -------------------------------------------------------------------
// GetStateCommand
// -------------------------------------------------------------------
GetStateCommand::GetStateCommand()
    : Command("getState")
{
}

QJsonObject GetStateCommand::toJson() const
{
    // No extra parameters
    return Command::toJson();
}

QJsonObject GetStateCommand::responseFormat() const
{
    QJsonObject resp = Command::responseFormat();
    // We'll rely on the microcontroller to fill stateUpdate
    return resp;
}

// -------------------------------------------------------------------
// EmergencyStopCommand
// -------------------------------------------------------------------
EmergencyStopCommand::EmergencyStopCommand()
    : Command("emergencyStop")
{
}

QJsonObject EmergencyStopCommand::toJson() const
{
    return Command::toJson(); // no parameters
}

QJsonObject EmergencyStopCommand::responseFormat() const
{
    QJsonObject resp = Command::responseFormat();
    resp["message"] = "Emergency stop.";
    return resp;
}

// -------------------------------------------------------------------
// SetArmStateCommand
// -------------------------------------------------------------------
#include "ArmState.h"

SetArmStateCommand::SetArmStateCommand(const ArmState* armState)
    : Command("setArmState"), armState(armState)
{
    if (!armState)
    {
        throw std::invalid_argument("SetArmStateCommand: ArmState pointer cannot be null");
    }
}

QJsonObject SetArmStateCommand::toJson() const
{
    QJsonObject baseJson = Command::toJson();

    if (armState)
    {
        QJsonObject paramObj;
        // Suppose armState has a toJson() that returns a QJsonObject
        paramObj["armState"] = armState->toJson();
        baseJson["parameters"] = paramObj;
    }
    return baseJson;
}

QJsonObject SetArmStateCommand::responseFormat() const
{
    // No special stateUpdate for this unless you want to include one
    return Command::responseFormat();
}

// -------------------------------------------------------------------
// RunTestCommand
// -------------------------------------------------------------------
RunTestCommand::RunTestCommand(int testIndex)
    : Command("runTest"), testIndex(testIndex)
{
}

QJsonObject RunTestCommand::toJson() const
{
    QJsonObject baseJson = Command::toJson();

    // Wrap parameters
    QJsonObject paramObj;
    paramObj["testIndex"] = testIndex;

    baseJson["parameters"] = paramObj;
    return baseJson;
}

QJsonObject RunTestCommand::responseFormat() const
{
    return Command::responseFormat();
}

// -------------------------------------------------------------------
// Factory
// -------------------------------------------------------------------
std::unique_ptr<Command> createCommand(const QJsonObject& json)
{
    if (!json.contains("command"))
    {
        qDebug() << "Invalid command JSON: Missing 'command' key"
                 << QJsonDocument(json).toJson(QJsonDocument::Compact);
        return nullptr;
    }

    QString cmd = json["command"].toString();
    if (cmd == "getState")
    {
        return std::make_unique<GetStateCommand>();
    }
    else if (cmd == "emergencyStop")
    {
        return std::make_unique<EmergencyStopCommand>();
    }
    else if (cmd == "homingSequence")
    {
        int axis = json["parameters"].toObject()["axis"].toInt(-1);
        return std::make_unique<HomingSequenceCommand>(axis);
    }
    else if (cmd == "setAxisAngle")
    {
        int axis = json["parameters"].toObject()["axis"].toInt(-1);
        float angle = static_cast<float>(json["parameters"].toObject()["angle"].toDouble(0.0));
        return std::make_unique<SetAxisAngleCommand>(axis, angle);
    }
    else if (cmd == "runTest")
    {
        int testIdx = json["parameters"].toObject()["testIndex"].toInt(-1);
        return std::make_unique<RunTestCommand>(testIdx);
    }
    else
    {
        qDebug() << "Unsupported command:" << cmd << QJsonDocument(json).toJson();
    }

    return nullptr;
}
