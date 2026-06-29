#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
class LoginScreen : public Screen {
public:
    LoginScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    InputField emailField, passwordField;
    Button loginBtn, toRegisterBtn;
    Notification note;
    sf::Text titleText, subtitleText, errorText;
    void tryLogin();
};
