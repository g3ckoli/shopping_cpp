#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

//variable pricing/name
struct Product {
    std::string name;
    double price;
};

//items within structure above
struct CartItem {
    Product product;
    int quantity;
};

//loop through json file to display items
void displayProducts(const std::vector<Product>& products) {
    std::cout << "\n=== Available Items ===\n";
    //n- new line
    for (size_t i = 0; i < products.size(); i++) {
        //loop through line by line from json file
        std::cout << i + 1 << ". " << products[i].name
                  << " - RM " << std::fixed << std::setprecision(2)
                  << products[i].price << "\n";
    }
    std::cout << "0. Checkout\n";
}

//display cart
void displayCart(const std::vector<CartItem>& cart) {
    if (cart.empty()) {
        std::cout << "\nYour cart is empty!\n";
        return;
    }

    std::cout << "\n=== Your Shopping Cart ===\n";
    double total = 0.0;
    for (const auto& item : cart) {
        //calculations of each product(s)
        double itemTotal = item.product.price * item.quantity;
        //display items added to cart
        std::cout << item.product.name << " x" << item.quantity
                  << " @ RM " << item.product.price
                  << " = RM " << std::fixed << std::setprecision(2)
                  << itemTotal << "\n";
        //calculations of all products added to cart
                  total += itemTotal;
    }
    std::cout << "---------------------------\n";
    //display total calculation based on all items added to cart
    std::cout << "Total: RM " << std::fixed << std::setprecision(2) << total << "\n";
}

int main() {
    // Load products.json
    std::ifstream file("products.json");
    //exclaimation point = not. basically means if file is not open
    if (!file.is_open()) {
        std::cerr << "Error: Could not open products.json\n";
        return 1;
    }

    //if error with json file format, will show error reading file
    json j;
    try {
        file >> j;
    } catch (std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return 1;
    }

    //if error with naming array "products" in json, will show error reading file
    if (!j.contains("products") || !j["products"].is_array()) {
        std::cerr << "Error: products.json must contain an array called 'products'.\n";
        return 1;
    }

    //if error with naming items within array "name" & "price", will show error reading file
    //and checks for type of variable within json file
    std::vector<Product> products;
    for (const auto& p : j["products"]) {
        if (p.contains("name") && p.contains("price")) {
            products.push_back({p["name"].get<std::string>(), p["price"].get<double>()});
        }
    }

    //if no items in array json file, will show error reading file
    if (products.empty()) {
        std::cerr << "Error: No products loaded from products.json\n";
        return 1;
    }

    std::vector<CartItem> cart;
    int choice;

    //program ui
    std::cout << "=== Welcome to the Shopsy Shops ===\n";


    while (true) {
        //callback first function
        displayProducts(products);
        std::cout << "Choose item number: ";
        std::cin >> choice;

        if (choice == 0) {
            // Show cart before exit
            displayCart(cart);
            std::cout << "Thank you for shopping!\n";
            break;
        }

        //if choose within the listed items
        if (choice > 0 && choice <= (int)products.size()) {
            int qty;
            std::cout << "Enter quantity: ";
            //cout - output
            //cin - input
            std::cin >> qty;
            if (qty > 0) {
                cart.push_back({products[choice - 1], qty});
                std::cout << products[choice - 1].name << " added to cart!\n";
            } else {
                std::cout << "Quantity must be greater than 0.\n";
            }
            //if choose more than listed products
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
