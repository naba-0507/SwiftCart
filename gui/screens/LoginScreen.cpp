#include "../include/screens/LoginScreen.hpp"
// Login has no sidebar - centred card layout
LoginScreen::LoginScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    // Card centred in window: x=400,w=480
    const float cx=400.f, cw=480.f, fx=cx+40.f, fw=cw-80.f;
    emailField    = ui.makeInput("Email address", fx, 316.f, fw, 44.f);
    passwordField = ui.makeInput("Password",      fx, 384.f, fw, 44.f, true);
    loginBtn      = ui.makeConfirmButton("Sign In",        fx, 450.f, fw, 44.f);
    toRegisterBtn = ui.makeSecondaryButton("Create Account",fx,506.f, fw, 38.f);
    titleText    = ui.makeText("SwiftCart", 52, Colors::Primary, true);
    subtitleText = ui.makeText("Sign in to continue", 17, Colors::TextSecondary);
    errorText    = ui.makeText("", 14, Colors::Danger);
    errorText.setPosition(fx, 558.f);
}
void LoginScreen::onEnter() {
    emailField.clear(); passwordField.clear(); errorText.setString("");
    // Centre title and subtitle over card (400+240=640)
    auto tb=titleText.getLocalBounds();
    titleText.setPosition(640.f-tb.width/2.f-tb.left, 165.f);
    auto sb=subtitleText.getLocalBounds();
    subtitleText.setPosition(640.f-sb.width/2.f-sb.left, 232.f);
}
void LoginScreen::handleEvent(const sf::Event& e, sf::RenderWindow&) {
    emailField.handleEvent(e); passwordField.handleEvent(e);
    if(e.type==sf::Event::MouseButtonPressed&&e.mouseButton.button==sf::Mouse::Left){
        sf::Vector2f mp((float)e.mouseButton.x,(float)e.mouseButton.y);
        if(loginBtn.contains(mp))      loginBtn.isPressed=true;
        if(toRegisterBtn.contains(mp)) toRegisterBtn.isPressed=true;
    }
    if(e.type==sf::Event::MouseButtonReleased&&e.mouseButton.button==sf::Mouse::Left){
        sf::Vector2f mp((float)e.mouseButton.x,(float)e.mouseButton.y);
        if(loginBtn.isPressed&&loginBtn.contains(mp))           tryLogin();
        if(toRegisterBtn.isPressed&&toRegisterBtn.contains(mp)) nextScreen="register";
        loginBtn.isPressed=toRegisterBtn.isPressed=false;
    }
    if(e.type==sf::Event::KeyPressed&&e.key.code==sf::Keyboard::Return) tryLogin();
}
void LoginScreen::tryLogin() {
    if(emailField.content.empty()||passwordField.content.empty()){
        errorText.setString("Please fill in all fields."); return;
    }
    if(platform.login(emailField.content,passwordField.content)){
        auto role=platform.getActiveRole();
        if(role=="customer") nextScreen="customer";
        else if(role=="seller") nextScreen="seller";
        else if(role=="admin") nextScreen="admin";
    } else {
        errorText.setString("Invalid email or password."); passwordField.clear();
    }
}
void LoginScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m=static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    emailField.update(dt); passwordField.update(dt);
    loginBtn.update(m); toRegisterBtn.update(m); note.update(dt);
}
void LoginScreen::render(sf::RenderWindow& w, bool /*clicked*/, sf::Vector2f /*mouse*/) {
    // No sidebar on login
    auto card=ui.makeCard(400.f,148.f,480.f,460.f); w.draw(card);
    w.draw(titleText); w.draw(subtitleText);
    const float fx=440.f;
    auto el=ui.makeText("Email",13,Colors::TextSecondary);
    auto pl=ui.makeText("Password",13,Colors::TextSecondary);
    el.setPosition(fx,298.f); w.draw(el);
    pl.setPosition(fx,366.f); w.draw(pl);
    emailField.draw(w); passwordField.draw(w);
    loginBtn.draw(w); toRegisterBtn.draw(w);
    w.draw(errorText); note.draw(w);
}
