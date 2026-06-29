#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
class AdminScreen : public Screen {
public:
    AdminScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    enum class Tab { SELLERS, DISCOUNTS, REVIEWS, STATS };
    Tab activeTab = Tab::SELLERS;
    Button sellersTabBtn, discountsTabBtn, reviewsTabBtn, statsTabBtn;
    ScrollPanel sellerList, discountList, reviewList;
    Button approveBtn, banBtn;
    InputField codeField, valueField, minOrderField;
    Button typePercBtn, typeFlatBtn, addCodeBtn, deactivateCodeBtn;
    bool isPercent = true;
    Button flagReviewBtn, deleteReviewBtn;
    sf::RectangleShape statsCard;
    sf::Text statCustomers, statSellers, statProducts, statOrders, statRevenue;
    Notification note;
    std::vector<NavItem> nav;
    void renderSellersTab(sf::RenderWindow& w);
    void renderDiscountsTab(sf::RenderWindow& w);
    void renderReviewsTab(sf::RenderWindow& w);
    void renderStatsTab(sf::RenderWindow& w);
    void refreshAll();
};
