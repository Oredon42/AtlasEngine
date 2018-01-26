#ifndef INTERACTION_H
#define INTERACTION_H

#include <glm/glm.hpp>

class Window;

class Interaction
{
public:
    enum KeyBoardKey
    {
        Unknown = -1,
        A = 0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        Escape,
        LControl, LShift, LAlt, LSystem,
        RControl, RShift, RAlt, RSystem,
        Menu,
        LBracket, RBracket,
        SemiColon, Comma, Period,
        Quote,
        Slash, BackSlash,
        Tilde,
        Equal,
        Dash,
        Space,
        Return,
        BackSpace,
        Tab,
        PageUp, PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add, Subtract, Multiply, Divide,
        Left, Right, Up, Down,
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
        Pause,
        KeyCount
    };

    enum MouseButton
    {
        Left,
        Right,
        Middle,
        XButton1,
        XButton2,
        ButtonCount
    };

    enum JoystickInfos
    {
        Count = 8,
        ButtonCount = 32,
        AxisCount = 8
    };

    enum JoystickAxis
    {
        X, Y, Z, R, U, V,
        PovX, PovY
    };

public:
    //  Keyboard
    inline static bool isKeyPressed(const KeyBoardKey &eKey);

    //  Mouse
    inline static bool isMouseButtonPressed(const MouseButton &eButton);
    inline static glm::vec2 getDesktopMousePosition();
    inline static glm::vec2 getWindowMousePosition(const Window *pRelativeWindow);
    inline static void setDesktopMousePosition(const unsigned int &x, const unsigned int &y);
    inline static void setWindowMousePosition(const unsigned int &x, const unsigned int &y, const Window *pRelativeWindow);

    //  Joystick
    inline static bool isConnected(const unsigned int &uiJoystick);
    inline static unsigned int getButtonCount(const unsigned int &uiJoystick);
    inline static bool hasAxis(const unsigned int &uiJoystick, const JoystickAxis &eAxis);
    inline static bool isButtonPressed(const unsigned int &uiJoystick, const unsigned int &uiButton);
    inline static float getAxisPosition(const unsigned int &uiJoystick, const JoystickAxis &eAxis);
};

#endif // INTERACTION_H
