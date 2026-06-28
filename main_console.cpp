#include "../include/Platform.h"
#include <iostream>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

Platform platform;

// ── Input helpers ─────────────────────────────
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getIntInput(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) { clearInput(); return val; }
        clearInput();
        cout << "Invalid input. Try again.\n";
    }
}

double getDoubleInput(const string& prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) { clearInput(); return val; }
        clearInput();
        cout << "Invalid input. Try again.\n";
    }
}

string getStringInput(const string& prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    return val;
}

// ═══════════════════════════════════════════════
//  CUSTOMER MENU
// ═══════════════════════════════════════════════
void browseProducts() {
    cout << "\n--- Browse Products ---\n";
    cout << "1. View All\n2. Search by Name\n3. Filter by Category\n";
    int choice = getIntInput("Choice: ");

    vector<Product*> products;
    if (choice == 1) {
        products = platform.getAllProducts();
    } else if (choice == 2) {
        string kw = getStringInput("Enter keyword: ");
        products = platform.searchByName(kw);
    } else if (choice == 3) {
        cout << "Categories: Electronics | Perishable | Clothing\n";
        string cat = getStringInput("Enter category: ");
        products = platform.filterByCategory(cat);
    }

    if (products.empty()) { cout << "No products found.\n"; return; }
    cout << "\n";
    for (auto* p : products) p->displayDetails();
}

void manageCart() {
    int custId = platform.getActiveUserId();
    Cart& cart = platform.getCart(custId);

    cout << "\n--- Cart Menu ---\n";
    cout << "1. View Cart\n2. Add Item\n3. Remove Item\n"
         << "4. Update Quantity\n5. Apply Discount Code\n6. Checkout\n";
    int choice = getIntInput("Choice: ");

    if (choice == 1) {
        cart.display();
    } else if (choice == 2) {
        int pid = getIntInput("Product ID: ");
        Product* p = platform.getProductById(pid);
        if (!p) { cout << "Product not found.\n"; return; }
        int qty = getIntInput("Quantity: ");
        if (qty > p->getStock()) { cout << "Insufficient stock.\n"; return; }
        cart.addItem(pid, p->getName(), p->getPrice(), qty);
        cout << "Added to cart!\n";
    } else if (choice == 3) {
        int pid = getIntInput("Product ID to remove: ");
        cart.removeItem(pid);
        cout << "Item removed.\n";
    } else if (choice == 4) {
        int pid = getIntInput("Product ID: ");
        int qty = getIntInput("New quantity: ");
        cart.updateQuantity(pid, qty);
        cout << "Quantity updated.\n";
    } else if (choice == 5) {
        string code = getStringInput("Enter discount code: ");
        platform.applyDiscountCode(cart, code);
    } else if (choice == 6) {
        cart.display();
        if (cart.getItems().empty()) return;
        string confirm = getStringInput("Confirm order? (yes/no): ");
        if (confirm == "yes") platform.placeOrder(custId, cart);
    }
}

void viewMyOrders() {
    int custId = platform.getActiveUserId();
    auto orders = platform.getCustomerOrders(custId);
    if (orders.empty()) { cout << "No orders yet.\n"; return; }
    for (auto& o : orders) o.displayOrder();
}

void submitReview() {
    Customer* c = platform.getActiveCustomer();
    if (!c) return;
    int pid    = getIntInput("Product ID to review: ");
    int rating = getIntInput("Rating (1-5): ");
    if (rating < 1 || rating > 5) { cout << "Invalid rating.\n"; return; }
    string comment = getStringInput("Comment: ");
    platform.submitReview(c->getId(), c->getName(), pid, rating, comment);
}

void customerMenu() {
    Customer* c = platform.getActiveCustomer();
    cout << "\nWelcome, " << (c ? c->getName() : "Customer") << "!\n";
    while (true) {
        cout << "\n===== CUSTOMER MENU =====\n"
             << "1. Browse Products\n"
             << "2. My Cart\n"
             << "3. My Orders\n"
             << "4. Submit Review\n"
             << "5. My Profile\n"
             << "6. Logout\n";
        int choice = getIntInput("Choice: ");
        if      (choice == 1) browseProducts();
        else if (choice == 2) manageCart();
        else if (choice == 3) viewMyOrders();
        else if (choice == 4) submitReview();
        else if (choice == 5) { if (c) c->displayProfile(); }
        else if (choice == 6) { platform.logout(); break; }
        else cout << "Invalid choice.\n";
    }
}

// ═══════════════════════════════════════════════
//  SELLER MENU
// ═══════════════════════════════════════════════
void addProduct() {
    int sellerId = platform.getActiveUserId();
    cout << "\n--- Add Product ---\n";
    cout << "Type: 1. Electronic  2. Perishable  3. Clothing\n";
    int type = getIntInput("Choice: ");

    string name = getStringInput("Product name: ");
    double price = getDoubleInput("Price (Rs.): ");
    int stock    = getIntInput("Stock quantity: ");
    int id       = FileHandler::getNextId("data/products.txt");

    if (type == 1) {
        string brand = getStringInput("Brand: ");
        int warranty = getIntInput("Warranty (months): ");
        ElectronicProduct ep(id, sellerId, name, price, stock, warranty, brand);
        platform.addProduct(&ep);
    } else if (type == 2) {
        string expiry  = getStringInput("Expiry date (YYYY-MM-DD): ");
        string storage = getStringInput("Storage condition: ");
        PerishableProduct pp(id, sellerId, name, price, stock, expiry, storage);
        platform.addProduct(&pp);
    } else if (type == 3) {
        string size     = getStringInput("Size (S/M/L/XL): ");
        string material = getStringInput("Material: ");
        ClothingProduct cp(id, sellerId, name, price, stock, size, material);
        platform.addProduct(&cp);
    } else {
        cout << "Invalid type.\n";
    }
}

void manageSellerOrders() {
    int sellerId = platform.getActiveUserId();
    auto orders = platform.getSellerOrders(sellerId);
    if (orders.empty()) { cout << "No orders yet.\n"; return; }
    for (auto& o : orders) o.displayOrder();

    cout << "\n1. Update order status\n2. Back\n";
    int choice = getIntInput("Choice: ");
    if (choice == 1) {
        int ordId = getIntInput("Order ID: ");
        cout << "Status: 1.Confirmed  2.Shipped  3.Delivered  4.Cancelled\n";
        int s = getIntInput("Choice: ");
        Order::Status status;
        if      (s == 1) status = Order::CONFIRMED;
        else if (s == 2) status = Order::SHIPPED;
        else if (s == 3) status = Order::DELIVERED;
        else if (s == 4) status = Order::CANCELLED;
        else { cout << "Invalid.\n"; return; }
        platform.updateOrderStatus(ordId, status);
    }
}

void sellerMenu() {
    Seller* s = platform.getActiveSeller();
    cout << "\nWelcome, " << (s ? s->getShopName() : "Seller") << "!\n";
    while (true) {
        cout << "\n===== SELLER MENU =====\n"
             << "1. Add Product\n"
             << "2. Edit Product\n"
             << "3. Remove Product\n"
             << "4. View My Orders\n"
             << "5. Generate Sales Report\n"
             << "6. My Profile\n"
             << "7. Logout\n";
        int choice = getIntInput("Choice: ");
        if      (choice == 1) addProduct();
        else if (choice == 2) {
            int pid = getIntInput("Product ID to edit: ");
            double price = getDoubleInput("New price (Rs.): ");
            int stock    = getIntInput("New stock: ");
            platform.editProduct(pid, price, stock);
        }
        else if (choice == 3) {
            int pid = getIntInput("Product ID to remove: ");
            platform.removeProduct(pid);
        }
        else if (choice == 4) manageSellerOrders();
        else if (choice == 5) platform.generateSalesReport(platform.getActiveUserId());
        else if (choice == 6) { if (s) s->displayProfile(); }
        else if (choice == 7) { platform.logout(); break; }
        else cout << "Invalid choice.\n";
    }
}

// ═══════════════════════════════════════════════
//  ADMIN MENU
// ═══════════════════════════════════════════════
void adminMenu() {
    Admin* a = platform.getActiveAdmin();
    cout << "\nWelcome, Admin " << (a ? a->getName() : "") << "!\n";
    while (true) {
        cout << "\n===== ADMIN MENU =====\n"
             << "1. View Pending Sellers\n"
             << "2. Approve Seller\n"
             << "3. Ban Seller\n"
             << "4. Add Discount Code\n"
             << "5. Deactivate Discount Code\n"
             << "6. View All Reviews\n"
             << "7. Flag Review\n"
             << "8. Delete Review\n"
             << "9. Platform Stats\n"
             << "10. Logout\n";
        int choice = getIntInput("Choice: ");

        if (choice == 1) {
            auto pending = platform.getPendingSellers();
            if (pending.empty()) { cout << "No pending sellers.\n"; continue; }
            for (auto& s : pending) s.displayProfile();
        }
        else if (choice == 2) {
            int id = getIntInput("Seller ID to approve: ");
            platform.approveSeller(id);
        }
        else if (choice == 3) {
            int id = getIntInput("Seller ID to ban: ");
            platform.banSeller(id);
        }
        else if (choice == 4) {
            string code = getStringInput("Discount code: ");
            cout << "Type: 1. Percentage  2. Flat amount\n";
            int t       = getIntInput("Choice: ");
            double val  = getDoubleInput("Value: ");
            double minO = getDoubleInput("Minimum order amount (Rs.): ");
            DiscountCode::Type type = (t == 1) ? DiscountCode::PERCENTAGE : DiscountCode::FLAT;
            platform.addDiscountCode(code, type, val, minO);
        }
        else if (choice == 5) {
            string code = getStringInput("Code to deactivate: ");
            platform.removeDiscountCode(code);
            cout << "Code deactivated.\n";
        }
        else if (choice == 6) {
            auto reviews = platform.getAllReviews();
            if (reviews.empty()) { cout << "No reviews yet.\n"; continue; }
            for (auto& r : reviews) r.display();
        }
        else if (choice == 7) {
            int id = getIntInput("Review ID to flag: ");
            platform.flagReview(id);
            cout << "Review flagged.\n";
        }
        else if (choice == 8) {
            int id = getIntInput("Review ID to delete: ");
            platform.deleteReview(id);
            cout << "Review deleted.\n";
        }
        else if (choice == 9) platform.displayPlatformStats();
        else if (choice == 10) { platform.logout(); break; }
        else cout << "Invalid choice.\n";
    }
}

// ═══════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════
void showMainMenu() {
    while (true) {
        cout << "\n╔══════════════════════════╗\n"
             << "║       SWIFTCART PLATFORM     ║\n"
             << "╠══════════════════════════╣\n"
             << "║ 1. Login                 ║\n"
             << "║ 2. Register as Customer  ║\n"
             << "║ 3. Register as Seller    ║\n"
             << "║ 4. Exit                  ║\n"
             << "╚══════════════════════════╝\n";
        int choice = getIntInput("Choice: ");

        if (choice == 1) {
            string email    = getStringInput("Email: ");
            string password = getStringInput("Password: ");
            if (platform.login(email, password)) {
                string role = platform.getActiveRole();
                if      (role == "customer") customerMenu();
                else if (role == "seller")   sellerMenu();
                else if (role == "admin")    adminMenu();
            } else {
                cout << "Invalid credentials.\n";
            }
        }
        else if (choice == 2) {
            string name     = getStringInput("Name: ");
            string email    = getStringInput("Email: ");
            string password = getStringInput("Password: ");
            string address  = getStringInput("Address: ");
            if (platform.registerCustomer(name, email, password, address))
                cout << "Registration successful! You can now login.\n";
            else
                cout << "Email already in use.\n";
        }
        else if (choice == 3) {
            string name     = getStringInput("Name: ");
            string email    = getStringInput("Email: ");
            string password = getStringInput("Password: ");
            string shopName = getStringInput("Shop name: ");
            if (platform.registerSeller(name, email, password, shopName))
                cout << "Seller registered! Await admin approval before logging in.\n";
            else
                cout << "Email already in use.\n";
        }
        else if (choice == 4) {
            cout << "Goodbye!\n";
            break;
        }
        else cout << "Invalid choice.\n";
    }
}

int main() {
    // Ensure data directory exists
    system("mkdir -p data");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    showMainMenu();
    return 0;
}
