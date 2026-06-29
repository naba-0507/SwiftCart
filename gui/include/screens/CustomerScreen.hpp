#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
class CustomerScreen : public Screen {
public:
    CustomerScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    sf::Text welcomeText, statOrders, statCartItems, profileInfo;
    sf::RectangleShape statsCard;
    bool showProfile = false;
    std::vector<NavItem> nav;
};
