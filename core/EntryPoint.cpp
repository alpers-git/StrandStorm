#include <Application.h>
#include <Logging.hpp>

int main(int argc, char* argv[])
{
    setupLogging();
    ssCore::Application<ssCore::TestRenderer> app(argc, argv);
    app.Run();
    return 0;
}