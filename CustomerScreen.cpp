#include "../include/screens/CustomerScreen.hpp"

CustomerScreen::CustomerScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Dashboard",       "customer",  "browse"},
        {"Browse Products", "products",  "browse"},
        {"My Cart",         "cart",      "cart"},
        {"My Orders",       "orders",    "orders"},
        {"My Profile",      "profile_",  "profile"},
    };
    statsCard = ui.makeCard(L::CX+20.f, L::CY+70.f, L::CW-40.f, 220.f);
}

void CustomerScreen::onEnter() {
    showProfile = false;
    auto* c = platform.getActiveCustomer();
    welcomeText = ui.makeText(
        c ? "Welcome back, " + c->getName() + "!" : "Welcome!",
        26, Colors::TextPrimary, true);
    welcomeText.setPosition(L::CX+28.f, L::CY+22.f);

    auto& cart  = platform.getCart(platform.getActiveUserId());
    auto  ords  = platform.getCustomerOrders(platform.getActiveUserId());
    statOrders    = ui.makeText("Orders placed :   " + std::to_string(ords.size()),  18, Colors::TextPrimary);
    statCartItems = ui.makeText("Items in cart :   " + std::to_string(cart.getItems().size()), 18, Colors::TextPrimary);
    statOrders.setPosition(L::CX+38.f,    L::CY+100.f);
    statCartItems.setPosition(L::CX+38.f, L::CY+138.f);

    if (c) {
        profileInfo = ui.makeText(
            "Name      :   " + c->getName()    + "\n\n"
            "Email     :   " + c->getEmail()   + "\n\n"
            "Address   :   " + c->getAddress(),
            16, Colors::TextSecondary);
        profileInfo.setPosition(L::CX+38.f, L::CY+100.f);
    }
}

void CustomerScreen::handleEvent(const sf::Event&, sf::RenderWindow&) {}

void CustomerScreen::update(float, sf::RenderWindow&) {}

void CustomerScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    auto* c = platform.getActiveCustomer();
    std::string rightInfo = c ? c->getName() + "  |  Customer" : "Customer";

    ui.drawTopBar(w, "SwiftCart", rightInfo);
    ui.drawBreadcrumb(w, showProfile ? "Home > My Profile" : "Home > Dashboard");

    w.draw(welcomeText);
    w.draw(statsCard);

    if (showProfile) {
        auto title = ui.makeText("My Profile", 20, Colors::Primary, true);
        title.setPosition(L::CX+38.f, L::CY+80.f); w.draw(title);
        w.draw(profileInfo);
    } else {
        w.draw(statOrders);
        w.draw(statCartItems);
        auto hint = ui.makeText("Use the sidebar to navigate.", 14, Colors::TextSecondary);
        hint.setPosition(L::CX+38.f, L::CY+188.f); w.draw(hint);
    }

    // Sidebar last (renders on top)
    std::string sc = ui.drawSidebar(w,
        c ? c->getName() : "User", "Customer",
        nav,
        showProfile ? "profile_" : "customer",
        mouse, clicked);

    if (sc == "logout")    { platform.logout(); nextScreen = "login"; }
    else if (sc == "profile_") showProfile = true;
    else if (sc == "customer") showProfile = false;
    else if (!sc.empty())  nextScreen = sc;
}
