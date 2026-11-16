#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

#include "../cst/style.hpp"

#ifndef CP_INIT_PREVIEW_STYLES
#define CP_INIT_PREVIEW_STYLES

namespace cp {
namespace init {

void preview_styles() {
    using namespace cp::cst::style;
    using item = std::pair<const char*, const char*>;

    auto print_group = [&](const char* title, const std::vector<item>& items) {
        std::cout << "\n=== " << title << " ===\n";
        for (const auto& item : items) {
            std::cout << "  " << std::left << std::setw(26) << item.first
                      << " | " << item.second << " Hello Terminal 123! " << reset << "\n";
        }
    };

    print_group("FOREGROUND COLORS (NORMAL)", {
        {"color_black",   color_black},
        {"color_red",     color_red},
        {"color_green",   color_green},
        {"color_yellow",  color_yellow},
        {"color_blue",    color_blue},
        {"color_magenta", color_magenta},
        {"color_cyan",    color_cyan},
        {"color_white",   color_white}
    });

    print_group("FOREGROUND COLORS (BRIGHT)", {
        {"color_bright_black",   color_bright_black},
        {"color_bright_red",     color_bright_red},
        {"color_bright_green",   color_bright_green},
        {"color_bright_yellow",  color_bright_yellow},
        {"color_bright_blue",    color_bright_blue},
        {"color_bright_magenta", color_bright_magenta},
        {"color_bright_cyan",    color_bright_cyan},
        {"color_bright_white",   color_bright_white}
    });

    print_group("BACKGROUND COLORS (NORMAL)", {
        {"backgroud_black",   backgroud_black},
        {"backgroud_red",     backgroud_red},
        {"backgroud_green",   backgroud_green},
        {"backgroud_yellow",  backgroud_yellow},
        {"backgroud_blue",    backgroud_blue},
        {"backgroud_magenta", backgroud_magenta},
        {"backgroud_cyan",    backgroud_cyan},
        {"backgroud_white",   backgroud_white}
    });

    print_group("BACKGROUND COLORS (BRIGHT)", {
        {"backgroud_bright_black",   backgroud_bright_black},
        {"backgroud_bright_red",     backgroud_bright_red},
        {"backgroud_bright_green",   backgroud_bright_green},
        {"backgroud_bright_yellow",  backgroud_bright_yellow},
        {"backgroud_bright_blue",    backgroud_bright_blue},
        {"backgroud_bright_magenta", backgroud_bright_magenta},
        {"backgroud_bright_cyan",    backgroud_bright_cyan},
        {"backgroud_bright_white",   backgroud_bright_white}
    });

    print_group("TEXT STYLES", {
        {"style_bold",      style_bold},
        {"style_dim",       style_dim},
        {"style_italic",    style_italic},
        {"style_underline", style_underline},
        {"style_blink",     style_blink},
        {"style_reverse",   style_reverse},
        {"style_hidden",    style_hidden},
        {"style_strike",    style_strike}
    });

    std::cout << "\n(Luu y: style_hidden se an chu o cot ben phai, chi thay khoang trong)\n";
}

} // namespace init
} // namespace cp

#endif // CP_INIT_PREVIEW_STYLES