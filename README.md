# SwiftCart 🛒

A fully functional e-commerce platform built in **C++** using **Object Oriented Programming** principles, featuring a complete console-based backend and a graphical user interface built with **SFML**. Developed as a second semester OOP course project.

---

## 👥 Team

| Name |
|---|---|
| Naba Gohar | 
| Taina Ejaz |

---

## 📌 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [OOP Concepts Used](#oop-concepts-used)
- [Tech Stack](#tech-stack)
- [Installation & Setup](#installation--setup)
  - [macOS (ARM/Intel)](#macos-armintel)
  - [Windows](#windows)
- [Running the Project](#running-the-project)
- [Default Credentials](#default-credentials)
- [How It Works](#how-it-works)
- [File Persistence](#file-persistence)

---

## Overview

SwiftCart is an Amazon-style e-commerce platform that simulates a complete online shopping experience. It supports three distinct user roles — **Customer**, **Seller**, and **Admin** — each with their own dashboard, permissions, and set of actions. 

The project is split into two layers:

- **Backend (Console Logic):** All business logic, data handling, file I/O, and OOP architecture lives here. Written entirely in standard C++17 with no external dependencies.
- **Frontend (SFML GUI):** A fully interactive graphical interface built on top of the backend using SFML 2.6. The GUI connects to the backend through a central `Platform` class without modifying any of the core logic.

All data is persisted between sessions using plain text files stored in a `data/` directory, making the system behave like a real application where nothing resets on close.

---

## Features

### 🧑‍💻 Customer
- Register and log in with saved credentials
- Browse all listed products with search and category filters
- Filter by Electronics, Food, Clothing, or any custom seller-added category
- View product details, ratings, and stock availability
- Add items to cart with quantity selection
- Apply discount codes at checkout
- Place orders and receive a generated receipt file
- Track order status (Pending → Confirmed → Shipped → Delivered)
- Leave reviews and star ratings on purchased products only

### 🏪 Seller
- Register a shop (requires admin approval before first login)
- Personal seller dashboard with stats and shop profile
- List products across three built-in categories: **Electronics**, **Food**, **Clothing**
- Create entirely **custom product categories** — these appear as filter buttons in the customer browse screen automatically
- Edit product prices and stock quantities
- View and manage incoming orders, update delivery status
- Generate and save sales reports to file

### 🔐 Admin
- Approve or ban seller accounts
- Create platform-wide discount codes (percentage or flat amount)
- View, flag, and delete customer reviews
- Monitor platform statistics: total customers, sellers, products, orders, and revenue

### 🖥️ GUI (SFML)
- Persistent sidebar navigation with icons
- Persistent top bar with shop name and breadcrumb trail
- Active tab/button highlighting that correctly shifts on selection
- Green confirm buttons, red danger buttons, blue primary actions
- Top-centre notification banners (green for success, red for error)
- Empty state messages in all list panels
- Scrollable product, order, and review lists
- Real-time search and filter

---

## Project Structure

```
SwiftCart/
│
├── include/                  # Backend header files
│   ├── User.h                # User base class + Customer, Seller, Admin
│   ├── Product.h             # Product base class + Electronic, Perishable, Clothing
│   ├── Transaction.h         # Cart, Order, Review, DiscountCode
│   ├── FileHandler.h         # Static file I/O utility class
│   └── Platform.h            # Central engine — session state + all operations
│
├── src/                      # Backend implementation files
│   ├── User.cpp
│   ├── Product.cpp
│   ├── Transaction.cpp
│   ├── FileHandler.cpp
│   ├── Platform.cpp
│   └── main_console.cpp      # Console-only entry point (reference, not compiled in GUI build)
│
├── gui/                      # SFML GUI layer
│   ├── main_gui.cpp          # GUI entry point
│   ├── Application.cpp       # Main loop, screen manager, window
│   ├── UIManager.cpp         # Shared widgets: buttons, inputs, sidebar, notifications
│   │
│   ├── include/              # GUI headers
│   │   ├── Screen.hpp        # Abstract base class for all screens
│   │   ├── UIManager.hpp     # Widget structs and UIManager class
│   │   ├── Application.hpp
│   │   └── screens/          # One header per screen
│   │       ├── LoginScreen.hpp
│   │       ├── RegisterScreen.hpp
│   │       ├── CustomerScreen.hpp
│   │       ├── ProductListScreen.hpp
│   │       ├── CartScreen.hpp
│   │       ├── OrderScreen.hpp
│   │       ├── SellerScreen.hpp
│   │       └── AdminScreen.hpp
│   │
│   ├── screens/              # Screen implementations
│   │   ├── LoginScreen.cpp
│   │   ├── RegisterScreen.cpp
│   │   ├── CustomerScreen.cpp
│   │   ├── ProductListScreen.cpp
│   │   ├── CartScreen.cpp
│   │   ├── OrderScreen.cpp
│   │   ├── SellerScreen.cpp
│   │   └── AdminScreen.cpp
│   │
│   └── assets/
│       └── fonts/
│           ├── Roboto-Regular.ttf
│           └── Roboto-Bold.ttf
│
├── data/                     # Auto-created at runtime
│   ├── customers.txt
│   ├── sellers.txt
│   ├── admins.txt
│   ├── products.txt
│   ├── orders.txt
│   ├── reviews.txt
│   ├── discounts.txt
│   ├── custom_categories.txt
│   └── cart_<id>.txt         # One file per customer cart
│
└── CMakeLists.txt            # Cross-platform build config
```

---

## OOP Concepts Used

This project was built to demonstrate every major Object Oriented Programming concept in a real, working system — not just on paper.

### 1. Classes & Objects
Every entity in the system is a class: `User`, `Product`, `Cart`, `Order`, `Review`, `DiscountCode`, `FileHandler`, `Platform`. On the GUI side: `Screen`, `UIManager`, `Application`, and one class per screen.

### 2. Inheritance
Two clean inheritance hierarchies:
- `User` → `Customer`, `Seller`, `Admin`
- `Product` → `ElectronicProduct`, `PerishableProduct`, `ClothingProduct`

On the GUI side, every screen inherits from the abstract `Screen` base class.

### 3. Polymorphism (Runtime)
Virtual functions `displayProfile()`, `displayDetails()`, `toFileString()`, and `getType()` are declared in base classes and overridden in every derived class. Calling these through a base class pointer correctly dispatches to the right version at runtime. `FileHandler::saveUser(User*)` uses this to save any user type to the correct file with one function call.

### 4. Encapsulation
All data members are `private` or `protected`. State is only accessible through getters and setters. No external code touches member variables directly.

### 5. Abstraction
The `Platform` class is the abstraction layer between the GUI and the backend. GUI screens only call high-level methods like `placeOrder()`, `login()`, `getAllProducts()`. All file loading, memory management, and business logic is hidden inside.

### 6. Constructors & Destructors
Every class has a default constructor and a parameterized constructor. Base classes have virtual destructors (`virtual ~User() {}`, `virtual ~Product() {}`) to prevent memory leaks when deleting through base pointers.

### 7. Constructor Chaining
Derived class constructors call parent constructors via initializer lists:
```cpp
Customer::Customer(int id, string name, string email, string password, string address)
    : User(id, name, email, password, "customer"), address(address) {}
```

### 8. Function Overriding with `override`
All virtual function overrides use the `override` keyword, letting the compiler catch typos and mismatches at compile time.

### 9. Access Specifiers
`private`, `protected`, and `public` are used deliberately throughout. Base class data is `protected` so child classes can access it. Everything else is `private`.

### 10. Composition
- `Cart` owns a `vector<CartItem>`
- `Order` owns a `vector<OrderItem>`
- `Platform` owns vectors of all entity types

### 11. Static Methods
All `FileHandler` methods are static — the class is a pure utility with no instance state.

### 12. Enumerations Inside Classes
`Order::Status` (`PENDING`, `CONFIRMED`, `SHIPPED`, `DELIVERED`, `CANCELLED`) and `DiscountCode::Type` (`PERCENTAGE`, `FLAT`) are scoped enums defined inside their respective classes.

### 13. Structs
`CartItem` and `OrderItem` are `struct` types — plain data containers with no behavior, clearly distinguishing them from full entity classes.

### 14. File I/O
`FileHandler` manages all persistent storage using `ifstream`/`ofstream`. `Cart` self-manages its own file (saves after every mutation). `Order` generates receipt `.txt` files on checkout.

---

## Tech Stack

| Component | Technology |
|---|---|
| Language | C++17 |
| GUI Framework | SFML 2.6 |
| Build System | CMake 3.16+ |
| Data Storage | Plain text files (.txt) |
| Fonts | Roboto (Google Fonts) |
| Platform | macOS (ARM/Intel), Windows |

---

## Installation & Setup

### Prerequisites
- CMake 3.16 or higher
- A C++17 compatible compiler (GCC, Clang, or MSVC)
- SFML 2.6
- Roboto font files (free from [fonts.google.com](https://fonts.google.com/specimen/Roboto))

Place these two font files in `gui/assets/fonts/` before building:
- `Roboto-Regular.ttf`
- `Roboto-Bold.ttf`

---

### macOS (ARM/Intel)

**Step 1 — Install dependencies**
```bash
brew install sfml cmake
```

**Step 2 — Clone the repository**
```bash
git clone https://github.com/yourusername/SwiftCart.git
cd SwiftCart
```

**Step 3 — Add fonts**

Download Roboto from [Google Fonts](https://fonts.google.com/specimen/Roboto) and place `Roboto-Regular.ttf` and `Roboto-Bold.ttf` inside `gui/assets/fonts/`.

**Step 4 — Build**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.logicalcpu)
```

**Step 5 — Run**
```bash
./SwiftCart
```

CMake automatically detects your Homebrew SFML installation and copies assets next to the binary.

---

### Windows

**Step 1 — Install CMake**

Download and install from [cmake.org](https://cmake.org/download/). Make sure to check *"Add CMake to system PATH"* during installation.

**Step 2 — Install MinGW (C++ compiler)**

Download MinGW-w64 from [winlibs.com](https://winlibs.com/) and extract it. Add the `bin/` folder to your system PATH.

**Step 3 — Download SFML 2.6**

Download the MinGW version of SFML 2.6 from [sfml-dev.org](https://www.sfml-dev.org/download.php) and extract it to `C:/SFML`.

**Step 4 — Clone the repository**
```bash
git clone https://github.com/yourusername/SwiftCart.git
cd SwiftCart
```

**Step 5 — Add fonts**

Place `Roboto-Regular.ttf` and `Roboto-Bold.ttf` inside `gui/assets/fonts/`.

**Step 6 — Build**
```bash
mkdir build
cd build
cmake .. -DSFML_DIR="C:/SFML/lib/cmake/SFML" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Step 7 — Run**
```bash
cd Release
SwiftCart.exe
```

> **Note:** SFML `.dll` files are copied automatically next to the executable by CMake.

---

## Running the Project

On first run, a default admin account is created automatically:

### Default Credentials

| Role | Email | Password |
|---|---|---|
| Admin | admin@swiftcart.com | admin123 |

Register as a **Customer** or **Seller** from the Create Account screen. Seller accounts require admin approval before they can log in.

---

## How It Works

### Session Flow
1. The application starts and loads all data from the `data/` directory into memory
2. The user logs in — the `Platform` class sets an `activeRole` and `activeUserId`
3. All subsequent actions go through `Platform` methods which validate the role and perform the operation
4. Data is written to file immediately after every change — no separate "save" step needed
5. On logout, session state is cleared and the app returns to the login screen

### Screen Navigation
The GUI uses a screen registry pattern. `Application` holds a map of screen name → screen object. When a screen sets its `nextScreen` string, `Application` switches to that screen on the next frame. The sidebar navigation calls `drawSidebar()` which returns the clicked screen key directly to the render function.

### Custom Categories
When a seller creates a new product category, the category name is saved to `data/custom_categories.txt`. The product is stored in `data/products.txt` with the custom category name as both its category field and type tag. `FileHandler::loadGeneral()` loads all products whose type tag is not one of the three built-in types. On the customer side, `ProductListScreen` reads `custom_categories.txt` on each visit and dynamically builds filter buttons for each custom category alongside the fixed ones.

---

## File Persistence

All data survives between sessions. The `data/` directory is created automatically on first run.

| File | Contents |
|---|---|
| `customers.txt` | Registered customer accounts |
| `sellers.txt` | Seller accounts and approval status |
| `admins.txt` | Admin accounts |
| `products.txt` | All product listings (all types in one file) |
| `orders.txt` | All placed orders with status history |
| `reviews.txt` | Customer reviews and ratings |
| `discounts.txt` | Discount codes and their active status |
| `custom_categories.txt` | Seller-created product categories |
| `cart_<id>.txt` | Persisted cart per customer (survives app close) |
| `receipt_<id>.txt` | Auto-generated receipt per completed order |
| `report_seller<id>.txt` | Sales report generated by seller |

---


## License

This project was built for academic purposes as part of the Object Oriented Programming course at the Institute of Space Technology, Islamabad.
