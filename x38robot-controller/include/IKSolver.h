#ifndef IKSOLVER_H
#define IKSOLVER_H

#include <tuple>

class IKSolver{
public:
    void solve(std::tuple<float, float, float> InOffset){
    }

private:
    std::tuple<float, float, float> endEffectorPosition;

};

#endif // IKSOLVER_H