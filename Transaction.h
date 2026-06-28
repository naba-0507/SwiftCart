#pragma once
#include <string>
#include <vector>
using namespace std;

// ─────────────────────────────────────────────
struct CartItem {
    int productId;
    string productName;
    double unitPrice;
    int quantity;
    double subtotal;
};

class Cart {
private:
    int customerId;
    vector<CartItem> items;
    string discountCode;
    double discountAmount;

public:
    Cart();
    Cart(int customerId);

    int getCustomerId() const;
    vector<CartItem> getItems() const;
    double getDiscountAmount() const;
    string getDiscountCode() const;

    void addItem(int productId, string productName, double price, int qty);
    void removeItem(int productId);
    void updateQuantity(int productId, int qty);
    void applyDiscount(double amount, string code);
    void clearCart();

    double getSubtotal() const;
    double getTotal() const;

    void display() const;
    void saveToFile() const;
    void loadFromFile(int customerId);
};

// ─────────────────────────────────────────────
struct OrderItem {
    int productId;
    string productName;
    double unitPrice;
    int quantity;
    double subtotal;
};

class Order {
public:
    enum Status { PENDING, CONFIRMED, SHIPPED, DELIVERED, CANCELLED };

private:
    int orderId;
    int customerId;
    int sellerId;
    vector<OrderItem> items;
    double totalAmount;
    Status status;
    string orderDate;
    string statusHistory;

public:
    Order();
    Order(int orderId, int customerId, int sellerId,
          vector<OrderItem> items, double total, string date);

    int getOrderId() const;
    int getCustomerId() const;
    int getSellerId() const;
    double getTotalAmount() const;
    Status getStatus() const;
    string getStatusStr() const;
    string getOrderDate() const;
    vector<OrderItem> getItems() const;

    void updateStatus(Status newStatus);
    void cancel();

    void displayOrder() const;
    void printReceipt() const; // saves formatted receipt to file
    string toFileString() const;
};

// ─────────────────────────────────────────────
class Review {
private:
    int reviewId;
    int productId;
    int customerId;
    string customerName;
    int rating;      // 1-5
    string comment;
    string date;
    bool isFlagged;

public:
    Review();
    Review(int reviewId, int productId, int customerId,
           string customerName, int rating, string comment, string date);

    int getReviewId() const;
    int getProductId() const;
    int getCustomerId() const;
    string getCustomerName() const;   // for GUI
    int getRating() const;
    string getComment() const;
    string getDate() const;           // for GUI
    bool getIsFlagged() const;

    void flag();
    void display() const;
    string toFileString() const;
};

// ─────────────────────────────────────────────
class DiscountCode {
public:
    enum Type { PERCENTAGE, FLAT };

private:
    string code;
    Type type;
    double value;       //10% off or Rs.10 off
    double minOrderAmt;
    bool isActive;

public:
    DiscountCode();
    DiscountCode(string code, Type type, double value, double minOrderAmt);

    string getCode() const;
    Type getType() const;
    double getValue() const;
    double getMinOrderAmt() const;
    bool getIsActive() const;

    double calculateDiscount(double orderTotal) const;
    void deactivate();

    void display() const;
    string toFileString() const;
};
