#include "../include/User.h"
#include <iostream>
using namespace std;

// ── User ──────────────────────────────────────
User::User() : id(0), name(""), email(""), password(""), role("") {}

User::User(int id, string name, string email, string password, string role)
    : id(id), name(name), email(email), password(password), role(role) {}

int    User::getId()       const { return id; }
string User::getName()     const { return name; }
string User::getEmail()    const { return email; }
string User::getPassword() const { return password; }
string User::getRole()     const { return role; }

void User::setName(string n)     { name = n; }
void User::setEmail(string e)    { email = e; }
void User::setPassword(string p) { password = p; }

void User::displayProfile() const {
    cout << "ID    : " << id   << "\n"
         << "Name  : " << name << "\n"
         << "Email : " << email << "\n"
         << "Role  : " << role  << "\n";
}

string User::toFileString() const {
    return to_string(id) + "|" + name + "|" + email + "|" + password + "|" + role;
}

// ── Customer ──────────────────────────────────
Customer::Customer() : User(), address("") {}

Customer::Customer(int id, string name, string email, string password, string address)
    : User(id, name, email, password, "customer"), address(address) {}

string Customer::getAddress() const { return address; }
void   Customer::setAddress(string a) { address = a; }

void Customer::displayProfile() const {
    User::displayProfile();
    cout << "Address : " << address << "\n";
}

string Customer::toFileString() const {
    return User::toFileString() + "|" + address;
}

// ── Seller ────────────────────────────────────
Seller::Seller() : User(), shopName(""), isApproved(false) {}

Seller::Seller(int id, string name, string email, string password, string shopName)
    : User(id, name, email, password, "seller"), shopName(shopName), isApproved(false) {}

string Seller::getShopName()    const { return shopName; }
bool   Seller::getIsApproved()  const { return isApproved; }
void   Seller::approve()              { isApproved = true; }
void   Seller::ban()                  { isApproved = false; }

void Seller::displayProfile() const {
    User::displayProfile();
    cout << "Shop    : " << shopName  << "\n"
         << "Status  : " << (isApproved ? "Approved" : "Pending/Banned") << "\n";
}

string Seller::toFileString() const {
    return User::toFileString() + "|" + shopName + "|" + (isApproved ? "1" : "0");
}

// ── Admin ─────────────────────────────────────
Admin::Admin() : User() {}

Admin::Admin(int id, string name, string email, string password)
    : User(id, name, email, password, "admin") {}

void Admin::displayProfile() const {
    User::displayProfile();
}

string Admin::toFileString() const {
    return User::toFileString();
}
