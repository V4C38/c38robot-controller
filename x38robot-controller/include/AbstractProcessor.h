// AbstractProcessor.h
#ifndef ABSTRACTPROCESSOR_H
#define ABSTRACTPROCESSOR_H

#include "IKSolver.h"

class AbstractProcessor {
public:
    virtual ~AbstractProcessor() = default;
    virtual void initialize(IKSolver* inIKSolver) = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
    virtual bool isActive() const = 0;

protected:
    IKSolver* ikSolver = nullptr;
};

#endif // ABSTRACTPROCESSOR_H