#pragma once

#include <glad/glad.h>
#include <stdio.h>
#include <EventHandler.h>

namespace ssCore
{
    template <class T>
    class Renderer
    {
    /*
    * This class is a template class that must be inherited by the renderer class
    * The derived class must implement the following functions:
    * Start()
    * RenderFirstPass()
    * RenderMainPass()
    * End()
    * The derived class must also have a constructor and destructor
    */
    public:
        Renderer()
        {
        }
        ~Renderer()
        {
        }

        /*
         * Called before the application loop starts
         */
        void Initialize()
        {
            // Initialize OpenGL
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                printf("Failed to initialize OpenGL context\n");
                return;
            }
            //program = std::make_unique<OpenGLProgram>();

            //glEnable(GL_DEBUG_OUTPUT);
		    static_cast<T*>(this)->Start();
        }

        /*
         * Called every frame in the application loop
         */
        void Render()
        {
            // Swap the buffers
            EventHandler::GetInstance().SwapBuffers();

            // Clear the screen
            //program->Clear();

            // First render pass: Shadows, reflections etc.
            static_cast<T*>(this)->RenderFirstPass();

            // Main render pass
            static_cast<T*>(this)->RenderMainPass();
            
            frameCount++;
        }

        void Terminate()
        {
            // Clean up here
            static_cast<T*>(this)->End();
        }

    protected:
        //std::unique_ptr<OpenGLProgram> program;
	    //std::shared_ptr<Scene> scene;
        long int frameCount = 0;

    //==============================================================
    // These functions must be implemented by the derived class
        /*
        * Called before application loop inside Initialize
        */
        virtual void Start() = 0;
        /*
        * Called in application loop inside Render before RenderMainPass
        */
        virtual void RenderFirstPass() = 0;
        /*
        * Called in application loop inside Render after RenderFirstPass
        */
        virtual void RenderMainPass() = 0;
        /*
        * Called after application loop inside Terminate
        */
        virtual void End() = 0;
    //================================================================
    };

    class TestRenderer : public Renderer<TestRenderer>
    {
    public:
        TestRenderer()
            : Renderer<TestRenderer>()
        {
        }
        ~TestRenderer()
        {
        }

        void Start()
        {
            printf("TestRenderer::Start()\n");
        }

        void RenderFirstPass()
        {
            printf("TestRenderer::RenderFirstPass()\n");
        }

        void RenderMainPass()
        {
            printf("TestRenderer::RenderMainPass()\n");
        }

        void End()
        {
            printf("TestRenderer::End()\n");
        }
    };
} // namespace ssCore