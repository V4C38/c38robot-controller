
    #include "ArmState.h"
    #include <fstream>
    #include <sstream>
    #include <iostream>
    
    #include <QJsonObject>



    ArmState::ArmState(QObject* parent) : QObject(parent)
    {
    }

    // Convert ArmState to JSON
    QJsonObject ArmState::toJson() const
    {
        QJsonObject json;
        for (size_t i = 0; i < jointData.size(); ++i)
        {
            const auto& joint = jointData[i];
            QJsonObject jointJson;
            jointJson["isCalibrated"] = joint.isCalibrated;
            jointJson["currentAngle"] = joint.currentAngle;
            jointJson["targetAngle"] = joint.targetAngle;
            jointJson["theta"] = joint.dHParameters.theta;
            jointJson["d"] = joint.dHParameters.d;
            jointJson["a"] = joint.dHParameters.a;
            jointJson["alpha"] = joint.dHParameters.alpha;
            jointJson["minAngle"] = joint.minAngle;
            jointJson["maxAngle"] = joint.maxAngle;
            json[QString("Joint%1").arg(i)] = jointJson;
        }
        return json;
    }

    void ArmState::updateFromJson(const QJsonObject& json)
    {
        // Example: Update state based on the JSON keys and values
        if (json.contains("joints") && json["joints"].isArray())
        {
            QJsonArray jointsArray = json["joints"].toArray();
            for (int i = 0; i < jointsArray.size(); ++i)
            {
                if (i < jointData.size())
                {
                    QJsonObject jointObj = jointsArray[i].toObject();
                    if (jointObj.contains("angle"))
                    {
                        jointData[i].currentAngle = jointObj["angle"].toDouble();
                    }
                }
            }
            qDebug() << "ArmState updated from JSON.";
        }
        else
        {
            qDebug() << "Invalid stateUpdate JSON.";
        }
    }

    // Parse incoming serial data - TBD
    void ArmState::parseSerialData(const std::string& InSerialData)
    {
        return;
    }

    // Safely get joint data by index
    std::optional<JointData> ArmState::getJointDataByIndex(int InIndex) const
    {
        if (InIndex < 0 || InIndex >= jointData.size())
        {
            std::cerr << "Index out of range: " << InIndex << std::endl;
            return std::nullopt;
        }
        return jointData[InIndex];
    }

    // Return all joint data
    std::vector<JointData> ArmState::getAllJointData() const
    {
        return jointData;
    }

    // Load arm configuration from .ini file
    bool ArmState::loadConfigFile(const std::string& InConfigFilePath)
    {
        std::ifstream configFile(InConfigFilePath);
        qDebug() << "ArmState::loadConfigFile called with path:" << QString::fromStdString(InConfigFilePath);
        if (!configFile.is_open())
        {
            std::cerr << "Failed to open config file: " << InConfigFilePath << std::endl;
            return false;
        }

        std::string line;
        int jointIndex = -1;

        while (std::getline(configFile, line))
        {
            // Trim whitespace
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            line.erase(0, line.find_first_not_of(" \t\r\n"));

            // Skip comments or empty lines
            if (line.empty() || line[0] == '#')
                continue;

            // Detect [JointX] sections
            if (line.find("Joint") != std::string::npos)
            {
                jointData.emplace_back();
                jointIndex++;
                continue;
            }

            // Parse lines in the format key=value
            auto eqPos = line.find('=');
            if (eqPos != std::string::npos)
            {
                std::string key = line.substr(0, eqPos);
                std::string valStr = line.substr(eqPos + 1);

                // Trim whitespace from key and value
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                valStr.erase(valStr.find_last_not_of(" \t\r\n") + 1);
                valStr.erase(0, valStr.find_first_not_of(" \t\r\n"));

                try
                {
                    float val = std::stof(valStr);

                    if (jointIndex >= 0 && jointIndex < (int)jointData.size())
                    {
                        if (key == "theta") jointData[jointIndex].dHParameters.theta = val;
                        else if (key == "d") jointData[jointIndex].dHParameters.d = val;
                        else if (key == "a") jointData[jointIndex].dHParameters.a = val;
                        else if (key == "alpha") jointData[jointIndex].dHParameters.alpha = val;
                        else if (key == "min_angle") jointData[jointIndex].minAngle = val;
                        else if (key == "max_angle") jointData[jointIndex].maxAngle = val;
                    }
                }
                catch (const std::invalid_argument& e)
                {
                    std::cerr << "Invalid value for key " << key << " in config file.\n";
                }
            }
        }

        // Print each joint's data once all parameters have been parsed
        for (size_t i = 0; i < jointData.size(); ++i)
        {
            const auto& joint = jointData[i];
            qDebug() << QString("Registered Joint %1: theta=%2 d=%3 a=%4 alpha=%5 min_angle=%6 max_angle=%7")
                            .arg(i)
                            .arg(joint.dHParameters.theta)
                            .arg(joint.dHParameters.d)
                            .arg(joint.dHParameters.a)
                            .arg(joint.dHParameters.alpha)
                            .arg(joint.minAngle)
                            .arg(joint.maxAngle);
        }

        emit onLoadedJointData(jointData);
        std::cout << "Successfully loaded " << jointData.size() << " joints from config.\n";
        return !jointData.empty();
    }
