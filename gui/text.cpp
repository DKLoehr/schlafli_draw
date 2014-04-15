#include <SFML/Graphics.hpp>
#include <string>
#include "text.h"

InputBox::InputBox(sf::RenderWindow* window, sf::Font font, int x, int y, int charWidth, int charHeight):
    m_stored(),
    m_rectangle(sf::Vector2f(charWidth * 15, charHeight * 15))
{
    m_w = window;
    m_f = font;
    m_x = x;
    m_y = y;
    m_width = charWidth;
    m_height = charHeight;
    m_isFocused = false;

    m_stored.setString("");
    m_stored.setFont(m_f);
    m_stored.setCharacterSize(15);
    m_stored.setColor(sf::Color::Black);
    m_rectangle.setOutlineThickness(2);
    m_rectangle.setOutlineColor(sf::Color(100,100,100));

    m_rectangle.setPosition(m_x, m_y);
    m_stored.setPosition(m_x, m_y - 2);
};

void InputBox::Draw() {
    m_w->draw(m_rectangle);
    m_w->draw(m_stored);
}

void InputBox::EnterText(char n) {
    if(n != 8 && n != 13 && n < 128) {
        m_stored.setString(m_stored.getString() + n);
    } else if(n == 8) {
        std::string temp = m_stored.getString();
        if(temp.length() > 0)
            m_stored.setString(temp.substr(0, temp.length() - 1));
    } else if(n == 13) {
        //do whatever we do when people hit enter
    }
}

std::string InputBox::GetStoredString() {
    return m_stored.getString();
}
