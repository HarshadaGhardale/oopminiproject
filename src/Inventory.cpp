
#include "Inventory.h"

#include <algorithm>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool getDateParts(const std::string& date, std::tm& result) {
    if (date.size() != 10 ||
        date[4] != '-' || date[7] != '-') {
        return false;
    }

    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (year < 1900 || month < 1 || month > 12 ||
            day < 1 || day > 31) {
            return false;
        }

        result = {};
        result.tm_year = year - 1900;
        result.tm_mon = month - 1;
        result.tm_mday = day;
        result.tm_hour = 12;
        result.tm_isdst = -1;

        std::tm copy = result;

        if (std::mktime(&copy) == -1) {
            return false;
        }

        return copy.tm_year == result.tm_year &&
               copy.tm_mon == result.tm_mon &&
               copy.tm_mday == result.tm_mday;
    } catch (...) {
        return false;
    }
}

long long dateNumber(std::tm date) {
    date.tm_hour = 12;
    date.tm_min = 0;
    date.tm_sec = 0;
    date.tm_isdst = -1;

    std::time_t value = std::mktime(&date);

    if (value == -1) {
        throw std::runtime_error("Unable to calculate date.");
    }

    return static_cast<long long>(value / 86400);
}

long long daysUntil(const std::string& date) {
    std::tm expiry{};

    if (!getDateParts(date, expiry)) {
        throw std::invalid_argument("Invalid expiry date.");
    }

    std::time_t now = std::time(nullptr);
    std::tm today{};

#ifdef _MSC_VER
    localtime_s(&today, &now);
#else
    std::tm* localTime = std::localtime(&now);

    if (localTime != nullptr) {
        today = *localTime;
    }
#endif

    today.tm_hour = 12;
    today.tm_min = 0;
    today.tm_sec = 0;
    today.tm_isdst = -1;

    return dateNumber(expiry) - dateNumber(today);
}

} // namespace

// --------------------------------------------------
// Constructor
// --------------------------------------------------

Inventory::Inventory() : nextId(1) {
}

// --------------------------------------------------
// Add food item
// --------------------------------------------------

void Inventory::addItem(const FoodItem& item) {
    for (const FoodItem& existing : items) {
        if (existing.getId() == item.getId()) {
            throw std::invalid_argument(
                "A food item with this ID already exists."
            );
        }
    }

    items.push_back(item);

    if (item.getId() >= nextId) {
        nextId = item.getId() + 1;
    }
}

// --------------------------------------------------
// Display all items
// --------------------------------------------------

void Inventory::displayAll() const {
    if (items.empty()) {
        std::cout << "\nInventory is empty.\n";
        return;
    }

    std::cout << '\n'
              << std::left
              << std::setw(5)  << "ID"
              << std::setw(20) << "Name"
              << std::setw(14) << "Category"
              << std::setw(10) << "Quantity"
              << std::setw(9)  << "Unit"
              << std::setw(12) << "Unit Price"
              << std::setw(13) << "Total Cost"
              << std::setw(13) << "Expiry Date"
              << "Status\n";

    std::cout << std::string(110, '-') << '\n';

    for (const FoodItem& item : items) {
        item.display();
    }
}

// --------------------------------------------------
// Find food item by ID
// --------------------------------------------------

FoodItem* Inventory::findById(int id) {
    for (FoodItem& item : items) {
        if (item.getId() == id) {
            return &item;
        }
    }

    return nullptr;
}

const FoodItem* Inventory::findById(int id) const {
    for (const FoodItem& item : items) {
        if (item.getId() == id) {
            return &item;
        }
    }

    return nullptr;
}

// --------------------------------------------------
// Search by name
// --------------------------------------------------

void Inventory::searchByName(const std::string& name) const {
    bool found = false;

    for (const FoodItem& item : items) {
        if (item.getName().find(name) != std::string::npos) {
            if (!found) {
                std::cout << "\nMatching food items:\n";
            }

            item.display();
            found = true;
        }
    }

    if (!found) {
        std::cout << "\nNo matching food items found.\n";
    }
}

// --------------------------------------------------
// Update an item
// --------------------------------------------------

bool Inventory::updateItem(
    int id,
    const FoodItem& updatedItem
) {
    FoodItem* item = findById(id);

    if (item == nullptr) {
        return false;
    }

    *item = updatedItem;

    return true;
}

// --------------------------------------------------
// Delete an item
// --------------------------------------------------

bool Inventory::deleteItem(int id) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getId() == id) {
            items.erase(it);
            return true;
        }
    }

    return false;
}

// --------------------------------------------------
// Consume food
// --------------------------------------------------

bool Inventory::consumeItem(int id, double amount) {
    FoodItem* item = findById(id);

    if (item == nullptr) {
        return false;
    }

    item->consume(amount);
    return true;
}

// --------------------------------------------------
// Discard food
// --------------------------------------------------

bool Inventory::discardItem(int id, double amount) {
    FoodItem* item = findById(id);

    if (item == nullptr) {
        return false;
    }

    item->discard(amount);
    return true;
}

// --------------------------------------------------
// Display items expiring within a chosen number of days
// --------------------------------------------------

void Inventory::displayExpiringItems(int days) const {
    if (days < 0) {
        throw std::invalid_argument(
            "Number of days cannot be negative."
        );
    }

    bool found = false;

    std::cout << "\nItems expiring within "
              << days << " day(s):\n";

    for (const FoodItem& item : items) {
        if (item.getQuantity() <= 0) {
            continue;
        }

        long long remaining = daysUntil(item.getExpiryDate());

        if (remaining >= 0 && remaining <= days) {
            item.display();
            found = true;
        }
    }

    if (!found) {
        std::cout << "No items found in this period.\n";
    }
}

// --------------------------------------------------
// Display expired items
// --------------------------------------------------

void Inventory::displayExpiredItems() const {
    bool found = false;

    std::cout << "\nExpired food items:\n";

    for (const FoodItem& item : items) {
        if (item.getQuantity() > 0 &&
            daysUntil(item.getExpiryDate()) < 0) {
            item.display();
            found = true;
        }
    }

    if (!found) {
        std::cout << "No expired items found.\n";
    }
}

// --------------------------------------------------
// Recommend items to use first
// --------------------------------------------------

void Inventory::displayUseFirst() const {
    std::vector<const FoodItem*> ordered;

    for (const FoodItem& item : items) {
        if (item.getQuantity() > 0) {
            ordered.push_back(&item);
        }
    }

    std::sort(
        ordered.begin(),
        ordered.end(),
        [](const FoodItem* a, const FoodItem* b) {
            return *a < *b;
        }
    );

    std::cout << "\n--- USE FOOD IN THIS ORDER ---\n";

    if (ordered.empty()) {
        std::cout << "No food items in stock.\n";
        return;
    }

    for (const FoodItem* item : ordered) {
        item->display();
    }
}

// --------------------------------------------------
// Sort inventory by expiry date
// --------------------------------------------------

void Inventory::sortByExpiry() {
    std::sort(items.begin(), items.end());
}

// --------------------------------------------------
// Calculate total inventory value
// --------------------------------------------------

double Inventory::getTotalInventoryValue() const {
    double total = 0.0;

    for (const FoodItem& item : items) {
        total += item.getTotalCost();
    }

    return total;
}

// --------------------------------------------------
// Calculate value of expired stock
// --------------------------------------------------

double Inventory::getTotalExpiredValue() const {
    double total = 0.0;

    for (const FoodItem& item : items) {
        if (item.getQuantity() > 0 &&
            daysUntil(item.getExpiryDate()) < 0) {
            total += item.getTotalCost();
        }
    }

    return total;
}

// --------------------------------------------------
// Display inventory summary
// --------------------------------------------------

void Inventory::displaySummary() const {
    int totalItems = static_cast<int>(items.size());
    int expiredItems = 0;
    int expiringSoon = 0;
    double totalQuantity = 0.0;

    for (const FoodItem& item : items) {
        totalQuantity += item.getQuantity();

        if (item.getQuantity() <= 0) {
            continue;
        }

        long long remaining = daysUntil(item.getExpiryDate());

        if (remaining < 0) {
            ++expiredItems;
        } else if (remaining <= 3) {
            ++expiringSoon;
        }
    }

    std::cout << "\n========== INVENTORY SUMMARY ==========\n";
    std::cout << "Total food records      : " << totalItems << '\n';
    std::cout << "Total quantity          : " << totalQuantity << '\n';
    std::cout << "Items expiring in 3 days: " << expiringSoon << '\n';
    std::cout << "Expired items in stock  : " << expiredItems << '\n';

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Inventory value         : Rs. "
              << getTotalInventoryValue() << '\n';

    std::cout << "Expired stock value     : Rs. "
              << getTotalExpiredValue() << '\n';

    std::cout << "========================================\n";
}

// --------------------------------------------------
// Get read-only access to items
// --------------------------------------------------

const std::vector<FoodItem>& Inventory::getItems() const {
    return items;
}

// --------------------------------------------------
// Generate the next available ID
// --------------------------------------------------

int Inventory::generateId() {
    return nextId++;
}

// --------------------------------------------------
// Remove items with zero quantity
// --------------------------------------------------

void Inventory::removeEmptyItems() {
    items.erase(
        std::remove_if(
            items.begin(),
            items.end(),
            [](const FoodItem& item) {
                return item.getQuantity() <= 0;
            }
        ),
        items.end()
    );
}
