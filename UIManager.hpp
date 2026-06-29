#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ── Layout constants ─────────────────────────────────────────
namespace L {
    constexpr float SW   = 200.f;   // sidebar width
    constexpr float TH   = 58.f;    // top bar height
    constexpr float BH   = 28.f;    // breadcrumb height
    constexpr float HH   = TH + BH; // header total = 86
    constexpr float CX   = SW;      // content start X = 200
    constexpr float CY   = HH;      // content start Y = 86
    constexpr float CW   = 1280.f - SW;   // 1080
    constexpr float CH   = 720.f  - HH;   // 634
    constexpr float WW   = 1280.f;
    constexpr float WH   = 720.f;
}

// ── Colour palette ──────────────────────────────────────────
namespace Colors {
    inline sf::Color Background   {  15,  20,  40 };
    inline sf::Color SidebarBg    {  22,  28,  50 };  // slightly lighter
    inline sf::Color Surface      {  25,  32,  56 };
    inline sf::Color SurfaceAlt   {  35,  44,  72 };
    inline sf::Color TopBarBg     {  20,  26,  48 };
    inline sf::Color Primary      {  99, 179, 237 };
    inline sf::Color PrimaryDark  {  66, 135, 194 };
    inline sf::Color Confirm      {  48, 164,  92 };  // green confirm
    inline sf::Color ConfirmDark  {  34, 120,  66 };
    inline sf::Color Success      {  72, 187, 120 };
    inline sf::Color Warning      { 251, 191,  36 };
    inline sf::Color Danger       { 220,  55,  55 };
    inline sf::Color DangerDark   { 170,  30,  30 };
    inline sf::Color TextPrimary  { 230, 230, 240 };
    inline sf::Color TextSecondary{ 150, 155, 175 };
    inline sf::Color Border       {  45,  55,  82 };
    inline sf::Color InputBg      {  18,  24,  46 };
}

// ── Nav item descriptor ──────────────────────────────────────
struct NavItem {
    std::string label;
    std::string screenKey;   // used for highlight + click result
    std::string iconKey;     // "cart","orders","browse","profile","products","add","discount","sellers","reviews","stats"
};

// ── Button ───────────────────────────────────────────────────
struct Button {
    sf::RectangleShape shape;
    sf::Text           label;
    sf::Color          normalColor, hoverColor, pressedColor;
    bool isHovered  = false;
    bool isPressed  = false;
    bool isDisabled = false;
    bool isActive   = false;
    void setPosition(float x, float y);
    void setSize(float w, float h);
    bool contains(sf::Vector2f p) const;
    void update(sf::Vector2f mouse);
    void draw(sf::RenderWindow& w);
};

// ── InputField ───────────────────────────────────────────────
struct InputField {
    sf::RectangleShape box;
    sf::Text           text, placeholder;
    sf::RectangleShape cursor;
    std::string        content;
    bool  isFocused = false, isPassword = false;
    float cursorTimer = 0.f;
    bool  cursorVisible = true;
    void setPosition(float x, float y, float w, float h);
    void handleEvent(const sf::Event& e);
    void update(float dt);
    void draw(sf::RenderWindow& w);
    void clear();
};

// ── List item ────────────────────────────────────────────────
struct ListItem {
    std::string id, line1, line2, badge;
    sf::Color   badgeColor = sf::Color(72,187,120);
    bool        selected   = false;
};

// ── Scroll panel ─────────────────────────────────────────────
struct ScrollPanel {
    std::vector<ListItem> items;
    float  scrollOffset  = 0.f;
    float  itemHeight    = 68.f;
    int    selectedIndex = -1;
    sf::FloatRect bounds {0,0,400,400};
    std::string emptyLabel = "Nothing here yet.";
    void setItems(std::vector<ListItem> v);
    void handleEvent(const sf::Event& e, sf::RenderWindow& w);
    void draw(sf::RenderWindow& w, const sf::Font& font);
    int  getSelectedIndex() const { return selectedIndex; }
};

// ── Notification ─────────────────────────────────────────────
struct Notification {
    sf::RectangleShape box, outlineBox;
    sf::Text           msg;
    float lifetime = 0.f, maxTime = 3.f;
    bool  visible  = false;
    bool  isOk     = true;
    // isOk=true → green text + border; false → red text + border
    void show(const std::string& text, bool ok, const sf::Font& font);
    void update(float dt);
    void draw(sf::RenderWindow& w);
};

// ── UIManager ────────────────────────────────────────────────
class UIManager {
public:
    sf::Font fontReg, fontBold;
    UIManager();

    sf::Text makeText(const std::string& s, unsigned sz,
                      sf::Color col = sf::Color(230,230,240), bool bold = false) const;

    Button makePrimaryButton  (const std::string& lbl, float x,float y,float w=200,float h=42);
    Button makeSecondaryButton(const std::string& lbl, float x,float y,float w=200,float h=42);
    Button makeDangerButton   (const std::string& lbl, float x,float y,float w=200,float h=42);
    Button makeConfirmButton  (const std::string& lbl, float x,float y,float w=200,float h=42);

    InputField makeInput(const std::string& ph, float x,float y,float w=300,float h=42,bool pwd=false);
    sf::RectangleShape makeCard(float x,float y,float w,float h,
                                sf::Color fill=sf::Color(25,32,56));
    void divider(sf::RenderWindow& w, float x,float y,float len,bool horiz=true,
                 sf::Color c=sf::Color(45,55,82));

    // ── Persistent chrome (call first in every render) ────────
    void drawTopBar(sf::RenderWindow& w, const std::string& shopName,
                    const std::string& rightInfo="");
    void drawBreadcrumb(sf::RenderWindow& w, const std::string& crumb);

    // Returns "" or screenKey/("logout") when a nav row is clicked.
    // Call AFTER content so sidebar renders on top.
    std::string drawSidebar(sf::RenderWindow& w,
                            const std::string& userName,
                            const std::string& userRole,
                            const std::vector<NavItem>& items,
                            const std::string& activeKey,
                            sf::Vector2f mouse,
                            bool clicked);

    void drawBadge(sf::RenderWindow& w, const std::string& txt,
                   float x,float y,sf::Color c);

    // Small icon drawn at (cx,cy)
    void icon(sf::RenderWindow& w, const std::string& key,
              float cx,float cy,float r,sf::Color c);

private:
    bool loadFonts();
    void relabel(Button& b);
};
