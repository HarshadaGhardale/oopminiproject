
#include "FoodItem.h"

#include <sstream>
#include <vector>
#include <ctime>
#include <cmath>
#include <cctype>
#include <limits>
#include <initializer_list>
#include <stdexcept>

namespace {

// Split a saved record using the | separator.
std::vector<std::string> splitRecord(const std::string& record) {
    std::vector<std::string> fields;
    std::stringstream stream(record);
    std::string field;

    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }

    if (!record.empty() && record.back() == '|') {
        fields.push_back("");
    }

    return fields;
}

// Validate a date in YYYY-MM-DD format.
bool parseDate(const std::string& date, std::tm& result) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i != 4 && i != 7 &&
            !std::isdigit(static_cast<unsigned char>(date[i]))) {
            return false;
        }
    }

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
}

// Get today's date.
// MSVC uses localtime_s; GCC/MinGW uses std::localtime.
std::tm todayDate() {
    std::time_t now = std::time(nullptr);
    std::tm result{};

#ifdef _MSC_VER
    localtime_s(&result, &now);
#else
    std::tm* localTime = std::localtime(&now);
    if (localTime != nullptr) {
        result = *localTime;
    }
#endif

    result.tm_hour = 12;
    result.tm_min = 0;
    result.tm_sec = 0;
    result.tm_isdst = -1;
    std::mktime(&result);

    return result;
}

long long dayNumber(std::tm date) {
    date.tm_hour = 12;
    date.tm_min = 0;
    date.tm_sec = 0;
    date.tm_isdst = -1;

    std::time_t timestamp = std::mktime(&date);

    if (timestamp == -1) {
        throw std::runtime_error(
            "Unable to calculate date difference."
        );
    }

    return static_cast<long long>(timestamp / 86400);
}

long long daysFromToday(const std::string& date) {
    std::tm parsed{};

    if (!parseDate(date, parsed)) {
        throw std::invalid_argument(
            "Invalid date. Use YYYY-MM-DD."
        );
    }

    return dayNumber(parsed) - dayNumber(todayDate());
}

void validateDate(const std::string& date) {
    std::tm parsed{};

    if (!parseDate(date, parsed)) {
        throw std::invalid_argument(
            "Invalid date. Use a real date in YYYY-MM-DD format."
        );
    }
}

} // namespace

// --------------------------------------------------
// Constructors
// --------------------------------------------------

FoodItem::FoodItem()
    : id(0),
      name(""),
      category("Other"),
      quantity(0.0),
      unit("pcs"),
      unitPrice(0.0),
      purchaseDate(""),
      expiryDate("") {
}

FoodItem::FoodItem(
    int id,
    const std::string& name,
    const std::string& category,
    double quantity,
    const std::string& unit,
    double unitPrice,
    const std::string& purchaseDate,
    const std::string& expiryDate
)
    : id(id),
      name(name),
      category(category),
      quantity(quantity),
      unit(unit),
      unitPrice(unitPrice),
      purchaseDate(purchaseDate),
      expiryDate(expiryDate) {

    if (id < 0) {
        throw std::invalid_argument("ID cannot be negative.");
    }

    if (name.empty()) {
        throw std::invalid_argument("Food name cannot be empty.");
    }

    if (quantity < 0 || !std::isfinite(quantity)) {
        throw std::invalid_argument(
            "Quantity must be non-negative."
        );
    }

    if (unitPrice < 0 || !std::isfinite(unitPrice)) {
        throw std::invalid_argument(
            "Unit price must be non-negative."
        );
    }

    if (unit.empty()) {
        throw std::invalid_argument("Unit cannot be empty.");
    }

    validateDate(purchaseDate);
    validateDate(expiryDate);

    if (expiryDate < purchaseDate) {
        throw std::invalid_argument(
            "Expiry date cannot be before purchase date."
        );
    }
}

// --------------------------------------------------
// Getters
// --------------------------------------------------

int FoodItem::getId() const {
    return id;
}

std::string FoodItem::getName() const {
    return name;
}

std::string FoodItem::getCategory() const {
    return category;
}

double FoodItem::getQuantity() const {
    return quantity;
}

std::string FoodItem::getUnit() const {
    return unit;
}

double FoodItem::getUnitPrice() const {
    return unitPrice;
}

std::string FoodItem::getPurchaseDate() const {
    return purchaseDate;
}

std::string FoodItem::getExpiryDate() const {
    return expiryDate;
}

// --------------------------------------------------
// Setters
// --------------------------------------------------

void FoodItem::setName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Food name cannot be empty.");
    }

    this->name = name;
}

void FoodItem::setCategory(const std::string& category) {
    if (category.empty()) {
        throw std::invalid_argument("Category cannot be empty.");
    }

    this->category = category;
}

void FoodItem::setQuantity(double quantity) {
    if (quantity < 0 || !std::isfinite(quantity)) {
        throw std::invalid_argument("Invalid quantity.");
    }

    this->quantity = quantity;
}

void FoodItem::setUnit(const std::string& unit) {
    if (unit.empty()) {
        throw std::invalid_argument("Unit cannot be empty.");
    }

    this->unit = unit;
}

void FoodItem::setUnitPrice(double unitPrice) {
    if (unitPrice < 0 || !std::isfinite(unitPrice)) {
        throw std::invalid_argument("Invalid unit price.");
    }

    this->unitPrice = unitPrice;
}

void FoodItem::setPurchaseDate(const std::string& date) {
    validateDate(date);

    if (!expiryDate.empty() && expiryDate < date) {
        throw std::invalid_argument(
            "Purchase date cannot be after expiry date."
        );
    }

    purchaseDate = date;
}

void FoodItem::setExpiryDate(const std::string& date) {
    validateDate(date);

    if (!purchaseDate.empty() && date < purchaseDate) {
        throw std::invalid_argument(
            "Expiry date cannot be before purchase date."
        );
    }

    expiryDate = date;
}

// --------------------------------------------------
// Cost calculation
// --------------------------------------------------

double FoodItem::getTotalCost() const {
    return quantity * unitPrice;
}

// --------------------------------------------------
// Expiry status
// --------------------------------------------------

std::string FoodItem::getFoodType() const {
    return "Food";
}

std::string FoodItem::getExpiryStatus() const {
    if (expiryDate.empty()) {
        return "Unknown";
    }

    long long remainingDays = daysFromToday(expiryDate);

    if (remainingDays < 0) {
        return "Expired";
    }

    if (remainingDays == 0) {
        return "Expires Today";
    }

    if (remainingDays <= 3) {
        return "Expiring Soon";
    }

    return "Fresh";
}

// --------------------------------------------------
// Display food details
// --------------------------------------------------

void FoodItem::display() const {
    std::cout << std::left
              << std::setw(5)  << id
              << std::setw(20) << name
              << std::setw(14) << category
              << std::setw(10) << quantity
              << std::setw(9)  << unit
              << std::setw(12) << std::fixed
              << std::setprecision(2) << unitPrice
              << std::setw(13) << getTotalCost()
              << std::setw(13) << expiryDate
              << getExpiryStatus()
              << '\n';
}

// --------------------------------------------------
// Consume or discard food
// --------------------------------------------------

void FoodItem::consume(double amount) {
    if (!std::isfinite(amount) || amount <= 0) {
        throw std::invalid_argument(
            "Consumption amount must be positive."
        );
    }

    if (amount > quantity) {
        throw std::invalid_argument(
            "Cannot consume more than available quantity."
        );
    }

    quantity -= amount;

    if (quantity < 1e-9) {
        quantity = 0.0;
    }
}

void FoodItem::discard(double amount) {
    if (!std::isfinite(amount) || amount <= 0) {
        throw std::invalid_argument(
            "Discard amount must be positive."
        );
    }

    if (amount > quantity) {
        throw std::invalid_argument(
            "Cannot discard more than available quantity."
        );
    }

    quantity -= amount;

    if (quantity < 1e-9) {
        quantity = 0.0;
    }
}

// --------------------------------------------------
// Operator overloading
// --------------------------------------------------

FoodItem FoodItem::operator+(const FoodItem& other) const {
    if (name != other.name ||
        category != other.category ||
        unit != other.unit ||
        std::abs(unitPrice - other.unitPrice) > 1e-9) {
        throw std::invalid_argument(
            "Only compatible food items can be combined."
        );
    }

    FoodItem result(*this);
    result.quantity += other.quantity;

    return result;
}

bool FoodItem::operator<(const FoodItem& other) const {
    return expiryDate < other.expiryDate;
}

// --------------------------------------------------
// Convert a food item into a text-file record
// Format: ID|Name|Category|Quantity|Unit|Price|Purchase|Expiry
// --------------------------------------------------

std::string FoodItem::toFileString() const {
    std::ostringstream record;

    record << id << '|'
           << name << '|'
           << category << '|'
           << std::setprecision(15) << quantity << '|'
           << unit << '|'
           << std::setprecision(15) << unitPrice << '|'
           << purchaseDate << '|'
           << expiryDate;

    return record.str();
}

FoodItem FoodItem::fromFileString(const std::string& record) {
    std::vector<std::string> fields = splitRecord(record);

    if (fields.size() != 8) {
        throw std::invalid_argument(
            "Invalid food record format."
        );
    }

    std::size_t position = 0;

    int id = std::stoi(fields[0]);
    double quantity = std::stod(fields[3]);
    double price = std::stod(fields[5]);

    // Reject partially parsed numeric values.
    for (int index : {0, 3, 5}) {
        position = 0;

        if (index == 0) {
            std::stoi(fields[index], &position);
        } else {
            std::stod(fields[index], &position);
        }

        if (position != fields[index].size()) {
            throw std::invalid_argument(
                "Invalid numeric field."
            );
        }
    }

    return FoodItem(
        id,
        fields[1],
        fields[2],
        quantity,
        fields[4],
        price,
        fields[6],
        fields[7]
    );
}

// --------------------------------------------------
// PerishableFood: inheritance and polymorphism
// --------------------------------------------------

PerishableFood::PerishableFood()
    : FoodItem() {
}

PerishableFood::PerishableFood(
    int id,
    const std::string& name,
    const std::string& category,
    double quantity,
    const std::string& unit,
    double unitPrice,
    const std::string& purchaseDate,
    const std::string& expiryDate
)
    : FoodItem(
        id, name, category, quantity, unit,
        unitPrice, purchaseDate, expiryDate
    ) {
}

std::string PerishableFood::getFoodType() const {
    return "Perishable Food";
}

std::string PerishableFood::getExpiryStatus() const {
    return FoodItem::getExpiryStatus();
}

void PerishableFood::display() const {
    FoodItem::display();
}
