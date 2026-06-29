#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
class CartScreen : public Screen {
public:
    CartScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    ScrollPanel cartList;
    InputField discountField;
    Button applyDiscountBtn, removeItemBtn, checkoutBtn;
    sf::RectangleShape summaryCard;
    sf::Text subtotalText, discountText, totalText;
    Notification note;
    std::vector<NavItem> nav;
    void refreshCart();
    void removeSelected();
    void checkout();
};
