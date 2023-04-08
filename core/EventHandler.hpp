#pragma once
#include <queue>

struct Event
{
    enum class Type
    {
        WindowClose,
        WindowResize,
        WindowMove,
        WindowFocus,
        WindowIconify,
        WindowMaximize,
        Keyboard,
        Char,
        MouseButton,
        MouseMove,
        MouseScroll,
        Drop,
        // GeometryChange,
        // TextureChange
    };

    Type type;
    union
    {
        struct
        {
            int width, height;
        } windowResize;
        struct
        {
            int x, y;
        } windowMove;
        struct
        {
            int focused;
        } windowFocus;
        struct
        {
            int iconified;
        } windowIconify;
        struct
        {
            int maximized;
        } windowMaximize;
        struct
        {
            int key, scancode, action, mods;
        } keyboard;
        struct
        {
            unsigned int codepoint;
        } character;
        struct
        {
            int button, action, mods;
        } mouseButton;
        struct
        {
            double x, y;
        } mouseMove;
        struct
        {
            double x, y;
        } mouseScroll;
        struct
        {
            int count;
            const char** paths;
        } drop;
    };
};

class Renderer;
class GLFWwindow;

class EventHandler
{
public:
    static EventHandler& GetInstance()
    {
        static EventHandler instance;
        return instance;
    }
    EventHandler(EventHandler const&) = delete;
    void operator=(EventHandler const&) = delete;
    void InitAndCreateWindow(int width=800, int height=600, const char* title="Strand Storm");
    void SwapBuffers();
    void Close();
    bool IsRunning();
    void SetWindowSize(int width, int height);
    void SetWindowPosition(int x, int y);
    void SetWindowName(const char* name);
    
    void DispatchEvents(Renderer& renderer);

    
    inline GLFWwindow* GetWindowPointer() { return windowHandle; }
    inline void QueueEvent(Event event) 
    {
        eventQueue.push(event); 
    }
    
private:
    EventHandler() = default;
    GLFWwindow* windowHandle=NULL;
    std::queue<Event> eventQueue;
    
    void setCallbacks();
};