#ifndef WINDOW_H
#define WINDOW_H

namespace sf
{
    class Window;
}   // namespace sf

class Window
{
public:
    Window(const std::string &strName = "Window", const unsigned int &uiWidth = 800, const unsigned int &uiHeight = 600);
    ~Window();

    inline sf::Window *getWindow() const{return m_pWindow;}
    void resize(const unsigned int &uiWidth, const unsigned int &uiHeight);
    void close();

    bool isOpen() const;

private:
    sf::Window *m_pWindow;
};

#endif // WINDOW_H
