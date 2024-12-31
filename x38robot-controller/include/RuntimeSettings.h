#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <QObject>
#include <atomic>

class RuntimeSettings : public QObject
{
    Q_OBJECT

public:
    static RuntimeSettings& instance();
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

signals:
    void debugModeChanged(bool enabled);

private:
    explicit RuntimeSettings(QObject* parent = nullptr);
    std::atomic<bool> debugMode;
};

#endif // RUNTIMESETTINGS_H
