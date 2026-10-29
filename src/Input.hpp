#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
    Track State of Keyboard and Mouse and Expose it for the User
    -> if(Input::KEY_ESCAPE) { SOME_LOGIC_THAT_EXECUTES_ON_ESCAPE_PRESS; }
*/

namespace CRRT {

void MouseUpdate(double X, double Y);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

}