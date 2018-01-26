#include "core/interaction.h"
#include "core/window.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>

//  Keyboard
bool Interaction::isKeyPressed(const Interaction::KeyBoardKey &eKey)
{
    return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(eKey));
}

//  Mouse
bool Interaction::isMouseButtonPressed(const Interaction::MouseButton &eButton)
{
    return sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(eButton));
}

glm::vec2 Interaction::getDesktopMousePosition()
{
    const sf::Vector2i &vMousePos = sf::Mouse::getPosition();
    return glm::vec2(vMousePos.x, vMousePos.y);
}

glm::vec2 Interaction::getWindowMousePosition(const Window *pRelativeWindow)
{
    const sf::Vector2i &vMousePos = sf::Mouse::getPosition(*pRelativeWindow->getWindow());
    return glm::vec2(vMousePos.x, vMousePos.y);
}

void Interaction::setDesktopMousePosition(const unsigned int &x, const unsigned int &y)
{
    sf::Mouse::setPosition(sf::Vector2i(x, y));
}

void Interaction::setWindowMousePosition(const unsigned int &x, const unsigned int &y, const Window *pRelativeWindow)
{
    sf::Mouse::setPosition(sf::Vector2i(x, y), *pRelativeWindow->getWindow());
}

//  Joystick
bool Interaction::isConnected(const unsigned int &uiJoystick)
{
    return sf::Joystick::isConnected(uiJoystick);
}

unsigned int Interaction::getButtonCount(const unsigned int &uiJoystick)
{
    return sf::Joystick::getButtonCount(uiJoystick);
}

bool Interaction::hasAxis(const unsigned int &uiJoystick, const Interaction::JoystickAxis &eAxis)
{
    return sf::Joystick::hasAxis(uiJoystick, static_cast<sf::Joystick::Axis>(eAxis));
}

bool Interaction::isButtonPressed(const unsigned int &uiJoystick, const unsigned int &uiButton)
{
    return sf::Joystick::isButtonPressed(uiJoystick, uiButton);
}

float Interaction::getAxisPosition(const unsigned int &uiJoystick, const Interaction::JoystickAxis &eAxis)
{
    return sf::Joystick::getAxisPosition(uiJoystick, static_cast<sf::Joystick::Axis>(eAxis));
}
