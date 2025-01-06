#pragma once

#include <QJsonObject>
#include <QUuid>
#include <memory>
#include <map>
#include <QDebug>

class ArmState;

// -------------------------------------------------------------------
// Base Command
// -------------------------------------------------------------------
class Command
{
public:
    Command(const QString& commandType);
    virtual ~Command() = default;

    virtual QJsonObject toJson() const;
    virtual QJsonObject responseFormat() const;
    
    QString getCommandType() const { return commandType; }
    QUuid getUuid() const { return uuid; }

protected:
    QJsonObject createStateUpdate(const QMap<int, float>& updatedAxes) const;

protected:
    QUuid uuid;
    QString commandType;
};

// -------------------------------------------------------------------
// HomingSequenceCommand
// -------------------------------------------------------------------
class HomingSequenceCommand : public Command
{
public:
    HomingSequenceCommand(int axis);
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;

private:
    int axis;
};

// -------------------------------------------------------------------
// SetAxisAngleCommand
// -------------------------------------------------------------------
class SetAxisAngleCommand : public Command
{
public:
    SetAxisAngleCommand(int axis, float angle);
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;

private:
    int axis;
    float angle;
};

// -------------------------------------------------------------------
// GetStateCommand
// -------------------------------------------------------------------
class GetStateCommand : public Command
{
public:
    GetStateCommand();
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;
};

// -------------------------------------------------------------------
// EmergencyStopCommand
// -------------------------------------------------------------------
class EmergencyStopCommand : public Command
{
public:
    EmergencyStopCommand();
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;
};

// -------------------------------------------------------------------
// SetArmStateCommand
// -------------------------------------------------------------------
class SetArmStateCommand : public Command
{
public:
    explicit SetArmStateCommand(const ArmState* armState);
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;

private:
    const ArmState* armState;
};

// -------------------------------------------------------------------
// RunTestCommand
// -------------------------------------------------------------------
class RunTestCommand : public Command
{
public:
    RunTestCommand(int testIndex);
    QJsonObject toJson() const override;
    QJsonObject responseFormat() const override;

private:
    int testIndex;
};

// -------------------------------------------------------------------
// Factory
// -------------------------------------------------------------------
std::unique_ptr<Command> createCommand(const QJsonObject& json);
