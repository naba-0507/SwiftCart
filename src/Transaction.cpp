#include "../include/Transaction.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

// ═══════════════════════════════════════════════
//  CART
// ═══════════════════════════════════════════════
Cart::Cart() : customerId(0), discountCode(""), discountAmount(0.0) {}
Cart::Cart(int customerId) : customerId(customerId), discountCode(""), discountAmount(0.0) {}

int             Cart::getCustomerId()    const { return customerId; }
vector<CartItem> Cart::getItems()        const { return items; }
double          Cart::getDiscountAmount() const { return discountAmount; }
string          Cart::getDiscountCode()  const { return discountCode; }

void Cart::addItem(int productId, string productName, double price, int qty) {
    for (auto& item : items) {
        if (item.productId == productId) {
            item.quantity += qty;
            item.subtotal = item.unitPrice * item.quantity;
            saveToFile();
            return;
        }
    }
    CartItem ci;
    ci.productId   = productId;
    ci.productName = productName;
    ci.unitPrice   = price;
    ci.quantity    = qty;
    ci.subtotal    = price * qty;
    items.push_back(ci);
    saveToFile();
}

void Cart::removeItem(int productId) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->productId == productId) {
            items.erase(it);
            saveToFile();
            return;
        }
    }
}

void Cart::updateQuantity(int productId, int qty) {
    for (auto& item : items) {
        if (item.productId == productId) {
            item.quantity = qty;
            item.subtotal = item.unitPrice * qty;
            saveToFile();
            return;
        }
    }
}

void Cart::applyDiscount(double amount, string code) {
    discountAmount = amount;
    discountCode   = code;
    saveToFile();
}

void Cart::clearCart() {
    items.clear();
    discountAmount = 0.0;
    discountCode   = "";
    saveToFile();
}

double Cart::getSubtotal() const {
    double total = 0;
    for (const auto& item : items) total += item.subtotal;
    return total;
}

double Cart::getTotal() const {
    return max(0.0, getSubtotal() - discountAmount);
}

void Cart::display() const {
    cout << fixed << setprecision(2);
    cout << "\n===== CART =====\n";
    if (items.empty()) { cout << "Your cart is empty.\n"; return; }
    for (const auto& item : items) {
        cout << "  " << item.productName
             << " x" << item.quantity
             << "  Rs." << item.subtotal << "\n";
    }
    cout << "----------------\n";
    cout << "Subtotal : Rs." << getSubtotal() << "\n";
    if (discountAmount > 0)
        cout << "Discount : -Rs." << discountAmount
             << " (" << discountCode << ")\n";
    cout << "TOTAL    : Rs." << getTotal() << "\n";
    cout << "================\n";
}

void Cart::saveToFile() const {
    string filename = "data/cart_" + to_string(customerId) + ".txt";
    ofstream file(filename);
    if (!file) return;
    file << customerId << "\n";
    file << discountCode << "\n";
    file << discountAmount << "\n";
    for (const auto& item : items) {
        file << item.productId << "|"
             << item.productName << "|"
             << item.unitPrice   << "|"
             << item.quantity    << "|"
             << item.subtotal    << "\n";
    }
}

void Cart::loadFromFile(int cid) {
    customerId = cid;
    items.clear();
    string filename = "data/cart_" + to_string(cid) + ".txt";
    ifstream file(filename);
    if (!file) return;
    string line;
    getline(file, line); // customerId
    getline(file, discountCode);
    file >> discountAmount;
    file.ignore();
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        CartItem ci;
        string tmp;
        getline(ss, tmp, '|'); ci.productId   = stoi(tmp);
        getline(ss, ci.productName, '|');
        getline(ss, tmp, '|'); ci.unitPrice   = stod(tmp);
        getline(ss, tmp, '|'); ci.quantity    = stoi(tmp);
        getline(ss, tmp, '|'); ci.subtotal    = stod(tmp);
        items.push_back(ci);
    }
}

// ═══════════════════════════════════════════════
//  ORDER
// ═══════════════════════════════════════════════
Order::Order()
    : orderId(0), customerId(0), sellerId(0),
      totalAmount(0.0), status(PENDING), orderDate(""), statusHistory("") {}

Order::Order(int orderId, int customerId, int sellerId,
             vector<OrderItem> items, double total, string date)
    : orderId(orderId), customerId(customerId), sellerId(sellerId),
      items(items), totalAmount(total), status(PENDING),
      orderDate(date), statusHistory("PENDING@" + date) {}

int    Order::getOrderId()     const { return orderId; }
int    Order::getCustomerId()  const { return customerId; }
int    Order::getSellerId()    const { return sellerId; }
double Order::getTotalAmount() const { return totalAmount; }
Order::Status Order::getStatus() const { return status; }
string Order::getOrderDate()   const { return orderDate; }
vector<OrderItem> Order::getItems() const { return items; }

string Order::getStatusStr() const {
    switch (status) {
        case PENDING:   return "Pending";
        case CONFIRMED: return "Confirmed";
        case SHIPPED:   return "Shipped";
        case DELIVERED: return "Delivered";
        case CANCELLED: return "Cancelled";
        default:        return "Unknown";
    }
}

void Order::updateStatus(Status newStatus) {
    status = newStatus;
    statusHistory += " -> " + getStatusStr();
}

void Order::cancel() {
    status = CANCELLED;
    statusHistory += " -> Cancelled";
}

void Order::displayOrder() const {
    cout << fixed << setprecision(2);
    cout << "\n===== ORDER #" << orderId << " =====\n"
         << "Date   : " << orderDate    << "\n"
         << "Status : " << getStatusStr() << "\n"
         << "Items  :\n";
    for (const auto& item : items) {
        cout << "  " << item.productName
             << " x" << item.quantity
             << "  Rs." << item.subtotal << "\n";
    }
    cout << "TOTAL  : Rs." << totalAmount << "\n";
    cout << "History: " << statusHistory  << "\n";
    cout << "=====================\n";
}

void Order::printReceipt() const {
    string filename = "data/receipt_" + to_string(orderId) + ".txt";
    ofstream file(filename);
    if (!file) return;
    file << "============== SWIFTCART RECEIPT ==============\n"
         << "Order ID : " << orderId   << "\n"
         << "Date     : " << orderDate << "\n"
         << "Status   : " << getStatusStr() << "\n"
         << "-------------------------------------------\n";
    for (const auto& item : items) {
        file << item.productName
             << " x" << item.quantity
             << "  Rs." << fixed << setprecision(2) << item.subtotal << "\n";
    }
    file << "-------------------------------------------\n"
         << "TOTAL    : Rs." << fixed << setprecision(2) << totalAmount << "\n"
         << "===========================================\n";
    cout << "Receipt saved to " << filename << "\n";
}

string Order::toFileString() const {
    string itemsStr = "";
    for (const auto& item : items) {
        itemsStr += to_string(item.productId) + ":" +
                    item.productName + ":" +
                    to_string(item.unitPrice) + ":" +
                    to_string(item.quantity) + ":" +
                    to_string(item.subtotal) + ";";
    }
    return to_string(orderId) + "|" + to_string(customerId) + "|" +
           to_string(sellerId) + "|" + itemsStr + "|" +
           to_string(totalAmount) + "|" + to_string((int)status) + "|" +
           orderDate + "|" + statusHistory;
}

// ═══════════════════════════════════════════════
//  REVIEW
// ═══════════════════════════════════════════════
Review::Review()
    : reviewId(0), productId(0), customerId(0),
      customerName(""), rating(0), comment(""), date(""), isFlagged(false) {}

Review::Review(int reviewId, int productId, int customerId,
               string customerName, int rating, string comment, string date)
    : reviewId(reviewId), productId(productId), customerId(customerId),
      customerName(customerName), rating(rating), comment(comment),
      date(date), isFlagged(false) {}

int    Review::getReviewId()    const { return reviewId; }
string Review::getCustomerName() const { return customerName; }
string Review::getDate()         const { return date; }
int    Review::getProductId()   const { return productId; }
int    Review::getCustomerId()  const { return customerId; }
int    Review::getRating()      const { return rating; }
string Review::getComment()     const { return comment; }
bool   Review::getIsFlagged()   const { return isFlagged; }

void Review::flag() { isFlagged = true; }

void Review::display() const {
    cout << "  [" << reviewId << "] "
         << customerName << " - " << rating << "/5\n"
         << "  " << comment << "\n"
         << "  Date: " << date
         << (isFlagged ? "  [FLAGGED]" : "") << "\n";
}

string Review::toFileString() const {
    return to_string(reviewId) + "|" + to_string(productId) + "|" +
           to_string(customerId) + "|" + customerName + "|" +
           to_string(rating) + "|" + comment + "|" + date + "|" +
           (isFlagged ? "1" : "0");
}

// ═══════════════════════════════════════════════
//  DISCOUNT CODE
// ═══════════════════════════════════════════════
DiscountCode::DiscountCode()
    : code(""), type(FLAT), value(0.0), minOrderAmt(0.0), isActive(false) {}

DiscountCode::DiscountCode(string code, Type type, double value, double minOrderAmt)
    : code(code), type(type), value(value),
      minOrderAmt(minOrderAmt), isActive(true) {}

string         DiscountCode::getCode()         const { return code; }
DiscountCode::Type DiscountCode::getType()     const { return type; }
double         DiscountCode::getValue()        const { return value; }
double         DiscountCode::getMinOrderAmt()  const { return minOrderAmt; }
bool           DiscountCode::getIsActive()     const { return isActive; }

double DiscountCode::calculateDiscount(double orderTotal) const {
    if (!isActive || orderTotal < minOrderAmt) return 0.0;
    if (type == PERCENTAGE) return orderTotal * (value / 100.0);
    return value;
}

void DiscountCode::deactivate() { isActive = false; }

void DiscountCode::display() const {
    cout << "  Code: " << code
         << "  Type: " << (type == PERCENTAGE ? "%" : "Flat")
         << "  Value: " << value
         << "  Min Order: Rs." << minOrderAmt
         << "  Status: " << (isActive ? "Active" : "Inactive") << "\n";
}

string DiscountCode::toFileString() const {
    return code + "|" + to_string((int)type) + "|" +
           to_string(value) + "|" + to_string(minOrderAmt) + "|" +
           (isActive ? "1" : "0");
}
