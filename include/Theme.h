
#ifndef THEME_H
#define THEME_H

#include <iostream>
#include <string>

class Theme {
public:
    enum class Color {
        Default,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    // Selected terminal theme
    static void setColor(Color color);
    static void resetColor();

    // Display helpers
    static void printHeader(const std::string& title);
    static void printSuccess(const std::string& message);
    static void printWarning(const std::string& message);
    static void printError(const std::string& message);
    static void printInfo(const std::string& message);
    static void printLine(char symbol = '-', int length = 55);

    // User-selectable colour theme
    static void showThemeMenu();
    static void selectTheme();

    // Get or change the selected colour
    static Color getSelectedColor();
    static void setSelectedColor(Color color);

    // Enable or disable coloured output
    static void enableColors(bool enabled);
    static bool areColorsEnabled();

private:
    static Color selectedColor;
    static bool colorsEnabled;

    static const char* getColorCode(Color color);
};

#endif
