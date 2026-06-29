#include "include/UIManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

UIManager::UIManager() {
    if (!loadFonts())
        std::cerr << "[UIManager] Place Roboto-Regular.ttf in assets/fonts/\n";
}
bool UIManager::loadFonts() {
    bool a = fontReg.loadFromFile("assets/fonts/Roboto-Regular.ttf");
    if (!a) a = fontReg.loadFromFile("assets/fonts/Arial.ttf");
    bool b = fontBold.loadFromFile("assets/fonts/Roboto-Bold.ttf");
    if (!b) fontBold = fontReg;
    return a;
}

sf::Text UIManager::makeText(const std::string& s, unsigned sz, sf::Color c, bool bold) const {
    sf::Text t; t.setFont(bold?fontBold:fontReg); t.setString(s);
    t.setCharacterSize(sz); t.setFillColor(c); return t;
}

void UIManager::relabel(Button& b) {
    auto bs = b.shape.getSize(); auto p = b.shape.getPosition();
    auto lb = b.label.getLocalBounds();
    b.label.setPosition(p.x+(bs.x-lb.width)/2.f-lb.left,
                        p.y+(bs.y-lb.height)/2.f-lb.top-2.f);
}

static Button makeBtn(sf::Color n, sf::Color h, sf::Color pr,
                      const std::string& lbl, float x, float y, float w, float h_,
                      bool bold, sf::Color txtCol, UIManager& ui, bool outline=false) {
    Button b;
    b.normalColor=n; b.hoverColor=h; b.pressedColor=pr;
    b.shape.setSize({w,h_}); b.shape.setPosition(x,y); b.shape.setFillColor(n);
    if (outline) { b.shape.setOutlineThickness(1); b.shape.setOutlineColor(Colors::Border); }
    b.label = ui.makeText(lbl,15,txtCol,bold);
    auto bs=b.shape.getSize(); auto pos=b.shape.getPosition();
    auto lb=b.label.getLocalBounds();
    b.label.setPosition(pos.x+(bs.x-lb.width)/2.f-lb.left,
                        pos.y+(bs.y-lb.height)/2.f-lb.top-2.f);
    return b;
}

Button UIManager::makePrimaryButton(const std::string& l,float x,float y,float w,float h){
    return makeBtn(Colors::Primary,Colors::PrimaryDark,{44,100,160},l,x,y,w,h,true,sf::Color::White,*this); }
Button UIManager::makeSecondaryButton(const std::string& l,float x,float y,float w,float h){
    return makeBtn(Colors::Surface,Colors::SurfaceAlt,Colors::Border,l,x,y,w,h,false,Colors::TextPrimary,*this,true); }
Button UIManager::makeDangerButton(const std::string& l,float x,float y,float w,float h){
    return makeBtn(Colors::Danger,Colors::DangerDark,{130,20,20},l,x,y,w,h,true,sf::Color::White,*this); }
Button UIManager::makeConfirmButton(const std::string& l,float x,float y,float w,float h){
    return makeBtn(Colors::Confirm,Colors::ConfirmDark,{24,90,48},l,x,y,w,h,true,sf::Color::White,*this); }

InputField UIManager::makeInput(const std::string& ph,float x,float y,float w,float h,bool pwd){
    InputField f; f.isPassword=pwd;
    f.box.setSize({w,h}); f.box.setPosition(x,y);
    f.box.setFillColor(Colors::InputBg);
    f.box.setOutlineThickness(1); f.box.setOutlineColor(Colors::Border);
    f.text=makeText("",15,Colors::TextPrimary);
    f.text.setPosition(x+10.f,y+(h-18.f)/2.f);
    f.placeholder=makeText(ph,15,Colors::TextSecondary);
    f.placeholder.setPosition(x+10.f,y+(h-18.f)/2.f);
    f.cursor.setSize({1.5f,h-14.f}); f.cursor.setFillColor(Colors::Primary);
    f.cursor.setPosition(x+10.f,y+7.f); return f;
}

sf::RectangleShape UIManager::makeCard(float x,float y,float w,float h,sf::Color fill){
    sf::RectangleShape c; c.setSize({w,h}); c.setPosition(x,y);
    c.setFillColor(fill); c.setOutlineThickness(1); c.setOutlineColor(Colors::Border); return c;
}

void UIManager::divider(sf::RenderWindow& w,float x,float y,float len,bool horiz,sf::Color c){
    sf::RectangleShape r(horiz?sf::Vector2f{len,1.f}:sf::Vector2f{1.f,len});
    r.setPosition(x,y); r.setFillColor(c); w.draw(r);
}

// ── Icon renderer ──────────────────────────────────────────
void UIManager::icon(sf::RenderWindow& w, const std::string& k,
                     float cx, float cy, float r, sf::Color c) {
    if (k=="cart") {
        sf::RectangleShape body({r*1.5f,r*1.f});
        body.setFillColor(sf::Color::Transparent);
        body.setOutlineThickness(1.8f); body.setOutlineColor(c);
        body.setPosition(cx-r*0.75f,cy-r*0.3f); w.draw(body);
        // handle
        sf::RectangleShape ha({r*0.6f,1.8f}),hb({1.8f,r*0.5f});
        ha.setFillColor(c); hb.setFillColor(c);
        ha.setPosition(cx-r*0.3f,cy-r*0.75f); hb.setPosition(cx-r*0.3f,cy-r*0.75f);
        w.draw(ha); w.draw(hb);
        sf::CircleShape wh(2.4f); wh.setFillColor(c);
        wh.setPosition(cx-r*0.4f,cy+r*0.65f); w.draw(wh);
        wh.setPosition(cx+r*0.1f,cy+r*0.65f); w.draw(wh);
    } else if (k=="browse"||k=="store") {
        // shop / storefront
        sf::ConvexShape roof(3);
        roof.setPoint(0,{cx,cy-r*1.1f});
        roof.setPoint(1,{cx-r*1.1f,cy-r*0.1f});
        roof.setPoint(2,{cx+r*1.1f,cy-r*0.1f});
        roof.setFillColor(c); w.draw(roof);
        sf::RectangleShape base({r*1.6f,r*1.0f});
        base.setFillColor(c); base.setPosition(cx-r*0.8f,cy-r*0.1f); w.draw(base);
        sf::RectangleShape door({r*0.5f,r*0.7f});
        door.setFillColor(Colors::SidebarBg);
        door.setPosition(cx-r*0.25f,cy+r*0.2f); w.draw(door);
    } else if (k=="orders") {
        // receipt / list
        sf::RectangleShape sheet({r*1.5f,r*2.0f});
        sheet.setFillColor(sf::Color::Transparent);
        sheet.setOutlineThickness(1.8f); sheet.setOutlineColor(c);
        sheet.setPosition(cx-r*0.75f,cy-r); w.draw(sheet);
        for(int i=0;i<3;i++){
            sf::RectangleShape line({r*0.9f,1.5f});
            line.setFillColor(c);
            line.setPosition(cx-r*0.45f,cy-r*0.5f+i*r*0.5f); w.draw(line);
        }
    } else if (k=="profile") {
        sf::CircleShape head(r*0.45f); head.setFillColor(c);
        head.setPosition(cx-r*0.45f,cy-r); w.draw(head);
        // body arc (half-circle for shoulders)
        sf::CircleShape body(r*0.8f); body.setFillColor(c);
        body.setPosition(cx-r*0.8f,cy-r*0.1f); w.draw(body);
        // mask upper body with sidebar colour to create silhouette
        sf::RectangleShape mask({r*2.f,r*0.6f});
        mask.setFillColor(Colors::SidebarBg);
        mask.setPosition(cx-r,cy-r*0.5f); w.draw(mask);
    } else if (k=="logout") {
        sf::RectangleShape bar({r*1.2f,2.f}); bar.setFillColor(c);
        bar.setPosition(cx-r*0.6f,cy-1.f); w.draw(bar);
        sf::ConvexShape arr(3);
        arr.setPoint(0,{cx+r*0.6f,cy});
        arr.setPoint(1,{cx+r*0.15f,cy-r*0.45f});
        arr.setPoint(2,{cx+r*0.15f,cy+r*0.45f});
        arr.setFillColor(c); w.draw(arr);
        // door frame
        sf::RectangleShape top({r*1.f,1.8f}),side({1.8f,r*1.2f}),bot({r*1.f,1.8f});
        top.setFillColor(c); side.setFillColor(c); bot.setFillColor(c);
        top.setPosition(cx-r,cy-r*0.6f); side.setPosition(cx-r,cy-r*0.6f);
        bot.setPosition(cx-r,cy+r*0.55f);
        w.draw(top); w.draw(side); w.draw(bot);
    } else if (k=="add"||k=="plus") {
        sf::RectangleShape h({r*1.6f,2.f}),v({2.f,r*1.6f});
        h.setFillColor(c); v.setFillColor(c);
        h.setPosition(cx-r*0.8f,cy-1.f); v.setPosition(cx-1.f,cy-r*0.8f);
        w.draw(h); w.draw(v);
    } else if (k=="products"||k=="box") {
        sf::RectangleShape b({r*1.6f,r*1.2f});
        b.setFillColor(sf::Color::Transparent);
        b.setOutlineThickness(1.8f); b.setOutlineColor(c);
        b.setPosition(cx-r*0.8f,cy-r*0.4f); w.draw(b);
        sf::RectangleShape lid({r*1.6f,1.8f}); lid.setFillColor(c);
        lid.setPosition(cx-r*0.8f,cy-r*0.8f); w.draw(lid);
    } else if (k=="discount"||k=="tag") {
        sf::CircleShape c1(r*0.28f),c2(r*0.28f);
        c1.setFillColor(c); c2.setFillColor(c);
        c1.setPosition(cx-r*0.85f,cy-r*0.9f); w.draw(c1);
        c2.setPosition(cx+r*0.2f,cy+r*0.05f); w.draw(c2);
        sf::RectangleShape sl({r*1.8f,2.f}); sl.setFillColor(c);
        sl.setRotation(-52.f); sl.setPosition(cx+r*0.5f,cy-r*0.7f); w.draw(sl);
    } else if (k=="sellers") {
        // two person silhouettes
        for(int i=0;i<2;i++){
            float ox=cx+(i==0?-r*0.5f:r*0.2f);
            sf::CircleShape h2(r*0.3f); h2.setFillColor(c);
            h2.setPosition(ox-r*0.3f,cy-r*0.8f); w.draw(h2);
            sf::CircleShape bd(r*0.45f); bd.setFillColor(c);
            bd.setPosition(ox-r*0.45f,cy-r*0.1f); w.draw(bd);
        }
    } else if (k=="reviews"||k=="star") {
        sf::ConvexShape star(10);
        float r1=r,r2=r*0.44f;
        for(int i=0;i<10;i++){
            float a=i*3.14159f/5.f-3.14159f/2.f;
            float rad=(i%2==0)?r1:r2;
            star.setPoint(i,{cx+rad*std::cos(a),cy+rad*std::sin(a)});
        }
        star.setFillColor(c); w.draw(star);
    } else if (k=="stats"||k=="chart") {
        for(int i=0;i<3;i++){
            float barH=r*(0.5f+i*0.35f);
            sf::RectangleShape bar({r*0.36f,barH});
            bar.setFillColor(c);
            bar.setPosition(cx-r*0.7f+i*r*0.65f,cy+r*0.6f-barH); w.draw(bar);
        }
    } else if (k=="check") {
        sf::RectangleShape a({r*0.7f,2.2f}),b_({r*1.1f,2.2f});
        a.setFillColor(c); b_.setFillColor(c);
        a.setRotation(45.f);  a.setPosition(cx-r*0.55f,cy+r*0.1f);
        b_.setRotation(-52.f); b_.setPosition(cx-r*0.1f,cy+r*0.5f);
        w.draw(a); w.draw(b_);
    } else if (k=="search") {
        sf::CircleShape circle(r*0.6f);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(2.f); circle.setOutlineColor(c);
        circle.setPosition(cx-r*1.0f,cy-r*1.0f); w.draw(circle);
        sf::RectangleShape handle({r*0.7f,2.2f});
        handle.setFillColor(c); handle.setRotation(45.f);
        handle.setPosition(cx-r*0.05f,cy+r*0.1f); w.draw(handle);
    } else if (k=="truck") {
        sf::RectangleShape body({r*1.4f,r*0.9f});
        body.setFillColor(c); body.setPosition(cx-r*0.9f,cy-r*0.55f); w.draw(body);
        sf::RectangleShape cab({r*0.55f,r*0.75f});
        cab.setFillColor(c); cab.setPosition(cx+r*0.5f,cy-r*0.45f); w.draw(cab);
        sf::CircleShape wheel(r*0.22f); wheel.setFillColor(Colors::Background);
        wheel.setPosition(cx-r*0.65f,cy+r*0.3f); w.draw(wheel);
        wheel.setPosition(cx+r*0.2f,cy+r*0.3f); w.draw(wheel);
    } else {
        // fallback dot
        sf::CircleShape dot(r*0.4f); dot.setFillColor(c);
        dot.setPosition(cx-r*0.4f,cy-r*0.4f); w.draw(dot);
    }
}

// ── Top bar (persistent chrome) ──────────────────────────────
void UIManager::drawTopBar(sf::RenderWindow& w, const std::string& shopName,
                            const std::string& rightInfo) {
    sf::RectangleShape bar({L::WW,L::TH});
    bar.setFillColor(Colors::TopBarBg); w.draw(bar);
    // Shop name — large, bold, primary, positioned after sidebar
    auto title=makeText(shopName,28,Colors::Primary,true);
    title.setPosition(L::SW+18.f,12.f); w.draw(title);
    // Right info
    if(!rightInfo.empty()){
        auto ri=makeText(rightInfo,13,Colors::TextSecondary);
        auto rb=ri.getLocalBounds();
        ri.setPosition(L::WW-rb.width-20.f,22.f); w.draw(ri);
    }
    divider(w,0,L::TH,L::WW,true,sf::Color(35,45,68));
}

// ── Breadcrumb bar ────────────────────────────────────────────
void UIManager::drawBreadcrumb(sf::RenderWindow& w, const std::string& crumb) {
    sf::RectangleShape bar({L::WW,L::BH});
    bar.setPosition(0,L::TH); bar.setFillColor(sf::Color(18,24,42)); w.draw(bar);
    auto t=makeText(crumb,12,Colors::TextSecondary);
    t.setPosition(L::SW+18.f,L::TH+7.f); w.draw(t);
    divider(w,0,L::TH+L::BH,L::WW,true,sf::Color(32,40,62));
}

// ── Sidebar ───────────────────────────────────────────────────
std::string UIManager::drawSidebar(sf::RenderWindow& w,
                                    const std::string& userName,
                                    const std::string& userRole,
                                    const std::vector<NavItem>& items,
                                    const std::string& activeKey,
                                    sf::Vector2f mouse,
                                    bool clicked) {
    std::string result="";
    // Background
    sf::RectangleShape bg({L::SW,L::WH}); bg.setFillColor(Colors::SidebarBg); w.draw(bg);
    // Right border
    divider(w,L::SW-1.f,0,L::WH,false,sf::Color(35,45,68));

    // App logo row (same height as top bar)
    sf::RectangleShape logoBg({L::SW,L::TH}); logoBg.setFillColor(sf::Color(16,20,40)); w.draw(logoBg);
    auto appName=makeText("SwiftCart",18,Colors::Primary,true);
    appName.setPosition(14.f,18.f); w.draw(appName);
    divider(w,0,L::TH,L::SW,true,sf::Color(35,45,68));

    // User avatar + info row
    float uy=L::TH+8.f;
    sf::CircleShape av(17.f); av.setFillColor(sf::Color(55,100,160));
    av.setPosition(12.f,uy+2.f); w.draw(av);
    // initials
    std::string ini = userName.empty()?"?":std::string(1,std::toupper(userName[0]));
    auto initT=makeText(ini,13,sf::Color::White,true);
    initT.setPosition(20.f,uy+6.f); w.draw(initT);
    auto uname=makeText(userName,13,Colors::TextPrimary,true);
    auto urole=makeText(userRole,11,Colors::TextSecondary);
    uname.setPosition(50.f,uy+2.f); urole.setPosition(50.f,uy+20.f);
    w.draw(uname); w.draw(urole);
    divider(w,8.f,uy+42.f,L::SW-16.f,true,sf::Color(35,44,65));

    // Nav items
    float ny=uy+52.f;
    const float IH=42.f, IGAP=4.f;
    for(const auto& item:items){
        bool active=(item.screenKey==activeKey);
        sf::FloatRect row(4.f,ny,L::SW-8.f,IH);
        bool hov=row.contains(mouse);
        sf::RectangleShape rowBg({L::SW-8.f,IH}); rowBg.setPosition(4.f,ny);
        rowBg.setFillColor(active?sf::Color(40,75,130):hov?sf::Color(30,38,62):sf::Color::Transparent);
        w.draw(rowBg);
        if(active){
            sf::RectangleShape accent({3.f,IH}); accent.setPosition(4.f,ny);
            accent.setFillColor(Colors::Primary); w.draw(accent);
        }
        sf::Color ic=active?Colors::Primary:hov?sf::Color(190,210,235):Colors::TextSecondary;
        icon(w,item.iconKey,22.f,ny+IH/2.f,7.f,ic);
        auto lbl=makeText(item.label,13,active?Colors::Primary:hov?Colors::TextPrimary:Colors::TextSecondary,active);
        lbl.setPosition(40.f,ny+(IH-13.f)/2.f-1.f); w.draw(lbl);
        if(clicked&&hov) result=item.screenKey;
        ny+=IH+IGAP;
    }

    // Sign out row at bottom
    divider(w,8.f,L::WH-50.f,L::SW-16.f,true,sf::Color(35,44,65));
    float ly=L::WH-42.f;
    bool lhov=sf::FloatRect(4.f,ly,L::SW-8.f,36.f).contains(mouse);
    sf::RectangleShape lrow({L::SW-8.f,36.f}); lrow.setPosition(4.f,ly);
    lrow.setFillColor(lhov?sf::Color(70,20,20):sf::Color::Transparent); w.draw(lrow);
    icon(w,"logout",22.f,ly+18.f,7.f,lhov?Colors::Danger:Colors::TextSecondary);
    auto lo=makeText("Sign Out",13,lhov?Colors::Danger:Colors::TextSecondary);
    lo.setPosition(40.f,ly+9.f); w.draw(lo);
    if(clicked&&lhov) result="logout";

    return result;
}

// ── Badge ─────────────────────────────────────────────────────
void UIManager::drawBadge(sf::RenderWindow& w,const std::string& txt,float x,float y,sf::Color c){
    auto t=makeText(txt,12,sf::Color::White,true);
    auto b=t.getLocalBounds();
    sf::RectangleShape bg({b.width+12.f,20.f}); bg.setPosition(x,y); bg.setFillColor(c);
    t.setPosition(x+6.f,y+2.f); w.draw(bg); w.draw(t);
}

// ── Button ────────────────────────────────────────────────────
bool Button::contains(sf::Vector2f p) const { return shape.getGlobalBounds().contains(p); }
void Button::setPosition(float x,float y){
    shape.setPosition(x,y);
    auto bs=shape.getSize(); auto lb=label.getLocalBounds();
    label.setPosition(x+(bs.x-lb.width)/2.f-lb.left,y+(bs.y-lb.height)/2.f-lb.top-2.f);
}
void Button::setSize(float w,float h){
    shape.setSize({w,h});
    auto p=shape.getPosition(); auto lb=label.getLocalBounds();
    label.setPosition(p.x+(w-lb.width)/2.f-lb.left,p.y+(h-lb.height)/2.f-lb.top-2.f);
}
void Button::update(sf::Vector2f m){
    if(isDisabled){shape.setFillColor(Colors::Border);return;}
    if(isActive){shape.setFillColor(isHovered?hoverColor:Colors::Primary);return;}
    isHovered=contains(m);
    shape.setFillColor(isPressed?pressedColor:isHovered?hoverColor:normalColor);
}
void Button::draw(sf::RenderWindow& w){ w.draw(shape); w.draw(label); }

// ── InputField ────────────────────────────────────────────────
void InputField::setPosition(float x,float y,float w,float h){
    box.setPosition(x,y); box.setSize({w,h});
    text.setPosition(x+10.f,y+(h-18.f)/2.f);
    placeholder.setPosition(x+10.f,y+(h-18.f)/2.f);
    cursor.setPosition(x+10.f,y+7.f);
}
void InputField::handleEvent(const sf::Event& e){
    if(e.type==sf::Event::MouseButtonPressed){
        isFocused=box.getGlobalBounds().contains((float)e.mouseButton.x,(float)e.mouseButton.y);
        box.setOutlineColor(isFocused?Colors::Primary:Colors::Border);
    }
    if(!isFocused)return;
    if(e.type==sf::Event::TextEntered){
        uint32_t c=e.text.unicode;
        if(c==8&&!content.empty()) content.pop_back();
        else if(c>=32&&c<127) content+=(char)c;
        std::string d=isPassword?std::string(content.size(),'*'):content;
        text.setString(d);
        auto pos=box.getPosition(); auto lb=text.getLocalBounds();
        cursor.setPosition(pos.x+10.f+lb.width+2.f,pos.y+7.f);
    }
}
void InputField::update(float dt){
    if(!isFocused)return; cursorTimer+=dt;
    if(cursorTimer>=0.5f){cursorVisible=!cursorVisible;cursorTimer=0.f;}
}
void InputField::draw(sf::RenderWindow& w){
    w.draw(box);
    if(content.empty()) w.draw(placeholder); else w.draw(text);
    if(isFocused&&cursorVisible) w.draw(cursor);
}
void InputField::clear(){
    content.clear(); text.setString("");
    auto p=box.getPosition(); cursor.setPosition(p.x+10.f,p.y+7.f);
}

// ── ScrollPanel ───────────────────────────────────────────────
void ScrollPanel::setItems(std::vector<ListItem> v){
    items=std::move(v); scrollOffset=0.f; selectedIndex=-1;
}
void ScrollPanel::handleEvent(const sf::Event& e,sf::RenderWindow&){
    if(e.type==sf::Event::MouseWheelScrolled){
        float mx=std::max(0.f,(float)items.size()*itemHeight-bounds.height);
        scrollOffset-=e.mouseWheelScroll.delta*30.f;
        scrollOffset=std::max(0.f,std::min(scrollOffset,mx));
    }
    if(e.type==sf::Event::MouseButtonPressed){
        sf::Vector2f mp((float)e.mouseButton.x,(float)e.mouseButton.y);
        if(!bounds.contains(mp))return;
        int idx=(int)((mp.y-bounds.top+scrollOffset)/itemHeight);
        if(idx>=0&&idx<(int)items.size()){
            selectedIndex=idx;
            for(int i=0;i<(int)items.size();i++) items[i].selected=(i==idx);
        }
    }
}
void ScrollPanel::draw(sf::RenderWindow& w, const sf::Font& font){
    // background
    sf::RectangleShape bg({bounds.width,bounds.height});
    bg.setPosition(bounds.left,bounds.top);
    bg.setFillColor(sf::Color(18,24,44));
    bg.setOutlineThickness(1); bg.setOutlineColor(sf::Color(40,50,76));
    w.draw(bg);

    // empty state
    if(items.empty()){
        sf::Text em; em.setFont(font); em.setString(emptyLabel);
        em.setCharacterSize(14); em.setFillColor(sf::Color(90,100,130));
        auto eb=em.getLocalBounds();
        em.setPosition(bounds.left+(bounds.width-eb.width)/2.f-eb.left,
                       bounds.top+(bounds.height-eb.height)/2.f-eb.top);
        w.draw(em); return;
    }

    float wy=bounds.top-scrollOffset;
    for(const auto& item:items){
        if(wy+itemHeight<bounds.top){wy+=itemHeight;continue;}
        if(wy>bounds.top+bounds.height){wy+=itemHeight;continue;}

        sf::RectangleShape row({bounds.width-2.f,itemHeight-2.f});
        row.setPosition(bounds.left+1.f,wy+1.f);
        row.setFillColor(item.selected?sf::Color(45,65,110):sf::Color(22,30,54));
        w.draw(row);

        if(item.selected){
            sf::RectangleShape acc({3.f,itemHeight-2.f});
            acc.setPosition(bounds.left+1.f,wy+1.f);
            acc.setFillColor(Colors::Primary); w.draw(acc);
        }

        sf::Text t1; t1.setFont(font); t1.setString(item.line1);
        t1.setCharacterSize(15); t1.setFillColor(sf::Color(225,228,240));
        t1.setPosition(bounds.left+14.f,wy+9.f); w.draw(t1);

        if(!item.line2.empty()){
            sf::Text t2; t2.setFont(font); t2.setString(item.line2);
            t2.setCharacterSize(12); t2.setFillColor(sf::Color(135,142,168));
            t2.setPosition(bounds.left+14.f,wy+32.f); w.draw(t2);
        }

        if(!item.badge.empty()){
            sf::Text tb; tb.setFont(font); tb.setString(item.badge);
            tb.setCharacterSize(11); tb.setFillColor(sf::Color::White);
            auto bb=tb.getLocalBounds();
            float bx=bounds.left+bounds.width-bb.width-26.f;
            sf::RectangleShape bkg({bb.width+14.f,20.f});
            bkg.setFillColor(item.badgeColor); bkg.setPosition(bx-4.f,wy+18.f);
            tb.setPosition(bx,wy+19.f); w.draw(bkg); w.draw(tb);
        }

        sf::RectangleShape div({bounds.width,1.f});
        div.setPosition(bounds.left,wy+itemHeight-1.f);
        div.setFillColor(sf::Color(35,44,66)); w.draw(div);
        wy+=itemHeight;
    }
}

// ── Notification ──────────────────────────────────────────────
void Notification::show(const std::string& text,bool ok,const sf::Font& font){
    isOk=ok; lifetime=maxTime; visible=true;
    sf::Color textCol =ok?sf::Color(80,210,130):sf::Color(230,80,80);
    sf::Color bordCol =ok?sf::Color(45,130,75):sf::Color(150,35,35);
    msg.setFont(font); msg.setString(text);
    msg.setCharacterSize(14); msg.setFillColor(textCol);
    auto mb=msg.getLocalBounds();
    float bw=mb.width+48.f, bh=38.f;
    float bx=L::SW+(L::CW-bw)/2.f;
    float by=L::HH+10.f;
    box.setSize({bw,bh}); box.setPosition(bx,by);
    box.setFillColor(sf::Color(20,26,46));
    outlineBox.setSize({bw+2.f,bh+2.f}); outlineBox.setPosition(bx-1.f,by-1.f);
    outlineBox.setFillColor(sf::Color::Transparent);
    outlineBox.setOutlineThickness(1.4f); outlineBox.setOutlineColor(bordCol);
    msg.setPosition(bx+24.f,by+11.f);
}
void Notification::update(float dt){if(!visible)return;lifetime-=dt;if(lifetime<=0.f)visible=false;}
void Notification::draw(sf::RenderWindow& w){
    if(!visible)return; w.draw(outlineBox); w.draw(box); w.draw(msg);
}
