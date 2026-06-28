#include "../include/Product.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ── Product ───────────────────────────────────
Product::Product()
    : id(0), sellerId(0), name(""), category(""),
      price(0.0), stock(0), avgRating(0.0), reviewCount(0) {}

Product::Product(int id, int sellerId, string name, string category, double price, int stock)
    : id(id), sellerId(sellerId), name(name), category(category),
      price(price), stock(stock), avgRating(0.0), reviewCount(0) {}

int    Product::getId()          const { return id; }
int    Product::getSellerId()    const { return sellerId; }
string Product::getName()        const { return name; }
string Product::getCategory()    const { return category; }
double Product::getPrice()       const { return price; }
int    Product::getStock()       const { return stock; }
double Product::getAvgRating()   const { return avgRating; }
int    Product::getReviewCount() const { return reviewCount; }

void Product::setPrice(double p)  { price = p; }
void Product::setStock(int s)     { stock = s; }
void Product::setCategory(string cat) { category = cat; }

void Product::decrementStock(int qty) {
    if (stock >= qty) stock -= qty;
}

void Product::updateRating(double newRating) {
    avgRating = ((avgRating * reviewCount) + newRating) / (reviewCount + 1);
    reviewCount++;
}

void Product::displayDetails() const {
    cout << fixed << setprecision(2);
    cout << "[" << id << "] " << name << "\n"
         << "  Category : " << category   << "\n"
         << "  Price    : Rs." << price   << "\n"
         << "  Stock    : " << stock      << "\n"
         << "  Rating   : " << avgRating  << " (" << reviewCount << " reviews)\n";
}

string Product::toFileString() const {
    return to_string(id) + "|" + to_string(sellerId) + "|" + name + "|" +
           category + "|" + to_string(price) + "|" + to_string(stock) + "|" +
           to_string(avgRating) + "|" + to_string(reviewCount) + "|General";
}

// ── ElectronicProduct ─────────────────────────
ElectronicProduct::ElectronicProduct() : Product(), warrantyMonths(0), brand("") {}

ElectronicProduct::ElectronicProduct(int id, int sellerId, string name, double price,
                                     int stock, int warrantyMonths, string brand)
    : Product(id, sellerId, name, "Electronics", price, stock),
      warrantyMonths(warrantyMonths), brand(brand) {}

int    ElectronicProduct::getWarrantyMonths() const { return warrantyMonths; }
string ElectronicProduct::getBrand()          const { return brand; }

void ElectronicProduct::displayDetails() const {
    Product::displayDetails();
    cout << "  Brand    : " << brand          << "\n"
         << "  Warranty : " << warrantyMonths << " months\n";
}

string ElectronicProduct::toFileString() const {
    return to_string(id) + "|" + to_string(sellerId) + "|" + name + "|" +
           category + "|" + to_string(price) + "|" + to_string(stock) + "|" +
           to_string(avgRating) + "|" + to_string(reviewCount) + "|Electronic|" +
           brand + "|" + to_string(warrantyMonths);
}

// ── PerishableProduct ─────────────────────────
PerishableProduct::PerishableProduct() : Product(), expiryDate(""), storageCondition("") {}

PerishableProduct::PerishableProduct(int id, int sellerId, string name, double price,
                                     int stock, string expiryDate, string storageCondition)
    : Product(id, sellerId, name, "Perishable", price, stock),
      expiryDate(expiryDate), storageCondition(storageCondition) {}

string PerishableProduct::getExpiryDate()        const { return expiryDate; }
string PerishableProduct::getStorageCondition()  const { return storageCondition; }

void PerishableProduct::displayDetails() const {
    Product::displayDetails();
    cout << "  Expiry   : " << expiryDate        << "\n"
         << "  Storage  : " << storageCondition  << "\n";
}

string PerishableProduct::toFileString() const {
    return to_string(id) + "|" + to_string(sellerId) + "|" + name + "|" +
           category + "|" + to_string(price) + "|" + to_string(stock) + "|" +
           to_string(avgRating) + "|" + to_string(reviewCount) + "|Perishable|" +
           expiryDate + "|" + storageCondition;
}

// ── ClothingProduct ───────────────────────────
ClothingProduct::ClothingProduct() : Product(), size(""), material("") {}

ClothingProduct::ClothingProduct(int id, int sellerId, string name, double price,
                                 int stock, string size, string material)
    : Product(id, sellerId, name, "Clothing", price, stock),
      size(size), material(material) {}

string ClothingProduct::getSize()     const { return size; }
string ClothingProduct::getMaterial() const { return material; }

void ClothingProduct::displayDetails() const {
    Product::displayDetails();
    cout << "  Size     : " << size     << "\n"
         << "  Material : " << material << "\n";
}

string ClothingProduct::toFileString() const {
    return to_string(id) + "|" + to_string(sellerId) + "|" + name + "|" +
           category + "|" + to_string(price) + "|" + to_string(stock) + "|" +
           to_string(avgRating) + "|" + to_string(reviewCount) + "|Clothing|" +
           size + "|" + material;
}
