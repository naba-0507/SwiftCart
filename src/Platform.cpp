#include "../include/Platform.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;

// ── Helper: get today's date as string ────────
static string getToday() {
    time_t t = time(nullptr);
    tm* tm_info = localtime(&t);
    ostringstream ss;
    ss << put_time(tm_info, "%Y-%m-%d");
    return ss.str();
}

// ─────────────────────────────────────────────
Platform::Platform() : activeRole(""), activeUserId(-1) {
    // Create data directory files if they don't exist
    vector<string> files = {
        "data/customers.txt", "data/sellers.txt", "data/admins.txt",
        "data/products.txt",  "data/orders.txt",  "data/reviews.txt",
        "data/discounts.txt"
    };
    for (const auto& f : files) {
        ofstream file(f, ios::app); // creates if not exists
    }
    loadAll();

    // Seed a default admin if none exists
    if (admins.empty()) {
        Admin defaultAdmin(1, "Admin", "admin@shopx.com", "admin123");
        FileHandler::saveUser(&defaultAdmin);
        admins.push_back(defaultAdmin);
        cout << "[SwiftCart/ShopX] Default admin created: admin@shopx.com / admin123\n";
    }
}

void Platform::loadAll() {
    customers   = FileHandler::loadCustomers();
    sellers     = FileHandler::loadSellers();
    admins      = FileHandler::loadAdmins();
    electronics = FileHandler::loadElectronics();
    perishables = FileHandler::loadPerishables();
    clothing         = FileHandler::loadClothing();
    generalProducts  = FileHandler::loadGeneral();
    orders      = FileHandler::loadOrders();
    reviews     = FileHandler::loadReviews();
    discountCodes = FileHandler::loadDiscountCodes();
}

// ── Auth ──────────────────────────────────────
bool Platform::registerCustomer(string name, string email, string password, string address) {
    for (const auto& c : customers)
        if (c.getEmail() == email) return false;
    int id = FileHandler::getNextId("data/customers.txt");
    Customer c(id, name, email, password, address);
    FileHandler::saveUser(&c);
    customers.push_back(c);
    return true;
}

bool Platform::registerSeller(string name, string email, string password, string shopName) {
    for (const auto& s : sellers)
        if (s.getEmail() == email) return false;
    int id = FileHandler::getNextId("data/sellers.txt");
    Seller s(id, name, email, password, shopName);
    FileHandler::saveUser(&s);
    sellers.push_back(s);
    return true;
}

bool Platform::login(string email, string password) {
    for (auto& c : customers) {
        if (c.getEmail() == email && c.getPassword() == password) {
            activeRole   = "customer";
            activeUserId = c.getId();
            return true;
        }
    }
    for (auto& s : sellers) {
        if (s.getEmail() == email && s.getPassword() == password) {
            if (!s.getIsApproved()) {
                cout << "Your seller account is pending admin approval.\n";
                return false;
            }
            activeRole   = "seller";
            activeUserId = s.getId();
            return true;
        }
    }
    for (auto& a : admins) {
        if (a.getEmail() == email && a.getPassword() == password) {
            activeRole   = "admin";
            activeUserId = a.getId();
            return true;
        }
    }
    return false;
}

void Platform::logout() {
    activeRole   = "";
    activeUserId = -1;
}

string Platform::getActiveRole()   const { return activeRole; }
int    Platform::getActiveUserId() const { return activeUserId; }

Customer* Platform::getActiveCustomer() {
    for (auto& c : customers)
        if (c.getId() == activeUserId) return &c;
    return nullptr;
}

Seller* Platform::getActiveSeller() {
    for (auto& s : sellers)
        if (s.getId() == activeUserId) return &s;
    return nullptr;
}

Admin* Platform::getActiveAdmin() {
    for (auto& a : admins)
        if (a.getId() == activeUserId) return &a;
    return nullptr;
}

// ── Product browsing ──────────────────────────
vector<Product*> Platform::getAllProducts() {
    vector<Product*> all;
    for (auto& p : electronics) if (p.getStock() > 0) all.push_back(&p);
    for (auto& p : perishables) if (p.getStock() > 0) all.push_back(&p);
    for (auto& p : clothing)    if (p.getStock() > 0) all.push_back(&p);
    for (auto& p : generalProducts) if (p.getStock() > 0) all.push_back(&p);
    return all;
}

vector<Product*> Platform::searchByName(const string& keyword) {
    vector<Product*> results;
    string kw = keyword;
    transform(kw.begin(), kw.end(), kw.begin(), ::tolower);
    for (auto* p : getAllProducts()) {
        string name = p->getName();
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.find(kw) != string::npos) results.push_back(p);
    }
    return results;
}

vector<Product*> Platform::filterByCategory(const string& category) {
    vector<Product*> results;
    for (auto* p : getAllProducts())
        if (p->getCategory() == category) results.push_back(p);
    return results;
}

Product* Platform::getProductById(int id) {
    for (auto& p : electronics) if (p.getId() == id) return &p;
    for (auto& p : perishables) if (p.getId() == id) return &p;
    for (auto& p : clothing)    if (p.getId() == id) return &p;
    for (auto& p : generalProducts) if (p.getId() == id) return &p;
    return nullptr;
}

// ── Cart ──────────────────────────────────────
Cart* Platform::findOrLoadCart(int customerId) {
    for (auto& c : carts)
        if (c.getCustomerId() == customerId) return &c;
    Cart newCart(customerId);
    newCart.loadFromFile(customerId);
    carts.push_back(newCart);
    return &carts.back();
}

Cart& Platform::getCart(int customerId) {
    return *findOrLoadCart(customerId);
}

bool Platform::applyDiscountCode(Cart& cart, const string& code) {
    for (auto& dc : discountCodes) {
        if (dc.getCode() == code && dc.getIsActive()) {
            double discount = dc.calculateDiscount(cart.getSubtotal());
            if (discount == 0.0) {
                cout << "Minimum order amount not met for this code.\n";
                return false;
            }
            cart.applyDiscount(discount, code);
            cout << "Discount applied! You save Rs." << discount << "\n";
            return true;
        }
    }
    cout << "Invalid or expired discount code.\n";
    return false;
}

// ── Place Order ───────────────────────────────
bool Platform::placeOrder(int customerId, Cart& cart) {
    if (cart.getItems().empty()) {
        cout << "Cart is empty.\n";
        return false;
    }

    // Validate stock for all items
    for (const auto& item : cart.getItems()) {
        Product* p = getProductById(item.productId);
        if (!p || p->getStock() < item.quantity) {
            cout << "Insufficient stock for: " << item.productName << "\n";
            return false;
        }
    }

    // Group items by seller (simplified: use first item's seller)
    // In a full system you'd split orders per seller
    int sellerId = -1;
    Product* firstProduct = getProductById(cart.getItems()[0].productId);
    if (firstProduct) sellerId = firstProduct->getSellerId();

    // Build order items
    vector<OrderItem> orderItems;
    for (const auto& ci : cart.getItems()) {
        OrderItem oi;
        oi.productId   = ci.productId;
        oi.productName = ci.productName;
        oi.unitPrice   = ci.unitPrice;
        oi.quantity    = ci.quantity;
        oi.subtotal    = ci.subtotal;
        orderItems.push_back(oi);
    }

    int orderId = FileHandler::getNextId("data/orders.txt");
    Order order(orderId, customerId, sellerId, orderItems, cart.getTotal(), getToday());
    FileHandler::saveOrder(order);
    orders.push_back(order);

    // Decrement stock
    for (const auto& item : cart.getItems()) {
        Product* p = getProductById(item.productId);
        if (p) {
            p->decrementStock(item.quantity);
            FileHandler::updateProductStock(p->getId(), p->getStock());
        }
    }

    // Save receipt and clear cart
    Customer* customer = getActiveCustomer();
    string custName = customer ? customer->getName() : "Customer";
    FileHandler::saveReceipt(order, custName);
    cart.clearCart();

    cout << "Order #" << orderId << " placed successfully!\n";
    return true;
}

vector<Order> Platform::getCustomerOrders(int customerId) {
    vector<Order> result;
    for (const auto& o : orders)
        if (o.getCustomerId() == customerId) result.push_back(o);
    return result;
}

bool Platform::submitReview(int customerId, string customerName,
                             int productId, int rating, string comment) {
    // Check if customer has ordered this product
    bool hasPurchased = false;
    for (const auto& o : orders) {
        if (o.getCustomerId() != customerId) continue;
        for (const auto& item : o.getItems()) {
            if (item.productId == productId) { hasPurchased = true; break; }
        }
        if (hasPurchased) break;
    }
    if (!hasPurchased) {
        cout << "You can only review products you have purchased.\n";
        return false;
    }

    int reviewId = FileHandler::getNextId("data/reviews.txt");
    Review r(reviewId, productId, customerId, customerName, rating, comment, getToday());
    FileHandler::saveReview(r);
    reviews.push_back(r);

    // Update product rating
    Product* p = getProductById(productId);
    if (p) {
        p->updateRating(rating);
        FileHandler::updateProductRating(productId, p->getAvgRating(), p->getReviewCount());
    }
    cout << "Review submitted!\n";
    return true;
}

// ── Seller actions ────────────────────────────
void Platform::reloadProducts() {
    electronics     = FileHandler::loadElectronics();
    perishables     = FileHandler::loadPerishables();
    clothing        = FileHandler::loadClothing();
    generalProducts = FileHandler::loadGeneral();
}

bool Platform::addProduct(Product* product) {
    FileHandler::saveProduct(product);
    // Reload to keep in-memory store fresh
    electronics = FileHandler::loadElectronics();
    perishables = FileHandler::loadPerishables();
    clothing         = FileHandler::loadClothing();
    generalProducts  = FileHandler::loadGeneral();
    cout << "Product listed successfully!\n";
    return true;
}

bool Platform::editProduct(int productId, double newPrice, int newStock) {
    Product* p = getProductById(productId);
    if (!p) { cout << "Product not found.\n"; return false; }
    if (p->getSellerId() != activeUserId) { cout << "Unauthorized.\n"; return false; }
    p->setPrice(newPrice);
    p->setStock(newStock);
    FileHandler::updateProductStock(productId, newStock);
    cout << "Product updated.\n";
    return true;
}

bool Platform::removeProduct(int productId) {
    Product* p = getProductById(productId);
    if (!p) { cout << "Product not found.\n"; return false; }
    if (p->getSellerId() != activeUserId) { cout << "Unauthorized.\n"; return false; }
    FileHandler::deleteProduct(productId);
    electronics = FileHandler::loadElectronics();
    perishables = FileHandler::loadPerishables();
    clothing         = FileHandler::loadClothing();
    generalProducts  = FileHandler::loadGeneral();
    cout << "Product removed.\n";
    return true;
}

vector<Order> Platform::getSellerOrders(int sellerId) {
    vector<Order> result;
    for (const auto& o : orders)
        if (o.getSellerId() == sellerId) result.push_back(o);
    return result;
}

bool Platform::updateOrderStatus(int orderId, Order::Status newStatus) {
    for (auto& o : orders) {
        if (o.getOrderId() == orderId && o.getSellerId() == activeUserId) {
            o.updateStatus(newStatus);
            FileHandler::updateOrderStatus(orderId, newStatus);
            cout << "Order #" << orderId << " status updated to " << o.getStatusStr() << "\n";
            return true;
        }
    }
    cout << "Order not found or unauthorized.\n";
    return false;
}

void Platform::generateSalesReport(int sellerId) {
    Seller* s = getActiveSeller();
    string shopName = s ? s->getShopName() : "Seller";
    FileHandler::saveSellerReport(sellerId, shopName, orders);
}

// ── Admin actions ─────────────────────────────
vector<Seller>   Platform::getAllSellers()    { return sellers; }
vector<Customer> Platform::getAllCustomers()  { return customers; }
vector<Order>    Platform::getAllOrders()     { return orders; }

vector<Seller> Platform::getPendingSellers() {
    vector<Seller> result;
    for (const auto& s : sellers)
        if (!s.getIsApproved()) result.push_back(s);
    return result;
}

bool Platform::approveSeller(int sellerId) {
    for (auto& s : sellers) {
        if (s.getId() == sellerId) {
            s.approve();
            FileHandler::updateSeller(s);
            cout << "Seller '" << s.getShopName() << "' approved.\n";
            return true;
        }
    }
    return false;
}

bool Platform::banSeller(int sellerId) {
    for (auto& s : sellers) {
        if (s.getId() == sellerId) {
            s.ban();
            FileHandler::updateSeller(s);
            cout << "Seller '" << s.getShopName() << "' banned.\n";
            return true;
        }
    }
    return false;
}

bool Platform::addDiscountCode(string code, DiscountCode::Type type,
                                double value, double minOrder) {
    for (const auto& dc : discountCodes)
        if (dc.getCode() == code) { cout << "Code already exists.\n"; return false; }
    DiscountCode dc(code, type, value, minOrder);
    FileHandler::saveDiscountCode(dc);
    discountCodes.push_back(dc);
    cout << "Discount code '" << code << "' added.\n";
    return true;
}

bool Platform::removeDiscountCode(const string& code) {
    FileHandler::deactivateDiscountCode(code);
    for (auto& dc : discountCodes)
        if (dc.getCode() == code) { dc.deactivate(); return true; }
    return false;
}

vector<Review> Platform::getAllReviews() { return reviews; }

bool Platform::flagReview(int reviewId) {
    for (auto& r : reviews) {
        if (r.getReviewId() == reviewId) {
            r.flag();
            FileHandler::flagReview(reviewId);
            return true;
        }
    }
    return false;
}

bool Platform::deleteReview(int reviewId) {
    FileHandler::deleteReview(reviewId);
    reviews = FileHandler::loadReviews();
    return true;
}

void Platform::displayPlatformStats() {
    double totalRevenue = 0;
    for (const auto& o : orders) totalRevenue += o.getTotalAmount();

    cout << "\n====== PLATFORM STATS ======\n"
         << "Customers    : " << customers.size()     << "\n"
         << "Sellers      : " << sellers.size()       << "\n"
         << "Products     : " << getAllProducts().size() << "\n"
         << "Total Orders : " << orders.size()        << "\n"
         << "Total Revenue: Rs." << fixed << setprecision(2) << totalRevenue << "\n"
         << "============================\n";
}
