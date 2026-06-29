#include "../include/screens/RegisterScreen.hpp"
RegisterScreen::RegisterScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    // Layout: card y=80, title y=96, tabs y=156, fields below
    customerTabBtn = ui.makeSecondaryButton("Customer", 400.f,156.f,200.f,38.f);
    sellerTabBtn   = ui.makeSecondaryButton("Seller",   604.f,156.f,200.f,38.f);
    nameField      = ui.makeInput("Full name",          400.f,212.f,480.f,44.f);
    emailField     = ui.makeInput("Email address",      400.f,270.f,480.f,44.f);
    passwordField  = ui.makeInput("Password",           400.f,328.f,480.f,44.f,true);
    addressField   = ui.makeInput("Delivery address",   400.f,386.f,480.f,44.f);
    shopNameField  = ui.makeInput("Shop name",          400.f,386.f,480.f,44.f);
    registerBtn    = ui.makeConfirmButton  ("Register", 400.f,448.f,480.f,44.f);
    backBtn        = ui.makeSecondaryButton("Back to Login",400.f,504.f,480.f,38.f);
}
void RegisterScreen::onEnter(){
    nameField.clear(); emailField.clear(); passwordField.clear();
    addressField.clear(); shopNameField.clear(); isSellerTab=false;
}
void RegisterScreen::handleEvent(const sf::Event& e, sf::RenderWindow&){
    nameField.handleEvent(e); emailField.handleEvent(e); passwordField.handleEvent(e);
    addressField.handleEvent(e); shopNameField.handleEvent(e);
    if(e.type==sf::Event::MouseButtonReleased&&e.mouseButton.button==sf::Mouse::Left){
        sf::Vector2f mp((float)e.mouseButton.x,(float)e.mouseButton.y);
        if(customerTabBtn.contains(mp)) isSellerTab=false;
        if(sellerTabBtn.contains(mp))   isSellerTab=true;
        if(registerBtn.contains(mp))    tryRegister();
        if(backBtn.contains(mp))        nextScreen="login";
    }
}
void RegisterScreen::tryRegister(){
    bool ok=isSellerTab
        ? platform.registerSeller(nameField.content,emailField.content,passwordField.content,shopNameField.content)
        : platform.registerCustomer(nameField.content,emailField.content,passwordField.content,addressField.content);
    note.show(ok?(isSellerTab?"Registered! Await admin approval.":"Account created! Please log in.")
                :"Email already in use.",ok,ui.fontReg);
}
void RegisterScreen::update(float dt, sf::RenderWindow& w){
    sf::Vector2f m=static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    nameField.update(dt); emailField.update(dt); passwordField.update(dt);
    addressField.update(dt); shopNameField.update(dt);
    customerTabBtn.update(m); sellerTabBtn.update(m);
    registerBtn.update(m); backBtn.update(m); note.update(dt);
}
void RegisterScreen::render(sf::RenderWindow& w, bool /*clicked*/, sf::Vector2f /*mouse*/){
    auto card=ui.makeCard(380.f,80.f,520.f,480.f); w.draw(card);
    auto t=ui.makeText("Create Account",24,Colors::Primary,true);
    t.setPosition(400.f,94.f); w.draw(t);
    customerTabBtn.isActive=!isSellerTab; sellerTabBtn.isActive=isSellerTab;
    customerTabBtn.draw(w); sellerTabBtn.draw(w);
    nameField.draw(w); emailField.draw(w); passwordField.draw(w);
    if(!isSellerTab) addressField.draw(w); else shopNameField.draw(w);
    registerBtn.draw(w); backBtn.draw(w); note.draw(w);
}
