#pragma once
#include "User.h"
#include "Product.h"
#include "Transaction.h"
#include <vector>
using namespace std;

class FileHandler {
public:
    // ── Users ──────────────────────────────────
    static void saveUser(const User* user);
    static vector<Customer>  loadCustomers();
    static vector<Seller>    loadSellers();
    static vector<Admin>     loadAdmins();
    static void updateSeller(const Seller& seller);

    // ── Products ───────────────────────────────
    static void saveProduct(const Product* product);
    static vector<ElectronicProduct>  loadElectronics();
    static vector<PerishableProduct>  loadPerishables();
    static vector<ClothingProduct>    loadClothing();
    static vector<ClothingProduct>    loadGeneral();   // custom-category products
    static void updateProductStock(int productId, int newStock);
    static void updateProductRating(int productId, double avgRating, int reviewCount);
    static void deleteProduct(int productId);

    // ── Orders ─────────────────────────────────
    static void saveOrder(const Order& order);
    static vector<Order> loadOrders();
    static void updateOrderStatus(int orderId, Order::Status status);

    // ── Reviews ────────────────────────────────
    static void saveReview(const Review& review);
    static vector<Review> loadReviews();
    static void flagReview(int reviewId);
    static void deleteReview(int reviewId);

    // ── Discount Codes ─────────────────────────
    static void saveDiscountCode(const DiscountCode& code);
    static vector<DiscountCode> loadDiscountCodes();
    static void deactivateDiscountCode(const string& code);

    // ── Receipts / Reports ─────────────────────
    static void saveReceipt(const Order& order, const string& customerName);
    static void saveSellerReport(int sellerId, const string& shopName,
                                  const vector<Order>& orders);

    // ── Helpers ────────────────────────────────
    static int getNextId(const string& filename);
};
