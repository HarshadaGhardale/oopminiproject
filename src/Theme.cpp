
#include "Theme.h"

#include <limits>

// Disable terminal colours completely.
Theme::Color Theme::selectedColor = Theme::Color::Green;
bool Theme::colorsEnabled = false;

// --------------------------------------------------
// ANSI colour codes
// --------------------------------------------------

const char* Theme::getColorCode(Color color) {
    (void)color;
    return "";
}

// --------------------------------------------------
// Set and reset colours
// --------------------------------------------------

void Theme::setColor(Theme::Color color) {
    (void)color;
}

void Theme::resetColor() {
}

// --------------------------------------------------
// Formatted messages
// --------------------------------------------------

void Theme::printHeader(const std::string& title) {
    std::cout << "\n"
              << "=======================================================\n"
              << "       " << title << '\n'
              << "=======================================================\n";
}

void Theme::printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << '\n';
}

void Theme::printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << '\n';
}

void Theme::printError(const std::string& message) {
    std::cout << "[ERROR] " << message << '\n';
}

void Theme::printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << '\n';
}

void Theme::printLine(char symbol, int length) {
    if (length < 1) {
        length = 1;
    }

    std::cout << std::string(
        static_cast<std::size_t>(length), symbol
    ) << '\n';
}

// --------------------------------------------------
// Selected theme
// --------------------------------------------------

Theme::Color Theme::getSelectedColor() {
    return selectedColor;
}

void Theme::setSelectedColor(Color color) {
    selectedColor = color;
}

// --------------------------------------------------
// Enable or disable colours
// --------------------------------------------------

void Theme::enableColors(bool enabled) {
    colorsEnabled = enabled;
}

bool Theme::areColorsEnabled() {
    return colorsEnabled;
}

// --------------------------------------------------
// Colour selection menu
// --------------------------------------------------

void Theme::showThemeMenu() {
    std::cout << "\n========== SELECT TERMINAL THEME ==========\n";
    std::cout << "1. Green   - Eco Theme\n";
    std::cout << "2. Blue    - Professional Theme\n";
    std::cout << "3. Purple  - Modern Theme\n";
    std::cout << "4. Red     - Bold Theme\n";
    std::cout << "5. Cyan    - Tech Theme\n";
    std::cout << "6. Yellow  - Bright Theme\n";
    std::cout << "7. White   - Classic Theme\n";
    std::cout << "8. Default - Terminal Default\n";
    std::cout << "9. Disable colours\n";
    std::cout << "===========================================\n";
}

void Theme::selectTheme() {
    showThemeMenu();

    int choice;

    std::cout << "Choose a theme: ";

    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
        printError("Invalid theme selection.");
        return;
    }

    // Consume the remaining newline.
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(), '\n'
    );

    switch (choice) {
        case 1:
            setSelectedColor(Color::Green);
            break;
        case 2:
            setSelectedColor(Color::Blue);
            break;
        case 3:
            setSelectedColor(Color::Magenta);
            break;
        case 4:
            setSelectedColor(Color::Red);
            break;
        case 5:
            setSelectedColor(Color::Cyan);
            break;
        case 6:
            setSelectedColor(Color::Yellow);
            break;
        case 7:
            setSelectedColor(Color::White);
            break;
        case 8:
            setSelectedColor(Color::Default);
            break;
        case 9:
            enableColors(false);
            break;
        default:
            printWarning("Unknown theme choice.");
            return;
    }

    printSuccess("Theme setting updated. Colour output remains disabled.");
}
