#include "../include/screens/SellerScreen.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>

SellerScreen::SellerScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Dashboard",    "home_tab",     "browse"},
        {"My Products",  "products_tab", "products"},
        {"Orders",       "orders_tab",   "orders"},
        {"Add Product",  "add_tab",      "add"},
    };

    const float ty = L::CY + 8.f;
    // ALL tabs start as secondary so isActive visually drives highlight
    homeTabBtn       = ui.makeSecondaryButton("Dashboard",   L::CX+12.f,  ty, 118.f, 38.f);
    productsTabBtn   = ui.makeSecondaryButton("My Products", L::CX+138.f, ty, 130.f, 38.f);
    ordersTabBtn     = ui.makeSecondaryButton("Orders",      L::CX+276.f, ty, 100.f, 38.f);
    addProductTabBtn = ui.makeSecondaryButton("Add Product", L::CX+384.f, ty, 122.f, 38.f);

    // Home tab
    homeCard     = ui.makeCard(L::CX+12.f, L::CY+56.f, L::CW-24.f, 240.f);
    welcomeText  = ui.makeText("", 24, Colors::TextPrimary, true);
    welcomeText.setPosition(L::CX+28.f, L::CY+72.f);
    homeStatProducts = ui.makeText("", 17, Colors::TextPrimary);
    homeStatOrders   = ui.makeText("", 17, Colors::TextPrimary);
    homeStatProducts.setPosition(L::CX+28.f, L::CY+120.f);
    homeStatOrders.setPosition(L::CX+28.f,   L::CY+154.f);
    profileInfo  = ui.makeText("", 15, Colors::TextSecondary);
    profileInfo.setPosition(L::CX+28.f, L::CY+320.f);

    // Products tab
    productList.bounds    = {L::CX+12.f, L::CY+56.f, L::CW-24.f, 450.f};
    productList.itemHeight = 68.f;
    productList.emptyLabel = "No products listed yet. Use 'Add Product' to list your first.";
    const float pb = L::CY + 516.f;
    editPriceField   = ui.makeInput("New price",  L::CX+12.f,  pb, 148.f, 38.f);
    editStockField   = ui.makeInput("New stock",  L::CX+168.f, pb, 110.f, 38.f);
    editPriceBtn     = ui.makePrimaryButton("Update", L::CX+286.f, pb,  96.f, 38.f);
    deleteProductBtn = ui.makeDangerButton ("Delete", L::CX+390.f, pb,  96.f, 38.f);

    // Orders tab
    orderList.bounds    = {L::CX+12.f, L::CY+56.f, L::CW-24.f, 450.f};
    orderList.itemHeight = 68.f;
    orderList.emptyLabel = "No orders received yet.";
    confirmBtn = ui.makeConfirmButton ("Confirm",    L::CX+12.f,  pb, 108.f, 38.f);
    shipBtn    = ui.makeConfirmButton ("Shipped",    L::CX+128.f, pb, 108.f, 38.f);
    deliverBtn = ui.makeConfirmButton ("Delivered",  L::CX+244.f, pb, 108.f, 38.f);
    cancelBtn  = ui.makeDangerButton  ("Cancel",     L::CX+360.f, pb,  96.f, 38.f);
    reportBtn  = ui.makeSecondaryButton("Save Report",L::CX+464.f,pb, 130.f, 38.f);

    // Add product tab — built-in type buttons (all secondary)
    const float fy = L::CY + 56.f;
    typeElecBtn   = ui.makeSecondaryButton("Electronic", L::CX+12.f,  fy, 120.f, 36.f);
    typePerishBtn = ui.makeSecondaryButton("Food",       L::CX+140.f, fy, 100.f, 36.f);
    typeClothBtn  = ui.makeSecondaryButton("Clothing",   L::CX+248.f, fy, 100.f, 36.f);
    // new category input
    newCategoryField = ui.makeInput("New category name...", L::CX+12.f, fy+44.f, 220.f, 36.f);
    addCategoryBtn   = ui.makePrimaryButton("+ Add",         L::CX+240.f,fy+44.f,  80.f, 36.f);

    // Product fields
    nameField     = ui.makeInput("Product name",      L::CX+12.f,  fy+96.f,  480.f, 44.f);
    priceField    = ui.makeInput("Price (Rs.)",        L::CX+12.f,  fy+158.f, 232.f, 44.f);
    stockField    = ui.makeInput("Stock quantity",     L::CX+256.f, fy+158.f, 232.f, 44.f);
    extra1Field   = ui.makeInput("",                   L::CX+12.f,  fy+220.f, 232.f, 44.f);
    extra2Field   = ui.makeInput("",                   L::CX+256.f, fy+220.f, 232.f, 44.f);
    addProductBtn = ui.makeConfirmButton("List Product",L::CX+12.f, fy+284.f, 480.f, 46.f);
}

// ── Custom category file helpers ─────────────────────────────
void SellerScreen::loadCustomCategories() {
    customTypes.clear();
    std::ifstream f("data/custom_categories.txt");
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) customTypes.push_back(line);
    rebuildCustomTypeBtns();
}

void SellerScreen::saveCustomCategories() {
    std::ofstream f("data/custom_categories.txt");
    for (const auto& c : customTypes) f << c << "\n";
}

void SellerScreen::rebuildCustomTypeBtns() {
    // Custom type buttons placed right after the three built-in ones
    // Row 1 of type buttons: Electronic | Food | Clothing (+ any custom that fits)
    // We put them on a second row starting at fy+44 was new-category row
    // Re-layout: move new-category row further down
    customTypeBtns.clear();
    const float fy = L::CY + 56.f;
    // Start custom buttons after built-in row
    float bx = L::CX + 12.f;
    float by = fy; // same row, continuing after Clothing btn (x=248+100=348)
    bx = L::CX + 356.f; // right after Clothing
    for (const auto& cat : customTypes) {
        // wrap to next row if needed
        if (bx + 110.f > L::CX + L::CW - 12.f) { bx = L::CX + 12.f; by += 44.f; }
        Button b = ui.makeSecondaryButton(cat, bx, by, 110.f, 36.f);
        customTypeBtns.push_back(b);
        bx += 118.f;
    }
}

void SellerScreen::setTypeActive(const std::string& type) {
    selectedType = type;
    typeElecBtn.isActive   = (type == "Electronic");
    typePerishBtn.isActive = (type == "Food");
    typeClothBtn.isActive  = (type == "Clothing");
    for (size_t i = 0; i < customTypeBtns.size(); i++)
        customTypeBtns[i].isActive = (customTypes[i] == type);

    // Update extra field placeholders
    if      (type == "Electronic") { extra1Field.placeholder.setString("Brand");               extra2Field.placeholder.setString("Warranty (months)"); }
    else if (type == "Food")       { extra1Field.placeholder.setString("Expiry (YYYY-MM-DD)"); extra2Field.placeholder.setString("Storage condition"); }
    else if (type == "Clothing")   { extra1Field.placeholder.setString("Size (S/M/L/XL)");    extra2Field.placeholder.setString("Material"); }
    else                           { extra1Field.placeholder.setString("Attribute 1");         extra2Field.placeholder.setString("Attribute 2"); }
    extra1Field.clear(); extra2Field.clear();
}

void SellerScreen::onEnter() {
    loadCustomCategories();
    setTypeActive("Electronic");
    refreshProducts();
    refreshOrders();
    // Home tab stats
    auto* s = platform.getActiveSeller();
    welcomeText.setString(s ? "Welcome, " + s->getName() + "!" : "Welcome!");
    auto allP = platform.getAllProducts();
    int sid = platform.getActiveUserId();
    int myProducts = 0;
    for (auto* p : allP) if (p->getSellerId() == sid) myProducts++;
    auto myOrders = platform.getSellerOrders(sid);
    homeStatProducts.setString("Products listed :   " + std::to_string(myProducts));
    homeStatOrders.setString  ("Orders received :   " + std::to_string(myOrders.size()));
    if (s) {
        profileInfo.setString(
            "Name    :   " + s->getName()     + "\n\n"
            "Email   :   " + s->getEmail()    + "\n\n"
            "Shop    :   " + s->getShopName() + "\n\n"
            "Status  :   " + (s->getIsApproved() ? "Approved" : "Pending"));
    }
}

void SellerScreen::refreshProducts() {
    auto all = platform.getAllProducts();
    int sid = platform.getActiveUserId();
    std::vector<ListItem> items;
    for (auto* p : all) {
        if (p->getSellerId() != sid) continue;
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Rs." << p->getPrice() << "  |  Stock: " << p->getStock() << "  |  " << p->getType();
        ListItem li;
        li.id    = std::to_string(p->getId());
        li.line1 = p->getName();
        li.line2 = ss.str();
        li.badge = p->getStock() > 0 ? "Active" : "Out";
        li.badgeColor = p->getStock() > 0 ? Colors::Success : Colors::Danger;
        items.push_back(li);
    }
    productList.setItems(items);
}

void SellerScreen::refreshOrders() {
    auto ords = platform.getSellerOrders(platform.getActiveUserId());
    std::vector<ListItem> items;
    for (const auto& o : ords) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << "Rs." << o.getTotalAmount();
        ListItem li;
        li.id    = std::to_string(o.getOrderId());
        li.line1 = "Order #" + std::to_string(o.getOrderId()) + "  —  " + ss.str();
        li.line2 = "Customer #" + std::to_string(o.getCustomerId()) + "  |  " + o.getOrderDate();
        li.badge = o.getStatusStr();
        li.badgeColor = o.getStatus()==Order::DELIVERED ? Colors::Success :
                        o.getStatus()==Order::CANCELLED ? Colors::Danger  : Colors::Warning;
        items.push_back(li);
    }
    orderList.setItems(items);
}

void SellerScreen::tryAddProduct() {
    int sid = platform.getActiveUserId();
    int id  = FileHandler::getNextId("data/products.txt");
    double price = 0; int stock = 0;
    try { price = std::stod(priceField.content); } catch (...) {}
    try { stock = std::stoi(stockField.content); } catch (...) {}

    bool ok = false;
    if (selectedType == "Electronic") {
        int w = 0; try { w = std::stoi(extra2Field.content); } catch (...) {}
        ElectronicProduct ep(id,sid,nameField.content,price,stock,w,extra1Field.content);
        ok = platform.addProduct(&ep);
    } else if (selectedType == "Food") {
        PerishableProduct pp(id,sid,nameField.content,price,stock,
                             extra1Field.content,extra2Field.content);
        ok = platform.addProduct(&pp);
    } else if (selectedType == "Clothing") {
        ClothingProduct cp(id,sid,nameField.content,price,stock,
                           extra1Field.content,extra2Field.content);
        ok = platform.addProduct(&cp);
    } else {
        // Custom category: write a product line directly to products.txt.
        // Format: id|sellerId|name|CATEGORY|price|stock|0.0|0|CATEGORY|attr1|attr2
        // We use the custom category name for BOTH the cat field (3) and the type field (8).
        // loadGeneral() picks up any row whose type is not Electronic/Perishable/Clothing.
        std::string line =
            std::to_string(id)   + "|" +
            std::to_string(sid)  + "|" +
            nameField.content    + "|" +
            selectedType         + "|" +    // field 3: category name
            std::to_string(price)+ "|" +
            std::to_string(stock)+ "|" +
            "0.000000|0|"                +   // avgRating | reviewCount
            selectedType         + "|" +    // field 8: type tag (same as category)
            extra1Field.content  + "|" +
            extra2Field.content;
        std::ofstream fout("data/products.txt", std::ios::app);
        if (fout) {
            fout << line << "\n";
            ok = true;
            // Reload product vectors so the new product appears immediately
            platform.reloadProducts();
        }
    }

    note.show(ok ? "Product listed!" : "Failed to list product.", ok, ui.fontReg);
    if (ok) {
        nameField.clear(); priceField.clear(); stockField.clear();
        extra1Field.clear(); extra2Field.clear();
        refreshProducts();
    }
}

void SellerScreen::handleEvent(const sf::Event& e, sf::RenderWindow& w) {
    productList.handleEvent(e, w);
    orderList.handleEvent(e, w);
    editPriceField.handleEvent(e); editStockField.handleEvent(e);
    nameField.handleEvent(e); priceField.handleEvent(e);
    stockField.handleEvent(e); extra1Field.handleEvent(e); extra2Field.handleEvent(e);
    newCategoryField.handleEvent(e);

    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);

        if (homeTabBtn.contains(mp))       activeTab = Tab::HOME;
        if (productsTabBtn.contains(mp))   activeTab = Tab::PRODUCTS;
        if (ordersTabBtn.contains(mp))     activeTab = Tab::ORDERS;
        if (addProductTabBtn.contains(mp)) activeTab = Tab::ADD_PRODUCT;

        if (activeTab == Tab::PRODUCTS) {
            auto all = platform.getAllProducts();
            int sid = platform.getActiveUserId();
            std::vector<Product*> mine;
            for (auto* p : all) if (p->getSellerId()==sid) mine.push_back(p);
            int idx = productList.getSelectedIndex();
            if (editPriceBtn.contains(mp) && idx>=0 && idx<(int)mine.size()) {
                double np=0; int ns=0;
                try{np=std::stod(editPriceField.content);}catch(...){}
                try{ns=std::stoi(editStockField.content);}catch(...){}
                platform.editProduct(mine[idx]->getId(),np,ns);
                refreshProducts();
            }
            if (deleteProductBtn.contains(mp) && idx>=0 && idx<(int)mine.size()) {
                platform.removeProduct(mine[idx]->getId());
                refreshProducts();
            }
        }

        if (activeTab == Tab::ORDERS) {
            auto ords = platform.getSellerOrders(platform.getActiveUserId());
            int idx = orderList.getSelectedIndex();
            if (idx>=0 && idx<(int)ords.size()) {
                int oid = ords[idx].getOrderId();
                if (confirmBtn.contains(mp)) { platform.updateOrderStatus(oid,Order::CONFIRMED); refreshOrders(); }
                if (shipBtn.contains(mp))    { platform.updateOrderStatus(oid,Order::SHIPPED);   refreshOrders(); }
                if (deliverBtn.contains(mp)) { platform.updateOrderStatus(oid,Order::DELIVERED); refreshOrders(); }
                if (cancelBtn.contains(mp))  { platform.updateOrderStatus(oid,Order::CANCELLED); refreshOrders(); }
            }
            if (reportBtn.contains(mp)) platform.generateSalesReport(platform.getActiveUserId());
        }

        if (activeTab == Tab::ADD_PRODUCT) {
            if (typeElecBtn.contains(mp))   setTypeActive("Electronic");
            if (typePerishBtn.contains(mp)) setTypeActive("Food");
            if (typeClothBtn.contains(mp))  setTypeActive("Clothing");

            for (size_t i = 0; i < customTypeBtns.size(); i++)
                if (customTypeBtns[i].contains(mp)) setTypeActive(customTypes[i]);

            if (addCategoryBtn.contains(mp)) {
                std::string newCat = newCategoryField.content;
                // Trim spaces
                while (!newCat.empty() && newCat.front()==' ') newCat.erase(newCat.begin());
                while (!newCat.empty() && newCat.back()==' ')  newCat.pop_back();
                if (!newCat.empty()) {
                    bool exists = false;
                    for (const auto& c : customTypes) if (c==newCat) { exists=true; break; }
                    if (!exists) {
                        customTypes.push_back(newCat);
                        saveCustomCategories();
                        rebuildCustomTypeBtns();
                        note.show("Category \"" + newCat + "\" added!", true, ui.fontReg);
                    } else {
                        note.show("Category already exists.", false, ui.fontReg);
                    }
                    newCategoryField.clear();
                }
            }

            if (addProductBtn.contains(mp)) tryAddProduct();
        }
    }
}

void SellerScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    homeTabBtn.update(m); productsTabBtn.update(m);
    ordersTabBtn.update(m); addProductTabBtn.update(m);
    editPriceField.update(dt); editStockField.update(dt);
    editPriceBtn.update(m); deleteProductBtn.update(m);
    confirmBtn.update(m); shipBtn.update(m); deliverBtn.update(m);
    cancelBtn.update(m); reportBtn.update(m);
    typeElecBtn.update(m); typePerishBtn.update(m); typeClothBtn.update(m);
    for (auto& b : customTypeBtns) b.update(m);
    newCategoryField.update(dt); addCategoryBtn.update(m);
    nameField.update(dt); priceField.update(dt); stockField.update(dt);
    extra1Field.update(dt); extra2Field.update(dt);
    addProductBtn.update(m);
    note.update(dt);
}

void SellerScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    auto* s = platform.getActiveSeller();
    std::string shopName = s ? s->getShopName() : "Seller";
    ui.drawTopBar(w, shopName, "Seller Dashboard");

    std::string crumb = "Dashboard > ";
    if      (activeTab==Tab::HOME)        crumb += "Home";
    else if (activeTab==Tab::PRODUCTS)    crumb += "My Products";
    else if (activeTab==Tab::ORDERS)      crumb += "Orders";
    else                                  crumb += "Add Product";
    ui.drawBreadcrumb(w, crumb);

    // Tab highlight
    homeTabBtn.isActive       = (activeTab == Tab::HOME);
    productsTabBtn.isActive   = (activeTab == Tab::PRODUCTS);
    ordersTabBtn.isActive     = (activeTab == Tab::ORDERS);
    addProductTabBtn.isActive = (activeTab == Tab::ADD_PRODUCT);
    homeTabBtn.draw(w); productsTabBtn.draw(w);
    ordersTabBtn.draw(w); addProductTabBtn.draw(w);
    ui.divider(w, L::CX+12.f, L::CY+52.f, L::CW-24.f, true, sf::Color(38,48,72));

    if      (activeTab == Tab::HOME)        renderHomeTab(w);
    else if (activeTab == Tab::PRODUCTS)    renderProductsTab(w);
    else if (activeTab == Tab::ORDERS)      renderOrdersTab(w);
    else                                    renderAddProductTab(w);
    note.draw(w);

    std::string activeKey = activeTab==Tab::HOME      ? "home_tab"     :
                            activeTab==Tab::PRODUCTS  ? "products_tab" :
                            activeTab==Tab::ORDERS    ? "orders_tab"   : "add_tab";
    std::string sc = ui.drawSidebar(w, shopName, "Seller", nav, activeKey, mouse, clicked);
    if (sc == "logout") { platform.logout(); nextScreen = "login"; }
    else if (sc == "home_tab")     activeTab = Tab::HOME;
    else if (sc == "products_tab") activeTab = Tab::PRODUCTS;
    else if (sc == "orders_tab")   activeTab = Tab::ORDERS;
    else if (sc == "add_tab")      activeTab = Tab::ADD_PRODUCT;
}

void SellerScreen::renderHomeTab(sf::RenderWindow& w) {
    w.draw(homeCard);
    w.draw(welcomeText);
    w.draw(homeStatProducts);
    w.draw(homeStatOrders);

    auto profileTitle = ui.makeText("Shop Profile", 18, Colors::Primary, true);
    profileTitle.setPosition(L::CX+28.f, L::CY+305.f);
    w.draw(profileTitle);
    w.draw(profileInfo);
}

void SellerScreen::renderProductsTab(sf::RenderWindow& w) {
    productList.draw(w, ui.fontReg);
    editPriceField.draw(w); editStockField.draw(w);
    editPriceBtn.draw(w); deleteProductBtn.draw(w);
}

void SellerScreen::renderOrdersTab(sf::RenderWindow& w) {
    orderList.draw(w, ui.fontReg);
    confirmBtn.draw(w); shipBtn.draw(w); deliverBtn.draw(w);
    cancelBtn.draw(w); reportBtn.draw(w);
}

void SellerScreen::renderAddProductTab(sf::RenderWindow& w) {
    const float fy = L::CY + 56.f;

    // Update isActive for built-in types
    typeElecBtn.isActive   = (selectedType == "Electronic");
    typePerishBtn.isActive = (selectedType == "Food");
    typeClothBtn.isActive  = (selectedType == "Clothing");
    typeElecBtn.draw(w); typePerishBtn.draw(w); typeClothBtn.draw(w);

    // Custom type buttons
    for (auto& b : customTypeBtns) b.draw(w);

    // New category row
    newCategoryField.draw(w); addCategoryBtn.draw(w);

    // Field labels (only draw correct ones for active type, no overlap)
    auto nl = ui.makeText("Product Name",   12, Colors::TextSecondary);
    auto pl = ui.makeText("Price",          12, Colors::TextSecondary);
    auto sl = ui.makeText("Stock",          12, Colors::TextSecondary);
    nl.setPosition(L::CX+12.f, fy+80.f);  w.draw(nl);
    pl.setPosition(L::CX+12.f, fy+142.f); w.draw(pl);
    sl.setPosition(L::CX+256.f,fy+142.f); w.draw(sl);

    std::string l1, l2;
    if      (selectedType == "Electronic") { l1="Brand";               l2="Warranty (months)"; }
    else if (selectedType == "Food")       { l1="Expiry (YYYY-MM-DD)"; l2="Storage condition"; }
    else if (selectedType == "Clothing")   { l1="Size (S/M/L/XL)";    l2="Material"; }
    else                                   { l1="Attribute 1";         l2="Attribute 2"; }

    auto e1l = ui.makeText(l1, 12, Colors::TextSecondary);
    auto e2l = ui.makeText(l2, 12, Colors::TextSecondary);
    e1l.setPosition(L::CX+12.f, fy+204.f); w.draw(e1l);
    e2l.setPosition(L::CX+256.f,fy+204.f); w.draw(e2l);

    nameField.draw(w); priceField.draw(w); stockField.draw(w);
    extra1Field.draw(w); extra2Field.draw(w);
    addProductBtn.draw(w);
}
