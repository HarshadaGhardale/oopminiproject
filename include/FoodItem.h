
#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>

class FoodItem {
protected:
    int id;
    std::string name;
    std::string category;
    double quantity;
    std::string unit;
    double unitPrice;
    std::string purchaseDate;
    std::string expiryDate;

public:
    // Default constructor
    FoodItem();

    // Parameterized constructor
    FoodItem(
        int id,
        const std::string& name,
        const std::string& category,
        double quantity,
        const std::string& unit,
        double unitPrice,
        const std::string& purchaseDate,
        const std::string& expiryDate
    );

    virtual ~FoodItem() = default;

    // Getters
    int getId() const;
    std::string getName() const;
    std::string getCategory() const;
    double getQuantity() const;
    std::string getUnit() const;
    double getUnitPrice() const;
    std::string getPurchaseDate() const;
    std::string getExpiryDate() const;

    // Setters
    void setName(const std::string& name);
    void setCategory(const std::string& category);
    void setQuantity(double quantity);
    void setUnit(const std::string& unit);
    void setUnitPrice(double unitPrice);
    void setPurchaseDate(const std::string& date);
    void setExpiryDate(const std::string& date);

    // Cost calculations
    double getTotalCost() const;

    // Expiry information
    virtual std::string getFoodType() const;
    virtual std::string getExpiryStatus() const;
    virtual void display() const;

    // Update quantity after consumption or disposal
    void consume(double amount);
    void discard(double amount);

    // Operator overloading: combine quantities of compatible items
    FoodItem operator+(const FoodItem& other) const;

    // Operator overloading: compare expiry dates
    bool operator<(const FoodItem& other) const;

    // File handling helpers
    std::string toFileString() const;
    static FoodItem fromFileString(const std::string& record);
};

// Inheritance and runtime polymorphism
class PerishableFood : public FoodItem {
public:
    PerishableFood();

    PerishableFood(
        int id,
        const std::string& name,
        const std::string& category,
        double quantity,
        const std::string& unit,
        double unitPrice,
        const std::string& purchaseDate,
        const std::string& expiryDate
    );

    std::string getFoodType() const override;
    std::string getExpiryStatus() const override;
    void display() const override;
};

#endif
