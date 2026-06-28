#pragma once
#include <string>
using namespace std;

class User {
protected:
    int id;
    string name;
    string email;
    string password;
    string role; // "customer", "seller", "admin"

public:
    User();
    User(int id, string name, string email, string password, string role);
    virtual ~User() {}

    int getId() const;
    string getName() const;
    string getEmail() const;
    string getPassword() const;
    string getRole() const;

    void setName(string name);
    void setEmail(string email);
    void setPassword(string password);

    virtual void displayProfile() const;
    virtual string toFileString() const;
};

// ─────────────────────────────────────────────
class Customer : public User {
private:
    string address;

public:
    Customer();
    Customer(int id, string name, string email, string password, string address);

    string getAddress() const;
    void setAddress(string address);

    void displayProfile() const override;
    string toFileString() const override;
};

// ─────────────────────────────────────────────
class Seller : public User {
private:
    string shopName;
    bool isApproved;

public:
    Seller();
    Seller(int id, string name, string email, string password, string shopName);

    string getShopName() const;
    bool getIsApproved() const;
    void approve();
    void ban();

    void displayProfile() const override;
    string toFileString() const override;
};

// ─────────────────────────────────────────────
class Admin : public User {
public:
    Admin();
    Admin(int id, string name, string email, string password);

    void displayProfile() const override;
    string toFileString() const override;
};
