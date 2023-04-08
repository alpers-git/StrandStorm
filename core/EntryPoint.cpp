#include <App.hpp>
#include <Logging.hpp>

int main(int argc, char* argv[])
{
    setupLogging();
    
    EventHandler &eventHandler = EventHandler::GetInstance();
    eventHandler.InitAndCreateWindow(1280, 720, "StrandStrom");

    // Initialize OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::info("Failed to initialize OpenGL context");
        return -1;
    }
    glEnable(GL_DEBUG_OUTPUT);
    GL_CALL(glDebugMessageCallback(GLDebugMessageCallback, NULL));

    App app(argc, argv);
    app.Run(eventHandler);
    return 0;
}