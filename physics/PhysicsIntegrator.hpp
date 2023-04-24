#pragma once
#include <memory>
#include <Scene.hpp>
#include <Logging.hpp>
#include <ElasticRod.hpp>

class PhysicsIntegrator
{

public:

    void Initialize();
    void Integrate();

    std::shared_ptr<Scene> scene;

    //getters and setters
    float getDt() const { return dt; }
    void setDt(float dt) { this->dt = std::max(dt,0.00001f); }
    int getNumSteps() const { return numSteps; }
    void setNumSteps(int numSteps) { this->numSteps = std::max(numSteps, 1); }

private:
    void TakeStep(float dt);
    float dt = 0.01f;
    int numSteps = 2;
};

