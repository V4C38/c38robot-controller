#pragma once

#include <QObject>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

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
};

// State Keeper for the whole chain
class ArmState : public QObject
{
    Q_OBJECT

public:
    explicit ArmState(QObject* parent = nullptr);

    bool loadConfigFile(const std::string& InConfigFilePath);
    void update(const std::string& InSerialData);

    std::optional<JointData> getJointDataByIndex(int InIndex) const;
    std::vector<JointData> getAllJointData() const;

signals:
    void onUpdated(const std::vector<JointData>& updatedJointData);
    

private:
    bool parseConfigFile(const std::string& InConfigFilePath);
    void parseSerialData(const std::string& InSerialData);

    std::vector<JointData> jointData;
};