#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
    Track State of Keyboard and Mouse and Expose it for the User
    -> if(Input::KEY_ESCAPE) { SOME_LOGIC_THAT_EXECUTES_ON_ESCAPE_PRESS; }
*/

namespace CRRT {

struct Input
{
    // Special Keys
    inline static bool KEY_ESCAPE         = false;
    inline static bool KEY_ENTER          = false;
    inline static bool KEY_TAB            = false;
    inline static bool KEY_CONTROL        = false;
    inline static bool KEY_SPACE          = false;
    inline static bool KEY_SHIFT          = false;
    inline static bool KEY_ALT            = false;
    
    // Arrow Keys
    inline static bool KEY_UP    = false;
    inline static bool KEY_DOWN  = false;
    inline static bool KEY_LEFT  = false;
    inline static bool KEY_RIGHT = false;

    // Mouse
    inline static bool   MOUSE_RIGHT_CLICK = false;    
    inline static bool   MOUSE_LEFT_CLICK  = false;
    inline static double MOUSE_X_DELTA     = 0.0;
    inline static double MOUSE_Y_DELTA     = 0.0;
    inline static double MOUSE_X           = 0.0;
    inline static double MOUSE_Y           = 0.0;

    // Numeric Keys
    inline static bool KEY_0 = false;
    inline static bool KEY_1 = false;
    inline static bool KEY_2 = false;
    inline static bool KEY_3 = false;
    inline static bool KEY_4 = false;
    inline static bool KEY_5 = false;
    inline static bool KEY_6 = false;
    inline static bool KEY_7 = false;
    inline static bool KEY_8 = false;
    inline static bool KEY_9 = false;

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
};

/* Helper Functions */
static inline void keyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_UP = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_DOWN = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyLeft(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_LEFT = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyRight(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_RIGHT = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyW(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_W = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyS(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_S = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyA(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_A = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyD(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_D = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_B = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyC(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_C = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyE(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_E = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyF(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_F = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyG(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_G = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyH(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_H = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyI(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_I = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyJ(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_J = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyK(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_K = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyL(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_L = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyM(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_M = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyN(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_N = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyO(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_O = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyP(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_P = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyQ(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_Q = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyR(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_R = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyT(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_T = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyU(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_U = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyV(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_V = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyX(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_X = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyY(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_Y = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyZ(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_Z = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyEscape(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_ESCAPE = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key0(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_0 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key1(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_1 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key2(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_2 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key3(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_3 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key4(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_4 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key5(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_5 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key6(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_6 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key7(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_7 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key8(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_8 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void key9(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_9 = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyEnter(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_ENTER = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyTab(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_TAB = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_CONTROL = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keySpace(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_SPACE = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyShift(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_SHIFT = action == GLFW_PRESS || action == GLFW_REPEAT;
}

static inline void keyAlt(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::KEY_ALT = action == GLFW_PRESS || action == GLFW_REPEAT;
}


static void leftMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    Input::MOUSE_LEFT_CLICK = action == GLFW_PRESS;
}

static void rightMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    Input::MOUSE_RIGHT_CLICK = action == GLFW_PRESS;
}

static void MouseUpdate(double X, double Y)
{
    Input::MOUSE_X_DELTA = X - Input::MOUSE_X;
    Input::MOUSE_Y_DELTA = Y - Input::MOUSE_Y;
    Input::MOUSE_X = X;
    Input::MOUSE_Y = Y;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch(key) {
        case (GLFW_KEY_0):
            key0(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_1):
            key1(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_2):
            key2(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_3):
            key3(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_4):
            key4(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_5):
            key5(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_6):
            key6(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_7):
            key7(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_8):
            key8(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_9):
            key9(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_ESCAPE):
            keyEscape(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_ENTER):
            keyEnter(window, key, scancode, action, mods);
            break;
        case (GLFW_KEY_TAB):
            keyTab(window, key, scancode, action, mods);
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

} // namespace CRRT
