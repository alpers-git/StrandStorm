#include <PhysicsIntegrator.hpp>
#include <sstream>

void PhysicsIntegrator::Initialize()
{
    //Initialize the physics integrator here
}

void PhysicsIntegrator::Integrate()
{
    for (int i = 0; i < numSteps; i++)
        TakeStep(dt);
    //Call Event Handler to scynronize the rendering geometry with the physics
    for (size_t i = 0; i < scene->rods.size(); i++) {
        scene->hairMesh.updateFrom(scene->rods[i], i);
    }
    Event e;
    e.type = Event::Type::PhysicsSync;
    EventHandler::GetInstance().QueueEvent(e);
}

void PhysicsIntegrator::TakeStep(float dt)
{
    //Integrate the physics here
    // printf("Integrating physics with dt = %f numSteps = %d dt*numSteps = %f\r", dt, numSteps, dt*numSteps);

    //Integrate the elastic rod
    for (ElasticRod& rod : scene->rods) {
        rod.integrateFwEuler(dt);
    }
    // {std::stringstream s;
    // s << scene->rods[0].v[6];
    // spdlog::debug("v = {}", s.str());}
    // {std::stringstream s;
    // s << scene->rods[0].x[6];
    // spdlog::debug("x = {}", s.str());}
}