
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "FoodItem.h"
#include "Inventory.h"

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

class FileManager {
private:
    std::string inventoryFile;
    std::string reportFile;

public:
    // Default file paths
    FileManager(
        const std::string& inventoryPath = "data/food_inventory.txt",
        const std::string& reportPath = "data/waste_report.txt"
    );

    // Save and load inventory
    bool saveInventory(const Inventory& inventory) const;
    bool loadInventory(Inventory& inventory) const;

    // Generate and save a waste report
    bool generateWasteReport(const Inventory& inventory) const;

    // Get configured file paths
    std::string getInventoryFile() const;
    std::string getReportFile() const;
};

#endif
