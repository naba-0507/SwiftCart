#pragma once
#include "../Screen.hpp"
#include "../UIManager.hpp"
#include "../../../include/Platform.h"
#include <vector>
#include <string>

class SellerScreen : public Screen {
public:
    SellerScreen(Platform& p, UIManager& ui);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w) override;
    void update(float dt, sf::RenderWindow& w) override;
    void render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) override;
    void onEnter() override;
private:
    Platform& platform; UIManager& ui;
    enum class Tab { HOME, PRODUCTS, ORDERS, ADD_PRODUCT };
    Tab activeTab = Tab::HOME;

    // Tab buttons
    Button homeTabBtn, productsTabBtn, ordersTabBtn, addProductTabBtn;

    // Home/dashboard tab
    sf::Text welcomeText, homeStatProducts, homeStatOrders;
    sf::RectangleShape homeCard;
    sf::Text profileInfo;

    // Products tab
    ScrollPanel productList;
    Button editPriceBtn, deleteProductBtn;
    InputField editPriceField, editStockField;

    // Orders tab
    ScrollPanel orderList;
    Button confirmBtn, shipBtn, deliverBtn, cancelBtn, reportBtn;

    // Add product tab
    // Built-in types
    Button typeElecBtn, typePerishBtn, typeClothBtn;
    // Custom types (loaded from file + seller can add new ones)
    std::vector<std::string> customTypes;   // names of seller-added categories
    std::vector<Button>      customTypeBtns;
    InputField newCategoryField;            // field to type a new category name
    Button addCategoryBtn;                  // button to create the new category
    std::string selectedType = "Electronic";

    InputField nameField, priceField, stockField, extra1Field, extra2Field;
    Button addProductBtn;

    Notification note;
    std::vector<NavItem> nav;

    void renderHomeTab(sf::RenderWindow& w);
    void renderProductsTab(sf::RenderWindow& w);
    void renderOrdersTab(sf::RenderWindow& w);
    void renderAddProductTab(sf::RenderWindow& w);
    void tryAddProduct();
    void refreshProducts();
    void refreshOrders();
    void loadCustomCategories();
    void saveCustomCategories();
    void rebuildCustomTypeBtns();
    void setTypeActive(const std::string& type);
};
