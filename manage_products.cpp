#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "nlohmann/json.hpp"
using namespace std;

using json = nlohmann::json;

// Column widths
const int COL_ID = 5;
const int COL_NAME = 40;
const int COL_PRICE = 10;
const int COL_STOCK = 10;

// variable pricing/name
struct Product
{
    int id;
    std::string name;
    double price;
    int stock;
};

// new items can be added to json file with this function
void saveProducts(const std::string &filename, const std::vector<Product> &products)
{
    json j;
    j["products"] = json::array();

    for (auto &p : products)
    {
        // takes input and places into json file
        j["products"].push_back({{"id", p.id},
                                 {"name", p.name},
                                 {"price", p.price},
                                 {"stock", p.stock}});
    }
    // saves the file
    std::ofstream file(filename);
    file << j.dump(4); // pretty print with 4 spaces
}

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
        else
        {
            cerr << "Error: products.json must contain an array called 'products'.\n";
        }
    }
    return products;
}

// display all loaded products
void displayProducts(const vector<Product> &products, const string &title = "Current products")
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

int main()
{
    std::string filename = "products.json";
    auto products = loadProducts(filename);

    int choice;
    do
    {
        displayProducts(products);

        std::cout << "\n=== Product Management ===\n";
        std::cout << "1. Add new product\n";
        std::cout << "2. Restock existing product\n";
        std::cout << "3. Remove a product\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        // adding a new product into json
        if (choice == 1)
        {
            int id, stock;
            double price;
            std::string name;

            displayProducts(products, "Add a new product");
            std::cout << "0. Return\n";

            std::cout << "\nEnter new product ID: ";
            std::cin >> id;

            // validation checking to verify ID is not being used
            bool exists = false;
            for (auto &p : products)
            {
                if (p.id == id)
                {
                    exists = true;
                    break;
                }
            }
            if (exists)
            {
                std::cout << "Error: Product ID " << id << " already exists. Please use another ID.\n";
            }

            // takes input and saves into json file
            else
            {
                std::cin.ignore();
                std::cout << "Enter product name: ";
                std::getline(std::cin, name);
                std::cout << "Enter product price: ";
                std::cin >> price;
                std::cout << "Enter initial stock: ";
                std::cin >> stock;

                products.push_back({id, name, price, stock});
                saveProducts("products.json", products);
                std::cout << "Product added successfully!\n";
            }
        }

        // restocking products
        else if (choice == 2)
        {
            displayProducts(products, "Restock a product");

            int id, addStock;
            std::cout << "\nEnter product ID to restock: ";
            std::cin >> id;
            std::cout << "Enter how many to add: ";
            std::cin >> addStock;

            bool found = false;
            for (auto &p : products)
            {
                // if ID matches, stock updates based on input
                if (p.id == id)
                {
                    // calculation of existing stock and new stock
                    p.stock += addStock;
                    found = true;
                    std::cout << "Stock updated! " << p.name
                              << " now has " << p.stock << " in stock.\n";
                    break;
                }
            }
            // validate if the ID existed
            if (!found)
            {
                std::cout << "Product with ID " << id << " not found.\n";
            }
        }
        // removing a product
        else if (choice == 3)
        {
            displayProducts(products, "Remove a product");

            int id;
            std::cout << "\nEnter product ID to remove: ";
            std::cin >> id;

            bool found = false;
            for (auto it = products.begin(); it != products.end(); ++it)
            {
                // if ID matches, it will remove product entirely
                if (it->id == id)
                {
                    std::cout << "Removing product: " << it->name << "\n";
                    products.erase(it);
                    found = true;
                    std::cout << "Product removed!\n";
                    break;
                }
            }
            // validate if ID existed
            if (!found)
            {
                std::cout << "Product with ID " << id << " not found.\n";
            }
        }
        // exit program manage_products
        else if (choice == 0)
        {
            std::cout << "Exiting program...\n";
        }
        else
        {
            std::cout << "Invalid choice! Try again.\n";
        }

        // Always save after each operation
        saveProducts(filename, products);

    } while (choice != 0);

    return 0;
}
