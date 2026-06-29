#include "../include/screens/ProductListScreen.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>

// ── Layout constants ─────────────────────────────────────────
// Content: x=200(CX), y=86(CY), w=1080(CW), h=634(CH)
// Filter row height: 46px
// List panel: x=CX+10, y=CY+54, w=540, h=CH-62
// Detail panel: x=CX+562, y=CY+8, w=510, h=CH-16
static constexpr float LX = L::CX + 10.f;
static constexpr float LY = L::CY + 94.f;   // below two filter rows (8+36+6+34+10)
static constexpr float LW = 540.f;
static constexpr float LH = L::CH - 102.f;  // reduced for two filter rows
static constexpr float DX = L::CX + 562.f;
static constexpr float DW = L::WW - (L::CX + 562.f) - 8.f; // 510
static constexpr float DY = L::CY + 94.f;   // aligned with list panel top
static constexpr float DH = L::CH - 102.f;  // matches list panel height

ProductListScreen::ProductListScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Dashboard",       "customer",  "browse"},
        {"Browse Products", "products",  "search"},
        {"My Cart",         "cart",      "cart"},
        {"My Orders",       "orders",    "orders"},
    };

    // Row 1: search
    const float fy1 = L::CY + 8.f;
    searchField = ui.makeInput("Search...", LX, fy1, 340.f, 36.f);
    searchBtn   = ui.makePrimaryButton("Search", LX + 348.f, fy1, 82.f, 36.f);

    // Row 2: category filter buttons (all secondary so isActive drives highlight)
    const float fy2 = L::CY + 52.f;
    filterAllBtn   = ui.makeSecondaryButton("All",         LX,         fy2,  52.f, 34.f);
    filterElecBtn  = ui.makeSecondaryButton("Electronics", LX + 58.f,  fy2, 108.f, 34.f);
    filterFoodBtn  = ui.makeSecondaryButton("Food",        LX + 172.f, fy2,  62.f, 34.f);
    filterClothBtn = ui.makeSecondaryButton("Clothing",    LX + 240.f, fy2,  86.f, 34.f);
    // Custom filter buttons built in onEnter() — placed after Clothing on same row

    // List panel starts below both filter rows
    productList.bounds     = {LX, LY, LW, LH};
    productList.itemHeight = 72.f;
    productList.emptyLabel = "No products found. Try a different search or filter.";

    // Detail panel — fits entirely within the window
    detailCard = ui.makeCard(DX, DY, DW, DH);

    detailTitle = ui.makeText("Select a product", 17, Colors::TextPrimary, true);
    detailTitle.setPosition(DX + 16.f, DY + 16.f);

    detailPrice = ui.makeText("", 20, Colors::Primary, true);
    detailPrice.setPosition(DX + 16.f, DY + 52.f);

    detailStock = ui.makeText("", 13, Colors::TextSecondary);
    detailStock.setPosition(DX + 16.f, DY + 88.f);

    detailRating = ui.makeText("", 13, Colors::Warning);
    detailRating.setPosition(DX + 16.f, DY + 112.f);

    detailExtra = ui.makeText("", 12, Colors::TextSecondary);
    detailExtra.setPosition(DX + 16.f, DY + 140.f);

    // Add-to-cart row pinned at bottom of detail card
    const float ay = DY + DH - 56.f;
    qtyField     = ui.makeInput("Qty", DX + 16.f, ay, 68.f, 40.f);
    addToCartBtn = ui.makeConfirmButton("Add to Cart", DX + 92.f, ay, DW - 108.f, 40.f);
}

// ── Load custom categories from file ─────────────────────────
void ProductListScreen::loadCustomCategories() {
    customCategories.clear();
    std::ifstream f("data/custom_categories.txt");
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) customCategories.push_back(line);
}

// ── Build dynamic filter buttons for custom categories ────────
void ProductListScreen::buildFilterButtons() {
    customFilterBtns.clear();
    // Row 2 (y=CY+52): All | Electronics | Food | Clothing | [custom buttons continue here]
    // Clothing ends at LX + 240 + 86 = LX + 326
    const float fy2 = L::CY + 52.f;
    float bx = LX + 334.f; // 8px gap after Clothing button
    for (const auto& cat : customCategories) {
        // Width proportional to label length, minimum 60px
        float bw = std::max(60.f, (float)cat.size() * 9.f + 18.f);
        // Wrap to next line if it would overflow the list panel right edge
        if (bx + bw > LX + LW) {
            // Just skip overflow ones — the list panel is 540px wide so we have ~200px for customs
            break;
        }
        Button b = ui.makeSecondaryButton(cat, bx, fy2, bw, 34.f);
        customFilterBtns.push_back(b);
        bx += bw + 6.f;
    }
}

void ProductListScreen::clearAllFilterActive() {
    filterAllBtn.isActive = filterElecBtn.isActive =
    filterFoodBtn.isActive = filterClothBtn.isActive = false;
    for (auto& b : customFilterBtns) b.isActive = false;
}

void ProductListScreen::onEnter() {
    loadCustomCategories();
    buildFilterButtons();

    allProducts = platform.getAllProducts();
    filtered    = allProducts;
    refreshList();

    // "All" starts active
    clearAllFilterActive();
    filterAllBtn.isActive = true;
    activeFilter = "All";
}

void ProductListScreen::refreshList() {
    std::vector<ListItem> items;
    for (auto* p : filtered) {
        std::ostringstream pr;
        pr << std::fixed << std::setprecision(2) << "Rs." << p->getPrice();
        ListItem li;
        li.id    = std::to_string(p->getId());
        li.line1 = p->getName();
        li.line2 = pr.str() + "  |  " + p->getCategory();
        li.badge = p->getStock() > 0 ? "In Stock" : "Out";
        li.badgeColor = p->getStock() > 0 ? Colors::Success : Colors::Danger;
        items.push_back(li);
    }
    productList.setItems(items);
}

void ProductListScreen::applyFilter() {
    filtered.clear();
    std::string kw = searchField.content;
    std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);
    for (auto* p : allProducts) {
        if (activeFilter != "All" && p->getCategory() != activeFilter) continue;
        if (!kw.empty()) {
            std::string name = p->getName();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (name.find(kw) == std::string::npos) continue;
        }
        filtered.push_back(p);
    }
    refreshList();
}

void ProductListScreen::showProductDetail(Product* p) {
    if (!p) return;
    detailTitle.setString(p->getName());
    std::ostringstream pr;
    pr << std::fixed << std::setprecision(2) << "Rs." << p->getPrice();
    detailPrice.setString(pr.str());
    detailStock.setString("Stock: " + std::to_string(p->getStock()) + " units");
    std::ostringstream rt;
    rt << std::fixed << std::setprecision(1) << p->getAvgRating();
    detailRating.setString("Rating: " + rt.str() + "/5  ("
                           + std::to_string(p->getReviewCount()) + " reviews)");
    detailExtra.setString("Type: " + p->getType());
}

void ProductListScreen::addSelectedToCart() {
    int idx = productList.getSelectedIndex();
    if (idx < 0 || idx >= (int)filtered.size()) {
        note.show("Select a product first.", false, ui.fontReg); return;
    }
    Product* p = filtered[idx];
    int qty = 1;
    try { qty = std::stoi(qtyField.content); } catch (...) { qty = 1; }
    if (qty <= 0 || qty > p->getStock()) {
        note.show("Invalid quantity or insufficient stock.", false, ui.fontReg); return;
    }
    Cart& cart = platform.getCart(platform.getActiveUserId());
    cart.addItem(p->getId(), p->getName(), p->getPrice(), qty);
    note.show("Added to cart: " + p->getName(), true, ui.fontReg);
}

void ProductListScreen::handleEvent(const sf::Event& e, sf::RenderWindow& w) {
    searchField.handleEvent(e);
    qtyField.handleEvent(e);
    productList.handleEvent(e, w);

    // Update detail panel on selection
    int idx = productList.getSelectedIndex();
    if (idx >= 0 && idx < (int)filtered.size()) showProductDetail(filtered[idx]);

    if (e.type == sf::Event::MouseButtonReleased
        && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);

        if (searchBtn.contains(mp)) applyFilter();

        if (filterAllBtn.contains(mp)) {
            clearAllFilterActive(); filterAllBtn.isActive = true;
            activeFilter = "All"; applyFilter();
        }
        if (filterElecBtn.contains(mp)) {
            clearAllFilterActive(); filterElecBtn.isActive = true;
            activeFilter = "Electronics"; applyFilter();
        }
        if (filterFoodBtn.contains(mp)) {
            clearAllFilterActive(); filterFoodBtn.isActive = true;
            activeFilter = "Perishable"; applyFilter();
        }
        if (filterClothBtn.contains(mp)) {
            clearAllFilterActive(); filterClothBtn.isActive = true;
            activeFilter = "Clothing"; applyFilter();
        }
        for (size_t i = 0; i < customFilterBtns.size(); i++) {
            if (customFilterBtns[i].contains(mp)) {
                clearAllFilterActive();
                customFilterBtns[i].isActive = true;
                activeFilter = customCategories[i];
                applyFilter();
            }
        }

        if (addToCartBtn.contains(mp)) addSelectedToCart();
    }
}

void ProductListScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    searchField.update(dt);
    qtyField.update(dt);
    searchBtn.update(m);
    filterAllBtn.update(m);
    filterElecBtn.update(m);
    filterFoodBtn.update(m);
    filterClothBtn.update(m);
    for (auto& b : customFilterBtns) b.update(m);
    addToCartBtn.update(m);
    note.update(dt);
}

void ProductListScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    auto* c = platform.getActiveCustomer();
    ui.drawTopBar(w, "SwiftCart", c ? c->getName() + "  |  Customer" : "");
    ui.drawBreadcrumb(w,
        "Home > Browse Products  —  "
        + std::to_string(filtered.size()) + " results");

    // Row 1: search
    searchField.draw(w); searchBtn.draw(w);
    // Row 2: category filters
    filterAllBtn.draw(w); filterElecBtn.draw(w);
    filterFoodBtn.draw(w); filterClothBtn.draw(w);
    for (auto& b : customFilterBtns) b.draw(w);

    // List + detail
    productList.draw(w, ui.fontReg);
    w.draw(detailCard);
    w.draw(detailTitle); w.draw(detailPrice); w.draw(detailStock);
    w.draw(detailRating); w.draw(detailExtra);
    qtyField.draw(w); addToCartBtn.draw(w);

    note.draw(w);

    // Sidebar last (renders on top)
    std::string sc = ui.drawSidebar(
        w, c ? c->getName() : "User", "Customer",
        nav, "products", mouse, clicked);
    if (sc == "logout") { platform.logout(); nextScreen = "login"; }
    else if (!sc.empty()) nextScreen = sc;
}
