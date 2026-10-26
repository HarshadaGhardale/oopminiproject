
#include "FileManager.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <exception>

// --------------------------------------------------
// Constructor
// --------------------------------------------------

FileManager::FileManager(
    const std::string& inventoryPath,
    const std::string& reportPath
)
    : inventoryFile(inventoryPath),
      reportFile(reportPath) {
}

// --------------------------------------------------
// Save inventory
// --------------------------------------------------

bool FileManager::saveInventory(const Inventory& inventory) const {
    std::ofstream file(inventoryFile);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open inventory file for writing."
                  << std::endl;
        return false;
    }

    for (const FoodItem& item : inventory.getItems()) {
        file << item.toFileString() << '\n';

        if (!file) {
            std::cerr << "Error: Error while writing inventory."
                      << std::endl;
            return false;
        }
    }

    file.close();

    if (!file) {
        std::cerr << "Error: Error closing inventory file."
                  << std::endl;
        return false;
    }

    std::cout << "Success: Inventory saved successfully."
              << std::endl;

    return true;
}

// --------------------------------------------------
// Load inventory
// --------------------------------------------------

bool FileManager::loadInventory(Inventory& inventory) const {
    std::ifstream file(inventoryFile);

    if (!file.is_open()) {
        // A missing file is normal on the first run.
        std::cout
            << "Info: No saved inventory found. "
            << "Starting with an empty inventory."
            << std::endl;

        return false;
    }

    int loaded = 0;
    int skipped = 0;
    std::string record;

    while (std::getline(file, record)) {
        if (record.empty()) {
            continue;
        }

        try {
            FoodItem item = FoodItem::fromFileString(record);
            inventory.addItem(item);
            ++loaded;
        } catch (const std::exception& error) {
            ++skipped;

            std::cerr
                << "Warning: Skipped invalid record: "
                << error.what()
                << std::endl;
        }
    }

    if (file.bad()) {
        std::cerr
            << "Error: An error occurred while reading the file."
            << std::endl;

        return false;
    }

    std::ostringstream message;
    message << "Loaded " << loaded << " food record(s).";

    if (skipped > 0) {
        message << " Skipped " << skipped
                << " invalid record(s).";
    }

    std::cout << "Success: " << message.str() << std::endl;

    return true;
}

// --------------------------------------------------
// Generate waste report
// --------------------------------------------------

bool FileManager::generateWasteReport(
    const Inventory& inventory
) const {
    std::ofstream file(reportFile);

    if (!file.is_open()) {
        std::cerr
            << "Error: Unable to open waste report file for writing."
            << std::endl;

        return false;
    }

    file << "============================================\n";
    file << "       FOOD WASTE AND EXPIRY REPORT\n";
    file << "============================================\n\n";

    file << std::fixed << std::setprecision(2);

    file << "Estimated total inventory value: Rs. "
         << inventory.getTotalInventoryValue() << '\n';

    file << "Estimated expired stock value: Rs. "
         << inventory.getTotalExpiredValue() << "\n\n";

    file << "EXPIRED FOOD ITEMS STILL IN STOCK\n";
    file << "--------------------------------------------\n";

    int count = 0;

    for (const FoodItem& item : inventory.getItems()) {
        if (item.getQuantity() <= 0 ||
            item.getExpiryStatus() != "Expired") {
            continue;
        }

        file << "ID: " << item.getId() << '\n'
             << "Name: " << item.getName() << '\n'
             << "Category: " << item.getCategory() << '\n'
             << "Remaining quantity: " << item.getQuantity()
             << ' ' << item.getUnit() << '\n'
             << "Unit price: Rs. " << item.getUnitPrice() << '\n'
             << "Estimated value: Rs. " << item.getTotalCost() << '\n'
             << "Expiry date: " << item.getExpiryDate() << "\n\n";

        ++count;
    }

    if (count == 0) {
        file << "No expired food items currently in stock.\n";
    }

    file << "\nTotal expired item records: " << count << '\n';
    file << "\nNote: This report estimates the value of expired stock.\n";
    file << "It does not independently track food already discarded.\n";

    file.close();

    if (!file) {
        std::cerr << "Error: Error saving the waste report."
                  << std::endl;

        return false;
    }

    std::cout << "Success: Waste report saved to "
              << reportFile << std::endl;

    return true;
}

// --------------------------------------------------
// Get file paths
// --------------------------------------------------

std::string FileManager::getInventoryFile() const {
    return inventoryFile;
}

std::string FileManager::getReportFile() const {
    return reportFile;
}
