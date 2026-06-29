#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
#include <vector>
#include <string>

class ProductListScreen : public Screen {
public:
    ProductListScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;

    InputField searchField, qtyField;

    // Fixed filter buttons (All, Electronics, Food, Clothing)
    Button searchBtn;
    Button filterAllBtn, filterElecBtn, filterFoodBtn, filterClothBtn;
    // Dynamic filter buttons for custom categories
    std::vector<Button>      customFilterBtns;
    std::vector<std::string> customCategories;

    ScrollPanel productList;
    std::vector<Product*> allProducts, filtered;

    sf::RectangleShape detailCard;
    sf::Text detailTitle, detailPrice, detailStock, detailRating, detailExtra;
    Button addToCartBtn;

    Notification note;
    std::string activeFilter = "All";
    std::vector<NavItem> nav;

    void loadCustomCategories();
    void buildFilterButtons();
    void clearAllFilterActive();
    void applyFilter();
    void refreshList();
    void showProductDetail(Product* p);
    void addSelectedToCart();
};
