#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Screen {
public:
    virtual ~Screen() = default;
    virtual void handleEvent(const sf::Event& e, sf::RenderWindow& w) = 0;
    virtual void update(float dt, sf::RenderWindow& w) = 0;
    virtual void render(sf::RenderWindow& w, bool sidebarClicked, sf::Vector2f mouse) = 0;
    virtual void onEnter() {}
    virtual void onExit()  {}
    std::string nextScreen;
};
