#include <Application.h>

int main(int argc, char* argv[])
{
    ssCore::Application<ssCore::TestRenderer> app(argc, argv);
    app.Run();
    return 0;
}