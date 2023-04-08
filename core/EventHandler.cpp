#include <iostream>
#include <EventHandler.hpp>
#include <Logging.hpp>

void EventHandler::InitAndCreateWindow(int width, int height, const char* title)
{
	if (!glfwInit()) {
		spdlog::error("Failed to initialize GLFW");
	}
	
	windowHandle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1); // Enable vsync

	setCallbacks();
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