#include "include/Application.hpp"
#include "include/AllScreens.hpp"
#include <iostream>

Application::Application()
    : window(sf::VideoMode(1280,720),"SwiftCart",sf::Style::Titlebar|sf::Style::Close)
{
    window.setFramerateLimit(60);
    reg();
    go("login");
}

void Application::reg(){
    screens["login"]    = std::make_unique<LoginScreen>(platform,ui);
    screens["register"] = std::make_unique<RegisterScreen>(platform,ui);
    screens["customer"] = std::make_unique<CustomerScreen>(platform,ui);
    screens["products"] = std::make_unique<ProductListScreen>(platform,ui);
    screens["cart"]     = std::make_unique<CartScreen>(platform,ui);
    screens["orders"]   = std::make_unique<OrderScreen>(platform,ui);
    screens["seller"]   = std::make_unique<SellerScreen>(platform,ui);
    screens["admin"]    = std::make_unique<AdminScreen>(platform,ui);
}

void Application::go(const std::string& k){
    if(current) current->onExit();
    auto it=screens.find(k);
    if(it==screens.end()){std::cerr<<"[App] no screen: "<<k<<"\n";return;}
    current=it->second.get();
    current->nextScreen.clear();
    current->onEnter();
}

void Application::run(){
    sf::Clock clk;
    while(window.isOpen()){
        float dt=clk.restart().asSeconds();
        mouseJustUp=false;
        events();
        if(!current) continue;
        if(!current->nextScreen.empty()){
            std::string dest=current->nextScreen;
            if(dest=="exit"){window.close();break;}
            if(dest=="logout"){platform.logout();go("login");continue;}
            go(dest); continue;
        }
        tick(dt); draw();
    }
}

void Application::events(){
    sf::Event e;
    while(window.pollEvent(e)){
        if(e.type==sf::Event::Closed){window.close();return;}
        if(e.type==sf::Event::MouseButtonReleased&&e.mouseButton.button==sf::Mouse::Left)
            mouseJustUp=true;
        if(current) current->handleEvent(e,window);
    }
}

void Application::tick(float dt){
    if(current) current->update(dt,window);
}

void Application::draw(){
    window.clear(Colors::Background);
    if(current){
        sf::Vector2f mouse=static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        current->render(window,mouseJustUp,mouse);
    }
    window.display();
}
