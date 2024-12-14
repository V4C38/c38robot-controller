
#include "ArmState.h"
#include <fstream>
#include <sstream>
#include <iostream>


ArmState::ArmState(QObject* parent) : QObject(parent)
{
}

void ArmState::update(const std::string& InSerialData)
{
    parseSerialData(InSerialData);
    emit onUpdated(jointData);
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
    if (!configFile.is_open())
    {
        std::cerr << "Failed to open config file: " << InConfigFilePath << std::endl;
        return false;
    }

    return parseConfigFile(InConfigFilePath);
}

// Parse config file
bool ArmState::parseConfigFile(const std::string& InConfigFilePath)
{
    std::ifstream configFile(InConfigFilePath);
    if (!configFile.is_open()) return false;

    std::string line;
    int jointIndex = -1;

    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        std::string key;
        float value;

        // Detect joint sections
        if (line.find("Joint") != std::string::npos)
        {
            jointData.emplace_back();  // Add a new joint entry
            jointIndex++;
            continue;
        }

        if (iss >> key >> value)
        {
            // Parse joint values
            if (jointIndex >= 0 && jointIndex < jointData.size())
            {
                if (key == "theta") jointData[jointIndex].dHParameters.theta = value;
                else if (key == "d") jointData[jointIndex].dHParameters.d = value;
                else if (key == "a") jointData[jointIndex].dHParameters.a = value;
                else if (key == "alpha") jointData[jointIndex].dHParameters.alpha = value;
            }
        }
    }

    //emit onUpdated(jointData);
    std::cout << "Successfully loaded " << jointData.size() << " joints from config.\n";
    return !jointData.empty();
}
