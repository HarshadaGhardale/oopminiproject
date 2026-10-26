
#include "FoodItem.h"
#include "Inventory.h"
#include "FileManager.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>

// --------------------------------------------------
// Simple message helpers (no Theme dependency)
// --------------------------------------------------

void printHeader(const std::string& title) {
    std::cout << "\n"
              << "=======================================================\n"
              << "       " << title << '\n'
              << "=======================================================\n";
}

void printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << '\n';
}

void printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << '\n';
}

void printError(const std::string& message) {
    std::cout << "[ERROR] " << message << '\n';
}

void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << '\n';
}

void printLine(char symbol = '-', int length = 55) {
    if (length < 1) {
        length = 1;
    }

    std::cout << std::string(
        static_cast<std::size_t>(length), symbol
    ) << '\n';
}

// --------------------------------------------------
// Safe input helpers
// --------------------------------------------------

int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;

        std::string input;
        if (!std::getline(std::cin, input)) {
            throw std::runtime_error("Input stream closed.");
        }

        std::stringstream stream(input);
        int value;
        char extra;

        if (stream >> value && !(stream >> extra)) {
            return value;
        }

        printError("Enter a valid whole number.");
    }
}

double readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;

        std::string input;
        if (!std::getline(std::cin, input)) {
            throw std::runtime_error("Input stream closed.");
        }

        std::stringstream stream(input);
        double value;
        char extra;

        if (stream >> value &&
            !(stream >> extra) &&
            std::isfinite(value)) {
            return value;
        }

        printError("Enter a valid number.");
    }
}

std::string readText(const std::string& prompt) {
    while (true) {
        std::cout << prompt;

        std::string value;
        if (!std::getline(std::cin, value)) {
            throw std::runtime_error("Input stream closed.");
        }

        if (!value.empty() &&
            value.find('|') == std::string::npos) {
            return value;
        }

        printError(
            "Input cannot be empty or contain the | character."
        );
    }
}

// --------------------------------------------------
// Input a new food record
// --------------------------------------------------

FoodItem inputFoodItem(int id) {
    std::cout << "\n--- ENTER FOOD DETAILS ---\n";

    std::string name = readText("Food name: ");

    std::string category = readText(
        "Category (Dairy/Fruits/Vegetables/Other): "
    );

    double quantity;

    do {
        quantity = readDouble("Quantity: ");

        if (quantity < 0) {
            printError("Quantity cannot be negative.");
        }
    } while (quantity < 0);

    std::string unit = readText(
        "Unit (kg, litres, packets, pcs): "
    );

    double price;

    do {
        price = readDouble("Price per unit (Rs.): ");

        if (price < 0) {
            printError("Price cannot be negative.");
        }
    } while (price < 0);

    std::string purchaseDate = readText(
        "Purchase date (YYYY-MM-DD): "
    );

    std::string expiryDate = readText(
        "Expiry date (YYYY-MM-DD): "
    );

    return FoodItem(
        id,
        name,
        category,
        quantity,
        unit,
        price,
        purchaseDate,
        expiryDate
    );
}

// --------------------------------------------------
// Main menu (Option 14 removed)
// --------------------------------------------------

void showMainMenu() {
    printHeader("FOOD WASTE & EXPIRY TRACKER");

    std::cout << "\n  1. Add Food Item\n"
              << "  2. View All Food Items\n"
              << "  3. Search Food Item\n"
              << "  4. Update Food Item\n"
              << "  5. Delete Food Item\n"
              << "  6. Check Expiring Items\n"
              << "  7. View Expired Items\n"
              << "  8. Recommend Food to Use First\n"
              << "  9. Consume Food\n"
              << " 10. Discard Food\n"
              << " 11. Inventory Summary\n"
              << " 12. Generate Waste Report\n"
              << " 13. Save Inventory\n"
              << "  0. Save and Exit\n";

    printLine('=');
}

// --------------------------------------------------
// Program entry point
// --------------------------------------------------

int main() {
    Inventory inventory;
    FileManager fileManager;

    try {
        printHeader("WELCOME TO FOOD TRACKER");

        printInfo(
            "A simple system to manage food inventory "
            "and reduce waste."
        );

        fileManager.loadInventory(inventory);

        // Display the menu only once at startup.
        showMainMenu();

        printInfo(
            "The menu is shown only once. Enter an option "
            "number each time; enter 0 to save and exit."
        );

        bool running = true;

        while (running) {
            int choice = readInt(
                "\nEnter your choice (0 to save and exit): "
            );

            try {
                switch (choice) {

                    // 1. Add food item
                    case 1: {
                        int id = inventory.generateId();

                        FoodItem item = inputFoodItem(id);
                        inventory.addItem(item);

                        printSuccess(
                            "Food item added successfully. ID: "
                            + std::to_string(id)
                        );
                        break;
                    }

                    // 2. View all food items
                    case 2:
                        inventory.displayAll();
                        break;

                    // 3. Search food item
                    case 3: {
                        std::string name = readText(
                            "Enter food name to search: "
                        );

                        inventory.searchByName(name);
                        break;
                    }

                    // 4. Update food item
                    case 4: {
                        int id = readInt(
                            "Enter food item ID to update: "
                        );

                        if (inventory.findById(id) == nullptr) {
                            printError("Food item not found.");
                            break;
                        }

                        FoodItem updated = inputFoodItem(id);

                        if (inventory.updateItem(id, updated)) {
                            printSuccess(
                                "Food item updated successfully."
                            );
                        } else {
                            printError("Update failed.");
                        }
                        break;
                    }

                    // 5. Delete food item
                    case 5: {
                        int id = readInt(
                            "Enter food item ID to delete: "
                        );

                        if (inventory.deleteItem(id)) {
                            printSuccess("Food item deleted.");
                        } else {
                            printError("Food item not found.");
                        }
                        break;
                    }

                    // 6. Check expiring items
                    case 6: {
                        int days = readInt(
                            "Show items expiring within how many days? "
                        );

                        inventory.displayExpiringItems(days);
                        break;
                    }

                    // 7. View expired items
                    case 7:
                        inventory.displayExpiredItems();
                        break;

                    // 8. Recommend food to use first
                    case 8:
                        inventory.displayUseFirst();
                        break;

                    // 9. Consume food
                    case 9: {
                        int id = readInt(
                            "Enter food item ID: "
                        );

                        double amount = readDouble(
                            "Quantity consumed: "
                        );

                        if (inventory.consumeItem(id, amount)) {
                            printSuccess("Consumption recorded.");
                        } else {
                            printError("Food item not found.");
                        }
                        break;
                    }

                    // 10. Discard food
                    case 10: {
                        int id = readInt(
                            "Enter food item ID: "
                        );

                        double amount = readDouble(
                            "Quantity discarded: "
                        );

                        if (inventory.discardItem(id, amount)) {
                            printSuccess(
                                "Discarded quantity recorded."
                            );
                        } else {
                            printError("Food item not found.");
                        }
                        break;
                    }

                    // 11. Inventory summary
                    case 11:
                        inventory.displaySummary();
                        break;

                    // 12. Generate waste report
                    case 12:
                        fileManager.generateWasteReport(inventory);
                        break;

                    // 13. Save inventory
                    case 13:
                        if (fileManager.saveInventory(inventory)) {
                            printSuccess("Inventory saved successfully.");
                        } else {
                            printError("Failed to save inventory.");
                        }
                        break;

                    // 0. Save and exit
                    case 0:
                        if (fileManager.saveInventory(inventory)) {
                            printSuccess(
                                "Thank you for using Food Waste Tracker!"
                            );

                            running = false;
                        } else {
                            printError(
                                "Inventory was not saved. "
                                "You can retry or exit without saving."
                            );

                            int retry = readInt(
                                "Enter 1 to retry saving, 0 to exit "
                                "without saving: "
                            );

                            if (retry == 0) {
                                running = false;
                            }
                        }
                        break;

                    // Invalid option
                    default:
                        printWarning(
                            "Invalid choice. Select a number "
                            "from the menu."
                        );
                }

            } catch (const std::exception& error) {
                printError(error.what());
            }
        }

    } catch (const std::exception& error) {
        printError(
            std::string("Application error: ") + error.what()
        );

        return 1;
    }

    return 0;
}
