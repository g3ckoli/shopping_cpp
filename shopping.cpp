#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include "nlohmann/json.hpp"
using namespace std;

using json = nlohmann::json;

// set the width of each column
const int COL_ID = 5;
const int COL_NAME = 40;
const int COL_PRICE = 10;
const int COL_STOCK = 10;

// variable for product(s)
struct Product
{
    int id;
    string name;
    double price;
    int stock;
};

// variable for cart
struct CartItem
{
    int id;
    string name;
    double price;
    int quantity;
};

// load products from json file
vector<Product> loadProducts(const string &filename)
{
    ifstream file(filename);
    vector<Product> products;

    if (file)
    {
        json j;
        file >> j;
        if (j.contains("products") && j["products"].is_array())
        {
            for (auto &item : j["products"])
            {
                products.push_back({item["id"],
                                    item["name"],
                                    item["price"],
                                    item["stock"]});
            }
        }
    }
    return products;
}

// function to deduct the stock amount once a purchase have been completed. then it will save the stock balance amount to json
void saveProducts(const string &filename, const vector<Product> &products)
{
    json j;
    j["products"] = json::array();

    for (auto &p : products)
    {
        j["products"].push_back({{"id", p.id},
                                 {"name", p.name},
                                 {"price", p.price},
                                 {"stock", p.stock}});
    }

    ofstream file(filename);
    file << j.dump(4);
}

// function to display all the products
void displayProducts(const vector<Product> &products, const string &title = "Available products")
{
    cout << "\n"
         << title << ":\n";
    cout << left << setw(COL_ID) << "ID"
         << setw(COL_NAME) << "Name"
         << setw(COL_PRICE) << "Price"
         << setw(COL_STOCK) << "Stock" << "\n";

    for (auto &p : products)
    {
        cout << left << setw(COL_ID) << p.id
             << setw(COL_NAME) << p.name
             << setw(COL_PRICE) << fixed << setprecision(2) << p.price
             << setw(COL_STOCK) << p.stock << "\n";
    }
}

// function to display all the cart item added (if any)
void displayCart(const vector<CartItem> &cart)
{
    cout << "\n=== Your Cart ===\n";
    // if empty then it mentions its empty
    if (cart.empty())
    {
        cout << "Cart is empty.\n";
        return;
    }

    double total = 0;
    cout << left << setw(COL_NAME) << "Item"
         << setw(COL_PRICE) << "Price"
         << setw(COL_STOCK) << "Qty"
         << "Subtotal\n";

    for (auto &item : cart)
    {
        double itemTotal = item.price * item.quantity;
        cout << left << setw(COL_NAME) << item.name
             << setw(COL_PRICE) << fixed << setprecision(2) << item.price
             << setw(COL_STOCK) << item.quantity
             << itemTotal << "\n";
        // calculation for the total amount
        total += itemTotal;
    }
    cout << "-------------------------\n";
    cout << "Total: RM " << total << "\n";
}

int main()
{
    string filename = "products.json";
    auto products = loadProducts(filename);
    vector<CartItem> cart;

    int choice;
    do
    {
        cout << "\n==== Shopping Menu ====\n";
        cout << "1. Add product to cart\n";
        cout << "2. View cart / Checkout\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        // add product to cart
        if (choice == 1)
        {
            displayProducts(products);

            int id, quantity;
            cout << "\nEnter product ID to add to cart: ";
            cin >> id;
            cout << "Enter quantity: ";
            cin >> quantity;

            auto it = find_if(products.begin(), products.end(),
                              [&](const Product &p)
                              { return p.id == id; });

            // if product ordered is more than stock, will show a message
            if (it != products.end())
            {
                if (quantity > it->stock)
                {
                    cout << "Sorry, only " << it->stock << " units of "
                         << it->name << " are available.\n";
                }
                else
                {
                    cart.push_back({it->id, it->name, it->price, quantity});
                    cout << quantity << " x " << it->name << " added to cart.\n";
                }
            }
            else
            {
                cout << "Product with ID " << id << " not found.\n";
            }
        }

        // view cart and/or checkout
        else if (choice == 2)
        {
            int cartChoice;
            do
            {
                displayCart(cart);

                cout << "\n1. Add more items\n";
                cout << "2. Checkout\n";
                cout << "0. Back to main menu\n";
                cout << "Choice: ";
                cin >> cartChoice;

                if (cartChoice == 1)
                {
                    choice = 1; // go back to add items
                    break;
                }

                // checkout the cart item (if any)
                else if (cartChoice == 2)
                {
                    if (cart.empty())
                    {
                        cout << "Cart is empty. Nothing to checkout.\n";
                    }
                    else
                    {
                        // validate if enough stock against the ordered products
                        bool stockOk = true;
                        for (auto &item : cart)
                        {
                            auto it = find_if(products.begin(), products.end(),
                                              [&](const Product &p)
                                              { return p.id == item.id; });
                            if (it == products.end() || item.quantity > it->stock)
                            {
                                cout << "Not enough stock for " << item.name
                                     << ". Available: " << (it != products.end() ? it->stock : 0) << "\n";
                                stockOk = false;
                            }
                        }
                        // if have enough stock then will proceed to checkout
                        if (stockOk)
                        {
                            char confirm;
                            cout << "Proceed to checkout? (y/n): ";
                            cin >> confirm;
                            if (confirm == 'y' || confirm == 'Y')
                            {
                                for (auto &item : cart)
                                {
                                    for (auto &p : products)
                                    {
                                        if (p.id == item.id)
                                        {
                                            p.stock -= item.quantity;
                                        }
                                    }
                                }
                                // deduct the stock amount on product, then proceed to checkout and complete the purchase
                                saveProducts(filename, products);
                                cart.clear();
                                cout << "Checkout successful! Thank you for your purchase.\n";
                            }
                            else
                            {
                                cout << "Checkout cancelled.\n";
                            }
                        }
                    }
                }

            } while (cartChoice != 0 && cartChoice != 1);
        }

    } while (choice != 0);

    cout << "Exiting program...\n";
    return 0;
}
