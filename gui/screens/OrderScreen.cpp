#include "../include/screens/OrderScreen.hpp"
#include <sstream>
#include <iomanip>

OrderScreen::OrderScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Dashboard",       "customer", "browse"},
        {"Browse Products", "products", "browse"},
        {"My Cart",         "cart",     "cart"},
        {"My Orders",       "orders",   "orders"},
    };

    const float lx = L::CX + 12.f, ly = L::CY + 10.f;
    const float lw = 490.f, lh = L::CH - 20.f;
    orderList.bounds     = {lx, ly, lw, lh};
    orderList.itemHeight = 76.f;
    orderList.emptyLabel = "No orders yet. Place your first order!";

    const float dx = lx + lw + 12.f;
    const float dw = L::CW - lw - 36.f;
    detailCard   = ui.makeCard(dx, ly, dw, 330.f);
    detailOrderId= ui.makeText("Select an order", 17, Colors::TextPrimary, true); detailOrderId.setPosition(dx+16.f, ly+16.f);
    detailDate   = ui.makeText("", 13, Colors::TextSecondary); detailDate.setPosition(dx+16.f,  ly+48.f);
    detailStatus = ui.makeText("", 15, Colors::Primary, true); detailStatus.setPosition(dx+16.f, ly+72.f);
    detailItems  = ui.makeText("", 13, Colors::TextSecondary); detailItems.setPosition(dx+16.f, ly+108.f);
    detailTotal  = ui.makeText("", 18, Colors::Primary, true); detailTotal.setPosition(dx+16.f, ly+282.f);

    reviewCard    = ui.makeCard(dx, ly+342.f, dw, 182.f);
    auto rl = ui.makeText("Leave a review:", 13, Colors::TextSecondary);
    ratingField   = ui.makeInput("1-5", dx+16.f, ly+378.f, 80.f, 38.f);
    reviewComment = ui.makeInput("Write your comment...", dx+106.f, ly+378.f, dw-122.f, 38.f);
    submitReviewBtn = ui.makeConfirmButton("Submit Review", dx+16.f, ly+430.f, dw-32.f, 40.f);
}

void OrderScreen::onEnter() { refreshOrders(); }

void OrderScreen::refreshOrders() {
    orders = platform.getCustomerOrders(platform.getActiveUserId());
    std::vector<ListItem> items;
    for (const auto& o : orders) {
        std::ostringstream ss; ss << std::fixed << std::setprecision(2) << "Rs." << o.getTotalAmount();
        ListItem li;
        li.id    = std::to_string(o.getOrderId());
        li.line1 = "Order #" + std::to_string(o.getOrderId()) + "  —  " + ss.str();
        li.line2 = o.getOrderDate();
        li.badge = o.getStatusStr();
        li.badgeColor = o.getStatus()==Order::DELIVERED ? Colors::Success :
                        o.getStatus()==Order::CANCELLED ? Colors::Danger  : Colors::Warning;
        items.push_back(li);
    }
    orderList.setItems(items);
}

void OrderScreen::showOrderDetail(const Order& o) {
    detailOrderId.setString("Order #" + std::to_string(o.getOrderId()));
    detailDate.setString("Date: " + o.getOrderDate());
    detailStatus.setString("Status: " + o.getStatusStr());
    std::string s;
    for (const auto& item : o.getItems())
        s += item.productName + "  x" + std::to_string(item.quantity) + "\n";
    detailItems.setString(s);
    std::ostringstream tot; tot << std::fixed << std::setprecision(2) << "Total: Rs." << o.getTotalAmount();
    detailTotal.setString(tot.str());
}

void OrderScreen::submitReview() {
    int idx = orderList.getSelectedIndex();
    if (idx < 0 || idx >= (int)orders.size()) { note.show("Select an order first.", false, ui.fontReg); return; }
    int rating = 0;
    try { rating = std::stoi(ratingField.content); } catch (...) {}
    if (rating < 1 || rating > 5) { note.show("Enter a rating between 1 and 5.", false, ui.fontReg); return; }
    auto* c = platform.getActiveCustomer(); if (!c) return;
    const Order& o = orders[idx]; if (o.getItems().empty()) return;
    bool ok = platform.submitReview(c->getId(), c->getName(),
                                    o.getItems()[0].productId, rating, reviewComment.content);
    note.show(ok ? "Review submitted!" : "You can only review purchased products.",
              ok, ui.fontReg);
    reviewComment.clear(); ratingField.clear();
}

void OrderScreen::handleEvent(const sf::Event& e, sf::RenderWindow& w) {
    orderList.handleEvent(e, w);
    reviewComment.handleEvent(e); ratingField.handleEvent(e);
    int idx = orderList.getSelectedIndex();
    if (idx >= 0 && idx < (int)orders.size()) showOrderDetail(orders[idx]);
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);
        if (submitReviewBtn.contains(mp)) submitReview();
    }
}

void OrderScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    reviewComment.update(dt); ratingField.update(dt);
    submitReviewBtn.update(m); note.update(dt);
}

void OrderScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    auto* c = platform.getActiveCustomer();
    ui.drawTopBar(w, "SwiftCart", c ? c->getName() + "  |  Customer" : "");
    ui.drawBreadcrumb(w, "Home > My Orders");

    orderList.draw(w, ui.fontReg);
    w.draw(detailCard);
    w.draw(detailOrderId); w.draw(detailDate); w.draw(detailStatus);
    w.draw(detailItems);   w.draw(detailTotal);
    w.draw(reviewCard);
    auto rl = ui.makeText("Leave a review:", 13, Colors::TextSecondary);
    rl.setPosition(orderList.bounds.left+orderList.bounds.width+28.f,
                   L::CY+350.f);
    w.draw(rl);
    ratingField.draw(w); reviewComment.draw(w); submitReviewBtn.draw(w);
    note.draw(w);

    std::string sc = ui.drawSidebar(w, c ? c->getName() : "User", "Customer",
                                    nav, "orders", mouse, clicked);
    if (sc == "logout") { platform.logout(); nextScreen = "login"; }
    else if (!sc.empty()) nextScreen = sc;
}
