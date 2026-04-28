#include <iostream>
#include <source_location>
#include "../ds/misc/formatter.hpp"

#ifndef CP_DEBUG
#define CP_DEBUG
namespace cp {
namespace internal {

template <typename Head, typename... Tail>
void show(const std::string& indent, std::string names, Head head, Tail... tail) {
    int bracket = 0, comma_pos = -1;
    for (size_t i = 0; i < names.length(); ++i) {
        if (names[i] == '(' || names[i] == '{' || names[i] == '<' || names[i] == '[') ++bracket;
        if (names[i] == ')' || names[i] == '}' || names[i] == '>' || names[i] == ']') --bracket;
        if (names[i] == ',' && bracket == 0) { comma_pos = i; break; }
    }

    std::string name = (comma_pos == -1) ? names : names.substr(0, comma_pos);
    size_t first = name.find_first_not_of(" \t");
    size_t last = name.find_last_not_of(" \t");
    if (first != std::string::npos) name = name.substr(first, last - first + 1);

    std::cerr << indent << style::color_white << name << style::reset;
    std::cerr << style::color_magenta << " = " << style::reset;
    cp::internal::print_item(std::cerr, head);
    
    if constexpr (sizeof...(tail) > 0) {
        std::cerr << "\n";
        show(indent, names.substr(comma_pos + 1), tail...);
    } else {
        std::cerr << "\n";
    }
}

template <typename... Args>
void console(const std::source_location& loc, const std::string& names, Args... args) {
    int col = loc.column();
    std::string indent(col > 1 ? col - 1 : 0, ' ');

    std::cerr << indent << style::color_bright_black << "[Line " << loc.line() << "]\n" << style::reset;
    show(indent, names, args...);
}

} // namespace internal
} // namespace cp

#define debug(...) cp::internal::console(std::source_location::current(), #__VA_ARGS__, __VA_ARGS__)

#endif  