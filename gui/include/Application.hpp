#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include "Screen.hpp"
#include "UIManager.hpp"
#include "../../include/Platform.h"

class Application {
public:
    Application();
    void run();
private:
    sf::RenderWindow window;
    Platform         platform;
    UIManager        ui;
    std::unordered_map<std::string,std::unique_ptr<Screen>> screens;
    Screen* current = nullptr;
    bool mouseJustUp = false;  // true exactly one frame when LMB released
    void reg();
    void go(const std::string& k);
    void events();
    void tick(float dt);
    void draw();
};
