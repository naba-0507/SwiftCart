#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
class RegisterScreen : public Screen {
public:
    RegisterScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    Button customerTabBtn, sellerTabBtn;
    bool isSellerTab = false;
    InputField nameField, emailField, passwordField, addressField, shopNameField;
    Button registerBtn, backBtn;
    Notification note;
    void tryRegister();
};
