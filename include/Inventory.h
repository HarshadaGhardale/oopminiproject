
#ifndef INVENTORY_H
#define INVENTORY_H

#include "FoodItem.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdexcept>

class Inventory {
private:
    std::vector<FoodItem> items;
    int nextId;

public:
    Inventory();

    // Add and display records
    void addItem(const FoodItem& item);
    void displayAll() const;

    // Search
    FoodItem* findById(int id);
    const FoodItem* findById(int id) const;
    void searchByName(const std::string& name) const;

    // Modify inventory
    bool updateItem(int id, const FoodItem& updatedItem);
    bool deleteItem(int id);
    bool consumeItem(int id, double amount);
    bool discardItem(int id, double amount);

    // Expiry tracking
    void displayExpiringItems(int days = 3) const;
    void displayExpiredItems() const;
    void displayUseFirst() const;

    // Sorting and reports
    void sortByExpiry();
    double getTotalInventoryValue() const;
    double getTotalExpiredValue() const;
    void displaySummary() const;

    // Access to inventory records
    const std::vector<FoodItem>& getItems() const;
    int generateId();

    // Template: find an item using a supplied condition
    template <typename Predicate>
    const FoodItem* findItem(Predicate condition) const {
        for (const FoodItem& item : items) {
            if (condition(item)) {
                return &item;
            }
        }
        return nullptr;
    }

    // Remove items with zero remaining quantity
    void removeEmptyItems();
};

#endif
