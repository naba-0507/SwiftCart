#include "../include/FileHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

// ── Helpers ───────────────────────────────────
int FileHandler::getNextId(const string& filename) {
    ifstream file(filename);
    if (!file) return 1;
    int maxId = 0, id;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string tmp;
        getline(ss, tmp, '|');
        try { id = stoi(tmp); if (id > maxId) maxId = id; } catch (...) {}
    }
    return maxId + 1;
}

// ── Users ─────────────────────────────────────
void FileHandler::saveUser(const User* user) {
    string filename = "data/" + user->getRole() + "s.txt";
    ofstream file(filename, ios::app);
    if (file) file << user->toFileString() << "\n";
}

vector<Customer> FileHandler::loadCustomers() {
    vector<Customer> list;
    ifstream file("data/customers.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, email, pass, role, address;
        getline(ss, id,      '|');
        getline(ss, name,    '|');
        getline(ss, email,   '|');
        getline(ss, pass,    '|');
        getline(ss, role,    '|');
        getline(ss, address, '|');
        list.emplace_back(stoi(id), name, email, pass, address);
    }
    return list;
}

vector<Seller> FileHandler::loadSellers() {
    vector<Seller> list;
    ifstream file("data/sellers.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, email, pass, role, shopName, approved;
        getline(ss, id,       '|');
        getline(ss, name,     '|');
        getline(ss, email,    '|');
        getline(ss, pass,     '|');
        getline(ss, role,     '|');
        getline(ss, shopName, '|');
        getline(ss, approved, '|');
        Seller s(stoi(id), name, email, pass, shopName);
        if (approved == "1") s.approve();
        list.push_back(s);
    }
    return list;
}

vector<Admin> FileHandler::loadAdmins() {
    vector<Admin> list;
    ifstream file("data/admins.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, email, pass, role;
        getline(ss, id,   '|');
        getline(ss, name, '|');
        getline(ss, email,'|');
        getline(ss, pass, '|');
        getline(ss, role, '|');
        list.emplace_back(stoi(id), name, email, pass);
    }
    return list;
}

void FileHandler::updateSeller(const Seller& seller) {
    vector<Seller> sellers = loadSellers();
    ofstream file("data/sellers.txt");
    for (auto& s : sellers) {
        if (s.getId() == seller.getId())
            file << seller.toFileString() << "\n";
        else
            file << s.toFileString() << "\n";
    }
}

// ── Products ──────────────────────────────────
void FileHandler::saveProduct(const Product* product) {
    ofstream file("data/products.txt", ios::app);
    if (file) file << product->toFileString() << "\n";
}

static vector<string> loadProductLines() {
    vector<string> lines;
    ifstream file("data/products.txt");
    string line;
    while (getline(file, line))
        if (!line.empty()) lines.push_back(line);
    return lines;
}

vector<ElectronicProduct> FileHandler::loadElectronics() {
    vector<ElectronicProduct> list;
    for (const auto& line : loadProductLines()) {
        stringstream ss(line);
        string id, sellerId, name, cat, price, stock, rating, rcount, type, brand, warranty;
        getline(ss, id,       '|'); getline(ss, sellerId, '|');
        getline(ss, name,     '|'); getline(ss, cat,      '|');
        getline(ss, price,    '|'); getline(ss, stock,    '|');
        getline(ss, rating,   '|'); getline(ss, rcount,   '|');
        getline(ss, type,     '|');
        if (type != "Electronic") continue;
        getline(ss, brand,    '|'); getline(ss, warranty, '|');
        ElectronicProduct ep(stoi(id), stoi(sellerId), name, stod(price),
                             stoi(stock), stoi(warranty), brand);
        list.push_back(ep);
    }
    return list;
}

vector<PerishableProduct> FileHandler::loadPerishables() {
    vector<PerishableProduct> list;
    for (const auto& line : loadProductLines()) {
        stringstream ss(line);
        string id, sellerId, name, cat, price, stock, rating, rcount, type, expiry, storage;
        getline(ss, id,      '|'); getline(ss, sellerId, '|');
        getline(ss, name,    '|'); getline(ss, cat,      '|');
        getline(ss, price,   '|'); getline(ss, stock,    '|');
        getline(ss, rating,  '|'); getline(ss, rcount,   '|');
        getline(ss, type,    '|');
        if (type != "Perishable") continue;
        getline(ss, expiry,  '|'); getline(ss, storage,  '|');
        PerishableProduct pp(stoi(id), stoi(sellerId), name, stod(price),
                             stoi(stock), expiry, storage);
        list.push_back(pp);
    }
    return list;
}

vector<ClothingProduct> FileHandler::loadClothing() {
    vector<ClothingProduct> list;
    for (const auto& line : loadProductLines()) {
        stringstream ss(line);
        string id, sellerId, name, cat, price, stock, rating, rcount, type, size, material;
        getline(ss, id,       '|'); getline(ss, sellerId, '|');
        getline(ss, name,     '|'); getline(ss, cat,      '|');
        getline(ss, price,    '|'); getline(ss, stock,    '|');
        getline(ss, rating,   '|'); getline(ss, rcount,   '|');
        getline(ss, type,     '|');
        if (type != "Clothing") continue;
        getline(ss, size,     '|'); getline(ss, material, '|');
        ClothingProduct cp(stoi(id), stoi(sellerId), name, stod(price),
                           stoi(stock), size, material);
        list.push_back(cp);
    }
    return list;
}

vector<ClothingProduct> FileHandler::loadGeneral() {
    // Loads all products whose type tag is not Electronic, Perishable, or Clothing.
    // These are custom-category products. We reuse ClothingProduct as the container
    // (same two generic extra fields) and call setCategory() to store the real name.
    vector<ClothingProduct> list;
    for (const auto& line : loadProductLines()) {
        stringstream ss(line);
        string id, sellerId, name, cat, price, stock, rating, rcount, type, f1, f2;
        getline(ss, id,      '|'); getline(ss, sellerId, '|');
        getline(ss, name,    '|'); getline(ss, cat,      '|');
        getline(ss, price,   '|'); getline(ss, stock,    '|');
        getline(ss, rating,  '|'); getline(ss, rcount,   '|');
        getline(ss, type,    '|');
        if (type == "Electronic" || type == "Perishable" || type == "Clothing") continue;
        getline(ss, f1, '|'); getline(ss, f2, '|');
        ClothingProduct cp(stoi(id), stoi(sellerId), name, stod(price), stoi(stock), f1, f2);
        cp.setCategory(cat);   // override "Clothing" with the real custom category name
        list.push_back(cp);
    }
    return list;
}

void FileHandler::updateProductStock(int productId, int newStock) {
    auto lines = loadProductLines();
    ofstream file("data/products.txt");
    for (auto& line : lines) {
        stringstream ss(line);
        string id; getline(ss, id, '|');
        if (stoi(id) == productId) {
            // rebuild line with new stock value
            string sellerId, name, cat, price, stock, rest, tok;
            getline(ss, sellerId,'|'); getline(ss, name,'|');
            getline(ss, cat,'|');     getline(ss, price,'|');
            getline(ss, stock,'|');   // discard old stock
            string newLine = id + "|" + sellerId + "|" + name + "|" + cat + "|" +
                             price + "|" + to_string(newStock) + "|";
            while (getline(ss, tok, '|')) newLine += tok + "|";
            file << newLine << "\n";
        } else {
            file << line << "\n";
        }
    }
}

void FileHandler::updateProductRating(int productId, double avgRating, int reviewCount) {
    auto lines = loadProductLines();
    ofstream file("data/products.txt");
    for (auto& line : lines) {
        stringstream ss(line);
        string id; getline(ss, id, '|');
        if (stoi(id) == productId) {
            // fields: id|sellerId|name|cat|price|stock|avgRating|reviewCount|type|...
            string sellerId, name, cat, price, stock, oldRating, oldCount, rest, tok;
            getline(ss, sellerId,'|'); getline(ss, name,'|');
            getline(ss, cat,'|');     getline(ss, price,'|');
            getline(ss, stock,'|');   getline(ss, oldRating,'|');
            getline(ss, oldCount,'|');
            string newLine = id + "|" + sellerId + "|" + name + "|" + cat + "|" +
                             price + "|" + stock + "|" +
                             to_string(avgRating) + "|" + to_string(reviewCount) + "|";
            while (getline(ss, tok, '|')) newLine += tok + "|";
            file << newLine << "\n";
        } else {
            file << line << "\n";
        }
    }
}

void FileHandler::deleteProduct(int productId) {
    auto lines = loadProductLines();
    ofstream file("data/products.txt");
    for (const auto& line : lines) {
        stringstream ss(line);
        string id; getline(ss, id, '|');
        if (stoi(id) != productId) file << line << "\n";
    }
}

// ── Orders ────────────────────────────────────
void FileHandler::saveOrder(const Order& order) {
    ofstream file("data/orders.txt", ios::app);
    if (file) file << order.toFileString() << "\n";
}

vector<Order> FileHandler::loadOrders() {
    vector<Order> list;
    ifstream file("data/orders.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string ordId, custId, sellId, itemsStr, total, statusStr, date, history;
        getline(ss, ordId,    '|'); getline(ss, custId,  '|');
        getline(ss, sellId,   '|'); getline(ss, itemsStr,'|');
        getline(ss, total,    '|'); getline(ss, statusStr,'|');
        getline(ss, date,     '|'); getline(ss, history, '|');

        vector<OrderItem> items;
        stringstream iss(itemsStr);
        string itemStr;
        while (getline(iss, itemStr, ';')) {
            if (itemStr.empty()) continue;
            stringstream istream(itemStr);
            string pid, pname, uprice, qty, sub;
            getline(istream, pid,    ':'); getline(istream, pname,  ':');
            getline(istream, uprice, ':'); getline(istream, qty,    ':');
            getline(istream, sub,    ':');
            OrderItem oi;
            oi.productId   = stoi(pid);
            oi.productName = pname;
            oi.unitPrice   = stod(uprice);
            oi.quantity    = stoi(qty);
            oi.subtotal    = stod(sub);
            items.push_back(oi);
        }
        Order o(stoi(ordId), stoi(custId), stoi(sellId), items, stod(total), date);
        o.updateStatus((Order::Status)stoi(statusStr));
        list.push_back(o);
    }
    return list;
}

void FileHandler::updateOrderStatus(int orderId, Order::Status status) {
    vector<Order> orders = loadOrders();
    ofstream file("data/orders.txt");
    for (auto& o : orders) {
        if (o.getOrderId() == orderId) o.updateStatus(status);
        file << o.toFileString() << "\n";
    }
}

// ── Reviews ───────────────────────────────────
void FileHandler::saveReview(const Review& review) {
    ofstream file("data/reviews.txt", ios::app);
    if (file) file << review.toFileString() << "\n";
}

vector<Review> FileHandler::loadReviews() {
    vector<Review> list;
    ifstream file("data/reviews.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string revId, prodId, custId, custName, rating, comment, date, flagged;
        getline(ss, revId,    '|'); getline(ss, prodId,   '|');
        getline(ss, custId,   '|'); getline(ss, custName, '|');
        getline(ss, rating,   '|'); getline(ss, comment,  '|');
        getline(ss, date,     '|'); getline(ss, flagged,  '|');
        Review r(stoi(revId), stoi(prodId), stoi(custId),
                 custName, stoi(rating), comment, date);
        if (flagged == "1") r.flag();
        list.push_back(r);
    }
    return list;
}

void FileHandler::flagReview(int reviewId) {
    vector<Review> reviews = loadReviews();
    ofstream file("data/reviews.txt");
    for (auto& r : reviews) {
        if (r.getReviewId() == reviewId) r.flag();
        file << r.toFileString() << "\n";
    }
}

void FileHandler::deleteReview(int reviewId) {
    vector<Review> reviews = loadReviews();
    ofstream file("data/reviews.txt");
    for (const auto& r : reviews)
        if (r.getReviewId() != reviewId)
            file << r.toFileString() << "\n";
}

// ── Discount Codes ────────────────────────────
void FileHandler::saveDiscountCode(const DiscountCode& code) {
    ofstream file("data/discounts.txt", ios::app);
    if (file) file << code.toFileString() << "\n";
}

vector<DiscountCode> FileHandler::loadDiscountCodes() {
    vector<DiscountCode> list;
    ifstream file("data/discounts.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string code, type, value, minOrder, active;
        getline(ss, code,     '|'); getline(ss, type,     '|');
        getline(ss, value,    '|'); getline(ss, minOrder, '|');
        getline(ss, active,   '|');
        DiscountCode dc(code, (DiscountCode::Type)stoi(type),
                        stod(value), stod(minOrder));
        if (active == "0") dc.deactivate();
        list.push_back(dc);
    }
    return list;
}

void FileHandler::deactivateDiscountCode(const string& code) {
    vector<DiscountCode> codes = loadDiscountCodes();
    ofstream file("data/discounts.txt");
    for (auto& dc : codes) {
        if (dc.getCode() == code) dc.deactivate();
        file << dc.toFileString() << "\n";
    }
}

// ── Receipt ───────────────────────────────────
void FileHandler::saveReceipt(const Order& order, const string& customerName) {
    string filename = "data/receipt_order" + to_string(order.getOrderId()) + ".txt";
    ofstream file(filename);
    if (!file) return;
    file << fixed << setprecision(2);
    file << "============== SWIFTCART RECEIPT ==============\n"
         << "Order ID   : " << order.getOrderId()    << "\n"
         << "Customer   : " << customerName           << "\n"
         << "Date       : " << order.getOrderDate()   << "\n"
         << "-------------------------------------------\n";
    for (const auto& item : order.getItems()) {
        file << left << setw(20) << item.productName
             << " x" << item.quantity
             << "   Rs." << item.subtotal << "\n";
    }
    file << "-------------------------------------------\n"
         << "TOTAL      : Rs." << order.getTotalAmount() << "\n"
         << "===========================================\n";
    cout << "Receipt saved: " << filename << "\n";
}

void FileHandler::saveSellerReport(int sellerId, const string& shopName,
                                    const vector<Order>& orders) {
    string filename = "data/report_seller" + to_string(sellerId) + ".txt";
    ofstream file(filename);
    if (!file) return;
    double totalRevenue = 0;
    int totalOrders = 0;
    file << "======= SALES REPORT: " << shopName << " =======\n";
    for (const auto& o : orders) {
        if (o.getSellerId() != sellerId) continue;
        file << "Order #" << o.getOrderId()
             << "  Date: " << o.getOrderDate()
             << "  Rs." << fixed << setprecision(2) << o.getTotalAmount()
             << "  [" << o.getStatusStr() << "]\n";
        totalRevenue += o.getTotalAmount();
        totalOrders++;
    }
    file << "----------------------------------------------\n"
         << "Total Orders  : " << totalOrders  << "\n"
         << "Total Revenue : Rs." << totalRevenue << "\n"
         << "==============================================\n";
    cout << "Report saved: " << filename << "\n";
}
