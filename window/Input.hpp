#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Seems redudant but it is import to keep input seperate from the windowing api */
struct Input
{
    // Special Keys
    inline static bool KEY_ESCAPE         = false;
    inline static bool KEY_CONTROL        = false;
    inline static bool KEY_SPACE          = false;
    inline static bool KEY_SHIFT          = false;
    inline static bool KEY_ALT            = false;
    
    // Mouse
    inline static bool   MOUSE_RIGHT_CLICK = false;    
    inline static bool MOUSE_LEFT_CLICK  = false;
    inline static double MOUSE_X_DELTA = 0.0;
    inline static double MOUSE_Y_DELTA = 0.0;
    inline static double MOUSE_X       = 0.0;
    inline static double MOUSE_Y       = 0.0;

    // Alphabetical Keys
    inline static bool KEY_W = false;
    inline static bool KEY_A = false;
    inline static bool KEY_S = false;
    inline static bool KEY_D = false;
    inline static bool KEY_B = false;
    inline static bool KEY_C = false;
    inline static bool KEY_E = false;
    inline static bool KEY_F = false;
    inline static bool KEY_G = false;
    inline static bool KEY_H = false;
    inline static bool KEY_I = false;
    inline static bool KEY_J = false;
    inline static bool KEY_K = false;
    inline static bool KEY_L = false;
    inline static bool KEY_M = false;
    inline static bool KEY_N = false;
    inline static bool KEY_O = false;
    inline static bool KEY_P = false;
    inline static bool KEY_Q = false;
    inline static bool KEY_R = false;
    inline static bool KEY_T = false;
    inline static bool KEY_U = false;
    inline static bool KEY_V = false;
    inline static bool KEY_X = false;
    inline static bool KEY_Y = false;
    inline static bool KEY_Z = false;

    inline static bool KEY_UP    = false;
    inline static bool KEY_DOWN  = false;
    inline static bool KEY_LEFT  = false;
    inline static bool KEY_RIGHT = false;

    static void MouseUpdate(double X, double Y)
    {
        MOUSE_X_DELTA = X - MOUSE_X;
        MOUSE_Y_DELTA = Y - MOUSE_Y;
        MOUSE_X = X;
        MOUSE_Y = Y;
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch(key) {
            case (GLFW_KEY_ESCAPE):
                keyEscape(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_LEFT_CONTROL):
                keyControl(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_RIGHT_CONTROL):
                keyControl(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_UP):
                keyUp(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_DOWN):
                keyDown(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_LEFT):
                keyLeft(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_RIGHT):
                keyRight(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_W):
                keyW(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_S):
                keyS(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_A):
                keyA(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_D):
                keyD(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_B):
                keyB(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_C):
                keyC(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_E):
                keyE(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_F):
                keyF(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_G):
                keyG(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_H):
                keyH(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_I):
                keyI(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_J):
                keyJ(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_K):
                keyK(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_L):
                keyL(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_M):
                keyM(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_N):
                keyN(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_O):
                keyO(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_P):
                keyP(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_Q):
                keyQ(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_R):
                keyR(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_T):
                keyT(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_U):
                keyU(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_V):
                keyV(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_X):
                keyX(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_Y):
                keyY(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_Z):
                keyZ(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_SPACE):
                keySpace(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_LEFT_SHIFT):
                keyShift(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_RIGHT_SHIFT):
                keyShift(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_LEFT_ALT):
                keyAlt(window, key, scancode, action, mods);
                break;
            case (GLFW_KEY_RIGHT_ALT):
                keyAlt(window, key, scancode, action, mods);
                break;
            // Add aditional keys here
            // ...
            default:
                break;
        }
    }

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        switch(button) {
            case (GLFW_MOUSE_BUTTON_LEFT):
                leftMouseButton(window, button, action, mods);
                break;
            case (GLFW_MOUSE_BUTTON_RIGHT):
                rightMouseButton(window, button, action, mods);
                break;
            // Add aditional buttons here
            // ...
            default:
                break;
        }
    }

    /* Helper Functions */
    static void keyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_UP = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_DOWN = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyLeft(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_LEFT = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyRight(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_RIGHT = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyW(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_W = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyS(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_S = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyA(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_A = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyD(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_D = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyB(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_B = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyC(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_C = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyE(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_E = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyF(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_F = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyG(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_G = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyH(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_H = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyI(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_I = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyJ(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_J = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyK(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_K = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyL(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_L = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyM(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_M = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyN(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_N = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyO(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_O = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyP(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_P = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyQ(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_Q = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyR(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_R = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyT(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_T = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyU(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_U = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyV(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_V = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyX(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_X = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyY(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_Y = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyZ(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_Z = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyEscape(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_ESCAPE = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyControl(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_CONTROL = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keySpace(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_SPACE = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyShift(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_SHIFT = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    static void keyAlt(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        KEY_ALT = action == GLFW_PRESS || action == GLFW_REPEAT;
    }


    static void leftMouseButton(GLFWwindow *window, int button, int action, int mods)
    {
        MOUSE_LEFT_CLICK = action == GLFW_PRESS;
    }

    static void rightMouseButton(GLFWwindow *window, int button, int action, int mods)
    {
        MOUSE_RIGHT_CLICK = action == GLFW_PRESS;
    }

};
