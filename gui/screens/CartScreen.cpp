#include "../include/screens/CartScreen.hpp"
#include <sstream>
#include <iomanip>

CartScreen::CartScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Dashboard",       "customer", "browse"},
        {"Browse Products", "products", "browse"},
        {"My Cart",         "cart",     "cart"},
        {"My Orders",       "orders",   "orders"},
    };

    const float lx = L::CX + 12.f;
    const float ly = L::CY + 10.f;
    const float lw = 650.f;
    const float lh = L::CH - 100.f;

    cartList.bounds     = {lx, ly, lw, lh};
    cartList.itemHeight = 68.f;
    cartList.emptyLabel = "Your cart is empty. Browse products to add items.";

    const float by = ly + lh + 10.f;
    discountField    = ui.makeInput("Discount code",  lx,       by, 240.f, 40.f);
    applyDiscountBtn = ui.makeSecondaryButton("Apply", lx+248.f, by, 100.f, 40.f);
    removeItemBtn    = ui.makeDangerButton("Remove",   lx+356.f, by, 120.f, 40.f);

    const float sx = L::CX + 680.f;
    const float sw = L::CW - 692.f;
    summaryCard  = ui.makeCard(sx, ly, sw, 240.f);
    subtotalText = ui.makeText("", 15, Colors::TextSecondary); subtotalText.setPosition(sx+18.f, ly+20.f);
    discountText = ui.makeText("", 15, Colors::Success);       discountText.setPosition(sx+18.f, ly+54.f);
    totalText    = ui.makeText("", 20, Colors::Primary, true); totalText.setPosition(sx+18.f,   ly+100.f);

    checkoutBtn = ui.makeConfirmButton("Checkout & Place Order", sx, ly+260.f, sw, 48.f);
}

void CartScreen::onEnter() { refreshCart(); }

void CartScreen::refreshCart() {
    Cart& cart = platform.getCart(platform.getActiveUserId());
    std::vector<ListItem> items;
    for (const auto& ci : cart.getItems()) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Rs." << ci.unitPrice << " x" << ci.quantity << "  =  Rs." << ci.subtotal;
        ListItem li;
        li.id = std::to_string(ci.productId); li.line1 = ci.productName; li.line2 = ss.str();
        items.push_back(li);
    }
    cartList.setItems(items);

    std::ostringstream sub, disc, tot;
    sub  << std::fixed << std::setprecision(2) << "Subtotal :   Rs." << cart.getSubtotal();
    disc << std::fixed << std::setprecision(2) << "Discount :  -Rs." << cart.getDiscountAmount();
    tot  << std::fixed << std::setprecision(2) << "TOTAL    :   Rs." << cart.getTotal();
    subtotalText.setString(sub.str());
    discountText.setString(cart.getDiscountAmount() > 0 ? disc.str() : "");
    totalText.setString(tot.str());
}

void CartScreen::removeSelected() {
    int idx = cartList.getSelectedIndex(); if (idx < 0) return;
    Cart& cart = platform.getCart(platform.getActiveUserId());
    auto items = cart.getItems();
    if (idx < (int)items.size()) { cart.removeItem(items[idx].productId); refreshCart(); }
}

void CartScreen::checkout() {
    Cart& cart = platform.getCart(platform.getActiveUserId());
    if (cart.getItems().empty()) { note.show("Your cart is empty.", false, ui.fontReg); return; }
    if (platform.placeOrder(platform.getActiveUserId(), cart)) {
        note.show("Order placed! Receipt saved to data/", true, ui.fontReg);
        refreshCart();
    } else {
        note.show("Checkout failed. Check stock.", false, ui.fontReg);
    }
}

void CartScreen::handleEvent(const sf::Event& e, sf::RenderWindow& w) {
    discountField.handleEvent(e);
    cartList.handleEvent(e, w);
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);
        if (applyDiscountBtn.contains(mp)) {
            Cart& cart = platform.getCart(platform.getActiveUserId());
            platform.applyDiscountCode(cart, discountField.content); refreshCart();
        }
        if (removeItemBtn.contains(mp)) removeSelected();
        if (checkoutBtn.contains(mp))   checkout();
    }
}

void CartScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    discountField.update(dt);
    applyDiscountBtn.update(m); removeItemBtn.update(m);
    checkoutBtn.update(m); note.update(dt);
}

void CartScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    auto* c = platform.getActiveCustomer();
    ui.drawTopBar(w, "SwiftCart", c ? c->getName() + "  |  Customer" : "");
    ui.drawBreadcrumb(w, "Home > My Cart");

    cartList.draw(w, ui.fontReg);
    discountField.draw(w); applyDiscountBtn.draw(w); removeItemBtn.draw(w);
    w.draw(summaryCard);
    w.draw(subtotalText); w.draw(discountText); w.draw(totalText);
    checkoutBtn.draw(w);
    note.draw(w);

    std::string sc = ui.drawSidebar(w, c ? c->getName() : "User", "Customer",
                                    nav, "cart", mouse, clicked);
    if (sc == "logout") { platform.logout(); nextScreen = "login"; }
    else if (!sc.empty()) nextScreen = sc;
}
