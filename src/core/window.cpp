#include "core/window.h"

#include <SFML/Window/Window.hpp>

Window::Window(const std::string &strName, const unsigned int &uiWidth, const unsigned int &uiHeight)
{
    m_pWindow = new sf::Window(sf::VideoMode(uiWidth, uiHeight), strName);
}

Window::~Window()
{
    delete m_pWindow;
}

void Window::resize(const unsigned int &uiWidth, const unsigned int &uiHeight)
{
    m_pWindow->setSize(sf::Vector2u(uiWidth, uiHeight));
}

void Window::close()
{
    m_pWindow->close();
}

bool Window::isOpen() const
{
    return m_pWindow->isOpen();
}