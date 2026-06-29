#include "../include/screens/AdminScreen.hpp"
#include <sstream>
#include <iomanip>

AdminScreen::AdminScreen(Platform& p, UIManager& u) : platform(p), ui(u) {
    nav = {
        {"Sellers",   "sellers_tab",   "sellers"},
        {"Discounts", "discounts_tab", "discount"},
        {"Reviews",   "reviews_tab",   "reviews"},
        {"Stats",     "stats_tab",     "stats"},
    };

    const float ty = L::CY + 8.f;
    sellersTabBtn   = ui.makeSecondaryButton("Sellers",   L::CX+12.f,  ty, 118.f, 38.f);
    discountsTabBtn = ui.makeSecondaryButton("Discounts", L::CX+138.f, ty, 118.f, 38.f);
    reviewsTabBtn   = ui.makeSecondaryButton("Reviews",   L::CX+264.f, ty, 110.f, 38.f);
    statsTabBtn     = ui.makeSecondaryButton("Stats",     L::CX+382.f, ty,  96.f, 38.f);

    // Sellers tab
    sellerList.bounds    = {L::CX+12.f, L::CY+56.f, L::CW-24.f, 450.f};
    sellerList.itemHeight = 68.f;
    sellerList.emptyLabel = "No sellers registered yet.";
    const float sb = L::CY + 516.f;
    approveBtn = ui.makeConfirmButton("Approve", L::CX+12.f, sb, 130.f, 40.f);
    banBtn     = ui.makeDangerButton ("Ban",     L::CX+150.f,sb,  96.f, 40.f);

    // Discounts tab
    discountList.bounds    = {L::CX+12.f, L::CY+56.f, L::CW-24.f, 280.f};
    discountList.itemHeight = 56.f;
    discountList.emptyLabel = "No discount codes yet.";
    const float df = L::CY + 350.f;
    codeField     = ui.makeInput("Code",       L::CX+12.f, df, 188.f, 42.f);
    typePercBtn   = ui.makePrimaryButton  ("%",L::CX+208.f, df,  56.f, 42.f);
    typeFlatBtn   = ui.makeSecondaryButton("Rs.",L::CX+272.f,df,  60.f, 42.f);
    valueField    = ui.makeInput("Value",      L::CX+340.f, df, 120.f, 42.f);
    minOrderField = ui.makeInput("Min order",  L::CX+468.f, df, 148.f, 42.f);
    addCodeBtn    = ui.makeConfirmButton("Add Code",   L::CX+12.f, df+52.f, 188.f, 40.f);
    deactivateCodeBtn = ui.makeDangerButton("Deactivate",L::CX+208.f,df+52.f,148.f,40.f);

    // Reviews tab
    reviewList.bounds    = {L::CX+12.f, L::CY+56.f, L::CW-24.f, 450.f};
    reviewList.itemHeight = 72.f;
    reviewList.emptyLabel = "No reviews submitted yet.";
    flagReviewBtn   = ui.makeSecondaryButton("Flag",   L::CX+12.f,  sb, 116.f, 40.f);
    deleteReviewBtn = ui.makeDangerButton   ("Delete", L::CX+136.f, sb, 116.f, 40.f);

    // Stats
    statsCard = ui.makeCard(L::CX+12.f, L::CY+56.f, 580.f, 310.f);
    float sy = L::CY+90.f;
    for (auto* t : {&statCustomers,&statSellers,&statProducts,&statOrders,&statRevenue}) {
        *t = ui.makeText("", 18, Colors::TextPrimary);
        t->setPosition(L::CX+32.f, sy); sy += 48.f;
    }
}

void AdminScreen::onEnter() { refreshAll(); }

void AdminScreen::refreshAll() {
    // Sellers list — all sellers
    auto allSellers = platform.getAllSellers();
    std::vector<ListItem> si;
    for (const auto& s : allSellers) {
        ListItem li;
        li.id    = std::to_string(s.getId());
        li.line1 = s.getShopName() + "  —  " + s.getName();
        li.line2 = s.getEmail();
        li.badge = s.getIsApproved() ? "Approved" : "Pending";
        li.badgeColor = s.getIsApproved() ? Colors::Success : Colors::Warning;
        si.push_back(li);
    }
    sellerList.setItems(si);

    // Reviews list
    auto revs = platform.getAllReviews();
    std::vector<ListItem> ri;
    for (const auto& r : revs) {
        ListItem li;
        li.id    = std::to_string(r.getReviewId());
        li.line1 = "Customer #" + std::to_string(r.getCustomerId())
                 + "  |  Rating: " + std::to_string(r.getRating()) + "/5";
        li.line2 = r.getComment();
        li.badge = r.getIsFlagged() ? "Flagged" : "";
        li.badgeColor = Colors::Danger;
        ri.push_back(li);
    }
    reviewList.setItems(ri);

    // Stats
    auto customers = platform.getAllCustomers();
    auto sellers   = platform.getAllSellers();
    auto products  = platform.getAllProducts();
    auto orders    = platform.getAllOrders();
    double revenue = 0.0;
    for (const auto& o : orders) revenue += o.getTotalAmount();
    std::ostringstream rev; rev << std::fixed << std::setprecision(2) << revenue;
    statCustomers.setString("Customers  :  " + std::to_string(customers.size()));
    statSellers.setString  ("Sellers    :  " + std::to_string(sellers.size()));
    statProducts.setString ("Products   :  " + std::to_string(products.size()));
    statOrders.setString   ("Orders     :  " + std::to_string(orders.size()));
    statRevenue.setString  ("Revenue    :  Rs. " + rev.str());
}

void AdminScreen::handleEvent(const sf::Event& e, sf::RenderWindow& w) {
    sellerList.handleEvent(e, w); reviewList.handleEvent(e, w); discountList.handleEvent(e, w);
    codeField.handleEvent(e); valueField.handleEvent(e); minOrderField.handleEvent(e);

    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);
        if (sellersTabBtn.contains(mp))   activeTab = Tab::SELLERS;
        if (discountsTabBtn.contains(mp)) activeTab = Tab::DISCOUNTS;
        if (reviewsTabBtn.contains(mp))   activeTab = Tab::REVIEWS;
        if (statsTabBtn.contains(mp))     { activeTab = Tab::STATS; refreshAll(); }

        if (activeTab == Tab::SELLERS) {
            auto allS = platform.getAllSellers();
            int idx = sellerList.getSelectedIndex();
            if (idx >= 0 && idx < (int)allS.size()) {
                if (approveBtn.contains(mp)) { platform.approveSeller(allS[idx].getId()); refreshAll(); }
                if (banBtn.contains(mp))     { platform.banSeller(allS[idx].getId());     refreshAll(); }
            }
        }
        if (activeTab == Tab::DISCOUNTS) {
            if (typePercBtn.contains(mp)) isPercent = true;
            if (typeFlatBtn.contains(mp)) isPercent = false;
            if (addCodeBtn.contains(mp)) {
                double val=0, minO=0;
                try{val=std::stod(valueField.content);}catch(...){}
                try{minO=std::stod(minOrderField.content);}catch(...){}
                bool ok = platform.addDiscountCode(codeField.content,
                    isPercent?DiscountCode::PERCENTAGE:DiscountCode::FLAT, val, minO);
                note.show(ok?"Discount code added!":"Code already exists.", ok, ui.fontReg);
                if (ok) { codeField.clear(); valueField.clear(); minOrderField.clear(); }
            }
            if (deactivateCodeBtn.contains(mp)) {
                platform.removeDiscountCode(codeField.content);
                note.show("Code deactivated.", false, ui.fontReg);
            }
        }
        if (activeTab == Tab::REVIEWS) {
            auto revs = platform.getAllReviews();
            int idx = reviewList.getSelectedIndex();
            if (idx >= 0 && idx < (int)revs.size()) {
                int rid = revs[idx].getReviewId();
                if (flagReviewBtn.contains(mp))   { platform.flagReview(rid);   refreshAll(); }
                if (deleteReviewBtn.contains(mp)) { platform.deleteReview(rid); refreshAll(); }
            }
        }
    }
}

void AdminScreen::update(float dt, sf::RenderWindow& w) {
    sf::Vector2f m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(w));
    sellersTabBtn.update(m); discountsTabBtn.update(m);
    reviewsTabBtn.update(m); statsTabBtn.update(m);
    approveBtn.update(m); banBtn.update(m);
    typePercBtn.update(m); typeFlatBtn.update(m);
    addCodeBtn.update(m); deactivateCodeBtn.update(m);
    flagReviewBtn.update(m); deleteReviewBtn.update(m);
    codeField.update(dt); valueField.update(dt); minOrderField.update(dt);
    note.update(dt);
}

void AdminScreen::render(sf::RenderWindow& w, bool clicked, sf::Vector2f mouse) {
    ui.drawTopBar(w, "SwiftCart", "Admin Panel");
    std::string crumb = "Admin > ";
    if (activeTab==Tab::SELLERS)   crumb += "Sellers";
    else if (activeTab==Tab::DISCOUNTS) crumb += "Discounts";
    else if (activeTab==Tab::REVIEWS)   crumb += "Reviews";
    else                                crumb += "Platform Stats";
    ui.drawBreadcrumb(w, crumb);

    sellersTabBtn.isActive   = (activeTab == Tab::SELLERS);
    discountsTabBtn.isActive = (activeTab == Tab::DISCOUNTS);
    reviewsTabBtn.isActive   = (activeTab == Tab::REVIEWS);
    statsTabBtn.isActive     = (activeTab == Tab::STATS);
    sellersTabBtn.draw(w); discountsTabBtn.draw(w);
    reviewsTabBtn.draw(w); statsTabBtn.draw(w);
    ui.divider(w, L::CX+12.f, L::CY+52.f, L::CW-24.f, true, sf::Color(38,48,72));

    if      (activeTab == Tab::SELLERS)   renderSellersTab(w);
    else if (activeTab == Tab::DISCOUNTS) renderDiscountsTab(w);
    else if (activeTab == Tab::REVIEWS)   renderReviewsTab(w);
    else                                  renderStatsTab(w);
    note.draw(w);

    std::string activeKey = activeTab==Tab::SELLERS   ? "sellers_tab"   :
                            activeTab==Tab::DISCOUNTS ? "discounts_tab" :
                            activeTab==Tab::REVIEWS   ? "reviews_tab"   : "stats_tab";
    std::string sc = ui.drawSidebar(w, "Admin", "Administrator", nav, activeKey, mouse, clicked);
    if (sc == "logout") { platform.logout(); nextScreen = "login"; }
    else if (sc == "sellers_tab")   activeTab = Tab::SELLERS;
    else if (sc == "discounts_tab") activeTab = Tab::DISCOUNTS;
    else if (sc == "reviews_tab")   activeTab = Tab::REVIEWS;
    else if (sc == "stats_tab")     { activeTab = Tab::STATS; refreshAll(); }
}

void AdminScreen::renderSellersTab(sf::RenderWindow& w) {
    sellerList.draw(w, ui.fontReg);
    approveBtn.draw(w); banBtn.draw(w);
}

void AdminScreen::renderDiscountsTab(sf::RenderWindow& w) {
    discountList.draw(w, ui.fontReg);
    typePercBtn.isActive  =  isPercent;
    typeFlatBtn.isActive  = !isPercent;
    codeField.draw(w); typePercBtn.draw(w); typeFlatBtn.draw(w);
    valueField.draw(w); minOrderField.draw(w);
    addCodeBtn.draw(w); deactivateCodeBtn.draw(w);
}

void AdminScreen::renderReviewsTab(sf::RenderWindow& w) {
    reviewList.draw(w, ui.fontReg);
    flagReviewBtn.draw(w); deleteReviewBtn.draw(w);
}

void AdminScreen::renderStatsTab(sf::RenderWindow& w) {
    w.draw(statsCard);
    w.draw(statCustomers); w.draw(statSellers);
    w.draw(statProducts);  w.draw(statOrders); w.draw(statRevenue);
}
