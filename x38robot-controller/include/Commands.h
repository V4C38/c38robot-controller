#pragma once

#include <QJsonObject>
#include <QString>
#include <memory>
#include <QUuid>
#include <QDebug>

// Forward declaration to avoid circular dependencies
class ArmState;

// Base class for commands
class Command
{
public:
    explicit Command(const QString& commandType);
    virtual ~Command() = default;

    virtual QJsonObject toJson() const;
    QString toString() const;

    QUuid getUuid() const { return uuid; }
    QString getCommandType() const { return commandType; }
    QJsonObject generateDebugResponse() const;

protected:
    QUuid uuid;
    QString commandType;
};

// Derived classes for specific commands
class GetStateCommand : public Command
{
public:
    GetStateCommand();
    QJsonObject toJson() const override;
};

class EmergencyStopCommand : public Command
{
public:
    EmergencyStopCommand();
    QJsonObject toJson() const override;
};

class HomingSequenceCommand : public Command
{
public:
    explicit HomingSequenceCommand(int axis = -1);
    QJsonObject toJson() const override;

private:
    int axis;
};

class SetAxisAngleCommand : public Command
{
public:
    SetAxisAngleCommand(int axis, float angle);
    QJsonObject toJson() const override;

private:
    int axis;
    float angle;
};

class SetArmStateCommand : public Command
{
public:
    explicit SetArmStateCommand(const ArmState* armState);
    QJsonObject toJson() const override;

private:
    const ArmState* armState;
};

// Factory method to create commands from JSON
std::unique_ptr<Command> createCommand(const QJsonObject& json);
