#include <SFML/Graphics.hpp>
#include <string>
#include "button.h"
#include "diagram.h"
#include <iostream>

Button::Button(sf::RenderWindow* window, sf::Font font, int x, int y, int width, int height, std::string str) :
        m_text(),
        m_rectangle(sf::Vector2f(width * 15, height * 15)) ,
        m_f(font)
{
        m_w = window;
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;

        m_text.setString(str);
        m_text.setFont(m_f);
        m_text.setCharacterSize(15);
        m_text.setColor(sf::Color::Black);
        m_rectangle.setOutlineThickness(2);
        m_rectangle.setOutlineColor(sf::Color(255,255,255));

        m_rectangle.setPosition(x,y);
        m_text.setPosition(x,y-2);
    }

sf::VertexArray Button::GetDiagram(std::string str) {
    int p = 0;
    int q = 1;
    if(str.find('{') != std::string::npos && str.rfind('}') != std::string::npos) {//if both { and } are present
        str = str.substr(str.find('{') + 1, str.rfind('}') - str.find('{') - 1);
    }

    if(str.find('/') != std::string::npos) {
        p = ToInt(str.substr(0, str.find('/')));
        q = ToInt(str.substr(str.find('/') + 1, std::string::npos));
    } else {
        p = ToInt(str);
        q = 1;
    }

    sf::VertexArray ret(sf::Lines,p);

    int currAngle = 0;
    int nextAngle = 0;
    int scale = 64;
    int offset = 100;

    for(int i = 0; i < ret.getVertexCount(); i++) {
        //std::cout << i * 6.283 / p << std::endl;
        //std::cout << (i + q) % p * 6.283 / p << std::endl;

        //calculates the angle that each given point will be at
        currAngle = i * 6.283 / p;
        nextAngle = (i + q) % p * 6.283 / p;

        //Actually adds in the lines to the diagram
        ret[i].position = sf::Vector2f(cos(currAngle) * scale + offset, sin(currAngle) * scale + offset);
        ret[i + 1].position = sf::Vector2f(cos(nextAngle) * scale + offset, sin(nextAngle) * scale + offset);
        ret[i].color = sf::Color::White;
        ret[i + 1].color = sf::Color::White;
    }
    return ret;
}

void Button::Draw(){
    m_w->draw(m_rectangle);
    m_w->draw(m_text);
    m_w->draw(m_shape);
}

void Button::Press(int xP, int yP, InputBox b){
    if(m_x < xP && xP <m_x + m_width * 15 && m_y < yP && yP < m_y + m_height * 15) {
        //d = Diagram(w, 0, 0, 100, 100, b.stored.getString());
        m_shape = GetDiagram(b.GetStoredString());
    }
}
