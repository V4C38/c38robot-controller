#include "RuntimeSettings.h"

RuntimeSettings& RuntimeSettings::instance()
{
    static RuntimeSettings instance;
    return instance;
}

RuntimeSettings::RuntimeSettings(QObject* parent)
    : QObject(parent), debugMode(false) {}

void RuntimeSettings::setDebugMode(bool enabled)
{
    if (debugMode != enabled)
    {
        debugMode = enabled;
        emit debugModeChanged(enabled);
    }
}

bool RuntimeSettings::isDebugMode() const
{
    return debugMode.load();
}
