#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray> 

// Struct for each joint
struct JointData
{
    bool isCalibrated = false;
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;

    struct DHParameters
    {
        float theta = 0.0f;
        float d = 0.0f;
        float a = 0.0f;
        float alpha = 0.0f;
    } dHParameters;

    float minAngle = -180.0f;
    float maxAngle = 180.0f;
};

// State Keeper for the whole chain
class ArmState : public QObject
{
    Q_OBJECT

public:
    explicit ArmState(QObject* parent = nullptr);
    QJsonObject toJson() const;public:
    void updateFromJson(const QJsonObject& json);

    bool loadConfigFile(const std::string& InConfigFilePath);

    std::optional<JointData> getJointDataByIndex(int InIndex) const;
    std::vector<JointData> getAllJointData() const;

signals:
    void onUpdated(const std::vector<JointData>& updatedJointData);
    void onLoadedJointData(const std::vector<JointData>& loadedJointData);

private:
    void parseSerialData(const std::string& InSerialData);

    std::vector<JointData> jointData;
};