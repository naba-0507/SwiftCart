#pragma once
#include <string>
using namespace std;

class Product {
protected:
    int id;
    int sellerId;
    string name;
    string category;
    double price;
    int stock;
    double avgRating;
    int reviewCount;

public:
    Product();
    Product(int id, int sellerId, string name, string category, double price, int stock);
    virtual ~Product() {}

    int getId() const;
    int getSellerId() const;
    string getName() const;
    string getCategory() const;
    double getPrice() const;
    int getStock() const;
    double getAvgRating() const;
    int getReviewCount() const;

    void setPrice(double price);
    void setStock(int stock);
    void setCategory(string category);
    void decrementStock(int qty);
    void updateRating(double newRating);

    virtual void displayDetails() const;
    virtual string getType() const { return "General"; }
    virtual string toFileString() const;
};

// ─────────────────────────────────────────────
class ElectronicProduct : public Product {
private:
    int warrantyMonths;
    string brand;

public:
    ElectronicProduct();
    ElectronicProduct(int id, int sellerId, string name, double price,
                      int stock, int warrantyMonths, string brand);

    int getWarrantyMonths() const;
    string getBrand() const;

    void displayDetails() const override;
    string getType() const override { return "Electronic"; }
    string toFileString() const override;
};

// ─────────────────────────────────────────────
class PerishableProduct : public Product {
private:
    string expiryDate;
    string storageCondition;

public:
    PerishableProduct();
    PerishableProduct(int id, int sellerId, string name, double price,
                      int stock, string expiryDate, string storageCondition);

    string getExpiryDate() const;
    string getStorageCondition() const;

    void displayDetails() const override;
    string getType() const override { return "Perishable"; }
    string toFileString() const override;
};

// ─────────────────────────────────────────────
class ClothingProduct : public Product {
private:
    string size;
    string material;

public:
    ClothingProduct();
    ClothingProduct(int id, int sellerId, string name, double price,
                    int stock, string size, string material);

    string getSize() const;
    string getMaterial() const;

    void displayDetails() const override;
    string getType() const override { return "Clothing"; }
    string toFileString() const override;
};
