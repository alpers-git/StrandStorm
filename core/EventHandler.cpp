#include <iostream>
#include <EventHandler.hpp>
#include <Logging.hpp>
#include <Renderer.hpp>
#include "imgui.h"

void EventHandler::InitAndCreateWindow(int width, int height, const char* title)
{
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
    }
    
    windowHandle = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(windowHandle);
    glfwSwapInterval(1); // Enable vsync

    setCallbacks();

    QueueEvent(Event{Event::Type::WindowResize, width, height});
}

void EventHandler::SwapBuffers()
{
    glfwSwapBuffers(windowHandle);
}

void EventHandler::Close()
{
    glfwTerminate();
}


bool EventHandler::IsRunning()
{
    return !glfwWindowShouldClose(windowHandle);
}

void EventHandler::SetWindowSize(int width, int height)
{
    glfwSetWindowSize(windowHandle, width, height);
}

void EventHandler::SetWindowPosition(int x, int y)
{
    glfwSetWindowPos(windowHandle, x, y);
}

void EventHandler::SetWindowName(const char* name)
{
    glfwSetWindowTitle(windowHandle, name);
}

void EventHandler::setCallbacks()
{
    /*glfwSetWindowCloseCallback(windowHandle,
        [](GLFWwindow* window)
        {
            Event event;
            event.type = Event::Type::WindowClose;
            EventHandler::GetInstance().QueueEvent(event);
        });*/
    
    glfwSetWindowSizeCallback(windowHandle,
        [](GLFWwindow* window, int width, int height)
        {
            Event event;
            event.type = Event::Type::WindowResize;
            event.windowResize.width = width;
            event.windowResize.height = height;
            EventHandler::GetInstance().QueueEvent(event);
        });

    glfwSetWindowPosCallback(windowHandle,
        [](GLFWwindow* window, int xpos, int ypos)
        {
            Event event;
            event.type = Event::Type::WindowMove;
            event.windowMove.x = xpos;
            event.windowMove.y = ypos;
            EventHandler::GetInstance().QueueEvent(event);
        });
    
    glfwSetWindowFocusCallback(windowHandle,
        [](GLFWwindow* window, int focused)
        {
            Event event;
            event.type = Event::Type::WindowFocus;
            event.windowFocus.focused = focused;
            EventHandler::GetInstance().QueueEvent(event);
        });

    glfwSetWindowIconifyCallback(windowHandle,
        [](GLFWwindow* window, int iconified)
        {
            Event event;
            event.type = Event::Type::WindowIconify;
            event.windowIconify.iconified = iconified;
            EventHandler::GetInstance().QueueEvent(event);
        });
    
    glfwSetWindowMaximizeCallback(windowHandle,
        [](GLFWwindow* window, int maximized)
        {
            Event event;
            event.type = Event::Type::WindowMaximize;
            event.windowMaximize.maximized = maximized;
            EventHandler::GetInstance().QueueEvent(event);
        });

    glfwSetKeyCallback(windowHandle,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Event event;
            event.type = Event::Type::Keyboard;
            event.keyboard.key = key;
            event.keyboard.scancode = scancode;
            event.keyboard.action = action;
            event.keyboard.mods = mods;
            EventHandler::GetInstance().QueueEvent(event);
        });

    glfwSetCharCallback(windowHandle,
        [](GLFWwindow* window, unsigned int codepoint)
        {
            Event event;
            event.type = Event::Type::Char;
            event.character.codepoint = codepoint;
            EventHandler::GetInstance().QueueEvent(event);
        });
    
    glfwSetMouseButtonCallback(windowHandle,
        [](GLFWwindow* window, int button, int action, int mods)
        {
            Event event;
            event.type = Event::Type::MouseButton;
            event.mouseButton.button = button;
            event.mouseButton.action = action;
            event.mouseButton.mods = mods;
            EventHandler::GetInstance().QueueEvent(event);
        });
    
    glfwSetCursorPosCallback(windowHandle,
        [](GLFWwindow* window, double xpos, double ypos)
        {
            Event event;
            event.type = Event::Type::MouseMove;
            event.mouseMove.x = xpos;
            event.mouseMove.y = ypos;
            EventHandler::GetInstance().QueueEvent(event);
        });
    
    glfwSetScrollCallback(windowHandle,
        [](GLFWwindow* window, double xoffset, double yoffset)
        {
            Event event;
            event.type = Event::Type::MouseScroll;
            event.mouseScroll.x = xoffset;
            event.mouseScroll.y = yoffset;
            EventHandler::GetInstance().QueueEvent(event);
        });
}

void EventHandler::DispatchEvents(Renderer& renderer)
{
    glfwPollEvents();
    
    //This can maybe be done better
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse ) {
        //go through the queue and remove all mouse events
        std::queue<Event> tempQueue;
        while (!eventQueue.empty())
        {
            if (eventQueue.front().type != Event::Type::MouseButton && eventQueue.front().type != Event::Type::MouseMove && eventQueue.front().type != Event::Type::MouseScroll)
            {
                tempQueue.push(eventQueue.front());
            }
            eventQueue.pop();
        }
        eventQueue = tempQueue;
    }
    //This can maybe be done better
    if (io.WantCaptureKeyboard) {
        //go through the queue and remove all keyboard events
        std::queue<Event> tempQueue;
        while (!eventQueue.empty())
        {
            if (eventQueue.front().type != Event::Type::Keyboard && eventQueue.front().type != Event::Type::Char)
            {
                tempQueue.push(eventQueue.front());
            }
            eventQueue.pop();
        }
        eventQueue = tempQueue;
    }

    //while loop over events
    while (!eventQueue.empty()) {
        Event& e = eventQueue.front();
        switch (eventQueue.front().type) {
            case Event::Type::WindowClose:
                Close();
                break;
            case Event::Type::WindowResize:
                renderer.OnWindowResize(e.windowResize.width, e.windowResize.height);
                break;
            case Event::Type::WindowMove:
                // renderer.OnWindowMove(e.windowMove.x, e.windowMove.y);
                break;
            case Event::Type::WindowFocus:
                // renderer.OnWindowFocus(e.windowFocus.focused);
                break;
            case Event::Type::WindowIconify:
                // renderer.OnWindowIconify(e.windowIconify.iconified);
                break;
            case Event::Type::WindowMaximize:
                // renderer.OnWindowMaximize(e.windowMaximize.maximized);
                break;
            case Event::Type::Keyboard:
                // renderer.OnKeyboard(e.keyboard.key, e.keyboard.scancode, e.keyboard.action, e.keyboard.mods);
                break;
            /*case Event::Type::Char:
                renderer.OnChar(e.character.codepoint);
                break;*/
            case Event::Type::MouseButton:
                renderer.OnMouseButton(e.mouseButton.button, e.mouseButton.action, e.mouseButton.mods);
                break;
            case Event::Type::MouseMove:
                renderer.OnMouseMove(e.mouseMove.x, e.mouseMove.y);
                break;
            case Event::Type::MouseScroll:
                // renderer.OnMouseScroll(e.mouseScroll.x, e.mouseScroll.y);
                break;
            case Event::Type::PhysicsSync:
                renderer.PostPhysicsSync();
                break;
            // case Event::Type::GeometryChange:
            //     renderer.OnGeometryChange(e.geometryChange.e, e.geometryChange.toBeRemoved);
            //     break;
            // case Event::Type::TextureChange:
            //     renderer.OnTextureChange(e.textureChange.e, e.textureChange.toBeRemoved);
            //     break;
            /*case Event::Type::Drop:
                renderer.OnDrop(e.drop.count, e.drop.paths);
                break;*/
        }
        eventQueue.pop();
    }
}