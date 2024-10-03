#ifndef IKSOLVER_H
#define IKSOLVER_H

#include <tuple>

class IKSolver{
public:
    void solve(std::tuple<float, float, float> InOffset);
    void requestHomingSequence();
private:
    std::tuple<float, float, float> endEffectorPosition;

};

#endif // IKSOLVER_H