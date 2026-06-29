#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
#include <vector>
class OrderScreen : public Screen {
public:
    OrderScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    ScrollPanel orderList;
    std::vector<Order> orders;
    sf::RectangleShape detailCard, reviewCard;
    sf::Text detailOrderId, detailDate, detailStatus, detailItems, detailTotal;
    InputField reviewComment, ratingField;
    Button submitReviewBtn;
    Notification note;
    std::vector<NavItem> nav;
    void refreshOrders();
    void showOrderDetail(const Order& o);
    void submitReview();
};
