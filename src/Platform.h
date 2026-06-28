#pragma once
#include "User.h"
#include "Product.h"
#include "Transaction.h"
#include "FileHandler.h"
#include <vector>
#include <memory>
using namespace std;

// Central engine — 

class Platform {
private:
    // In-memory stores
    vector<Customer>          customers;
    vector<Seller>            sellers;
    vector<Admin>             admins;
    vector<ElectronicProduct> electronics;
    vector<PerishableProduct> perishables;
    vector<ClothingProduct>   clothing;
    vector<ClothingProduct>   generalProducts; // custom-category products
    vector<Order>             orders;
    vector<Review>            reviews;
    vector<DiscountCode>      discountCodes;

    // Active session
    string  activeRole;   // "customer" | "seller" | "admin" | ""
    int     activeUserId;

    void loadAll();

public:
    Platform();

    // ── Auth ───────────────────────────────────
    bool registerCustomer(string name, string email, string password, string address);
    bool registerSeller(string name, string email, string password, string shopName);
    bool login(string email, string password); // sets activeRole + activeUserId
    void logout();

    string getActiveRole() const;
    int    getActiveUserId() const;
    Customer* getActiveCustomer();
    Seller*   getActiveSeller();
    Admin*    getActiveAdmin();

    // ── Product browsing (any user) ────────────
    vector<Product*> getAllProducts();                          // returns ptrs to in-mem products
    vector<Product*> searchByName(const string& keyword);
    vector<Product*> filterByCategory(const string& category);
    Product*         getProductById(int id);

    // ── Customer actions ───────────────────────
    Cart&   getCart(int customerId);                           // loads cart from file
    bool    placeOrder(int customerId, Cart& cart);
    vector<Order> getCustomerOrders(int customerId);
    bool    submitReview(int customerId, string customerName,
                         int productId, int rating, string comment);
    bool    applyDiscountCode(Cart& cart, const string& code);

    // ── Seller actions ─────────────────────────
    bool    addProduct(Product* product);
    void    reloadProducts();  // reload all product vectors from file
    bool    editProduct(int productId, double newPrice, int newStock);
    bool    removeProduct(int productId);
    vector<Order>   getSellerOrders(int sellerId);
    bool    updateOrderStatus(int orderId, Order::Status newStatus);
    void    generateSalesReport(int sellerId);

    // ── Admin actions ──────────────────────────
    vector<Seller>  getPendingSellers();
    vector<Seller>   getAllSellers();
    vector<Customer> getAllCustomers();
    vector<Order>    getAllOrders();
    bool    approveSeller(int sellerId);
    bool    banSeller(int sellerId);
    bool    addDiscountCode(string code, DiscountCode::Type type,
                            double value, double minOrder);
    bool    removeDiscountCode(const string& code);
    vector<Review>  getAllReviews();
    bool    flagReview(int reviewId);
    bool    deleteReview(int reviewId);
    void    displayPlatformStats();

private:
    vector<Cart> carts; // one per customer, loaded
    Cart* findOrLoadCart(int customerId);
};
