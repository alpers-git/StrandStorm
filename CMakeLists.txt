CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0 FATAL_ERROR)
cmake_policy(VERSION 3.0.0)

project(StrandStrom)

#========== Global Configurations =============#
#----------------------------------------------#

# Set the C++ standard for all targets
# OPTIONAL:
#---------------------------------
set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)


#========= Dependency Configurations ==========#
find_package(OpenGL REQUIRED)

add_subdirectory(submodules/glm EXCLUDE_FROM_ALL)
add_subdirectory(submodules/eigen EXCLUDE_FROM_ALL)

#--------------------GLAD---------------------#
#https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=compatibility&loader=on
add_subdirectory(submodules/glad/)
#---------------------------------------------#

#--------------------GLFW---------------------#
set(GLFW_BUILD_DOCS OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
add_subdirectory(submodules/glfw)
set_property(TARGET glfw PROPERTY FOLDER "submodules/GLFW3" )
#----------------------------------------------#

#--------------------ImGui---------------------#
# Set where the ImGui files are stored
set(IMGUI_PATH  submodules/imgui)

# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES}
            ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
            ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends glfw)
#---------------------------------------------#

#--------------------EnTT---------------------#
add_subdirectory(submodules/entt)
add_compile_options(-DENTT_BUILD_DOCS=ON)
#---------------------------------------------#

include_directories(
    submodules/imgui
    submodules/lodepng
    submodules/glfw/include
    submodules/cyCodeBase
    submodules/eigen
    core
#    physics
)
#========== Targets Configurations ===========#
add_executable(strandStorm 
    core/EntryPoint.cpp
    core/EventHandler.cpp
    core/OpenGLProgram.cpp
    core/GUIManager.cpp
#    core/Scene.cpp
)
    
# Set executable dependency libraries
target_link_libraries(strandStorm
    PRIVATE ${OPENGL_LIBRARIES}
    PRIVATE glad
    PRIVATE glfw
    PRIVATE glm::glm
    PRIVATE Eigen3::Eigen
    PRIVATE EnTT::EnTT
    PUBLIC ImGui
)