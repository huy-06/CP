#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "config.hpp"
#include "style.hpp"

#ifndef CP_INIT_RUNNER_DIFF_HPP
#define CP_INIT_RUNNER_DIFF_HPP

namespace cp {
namespace init {
namespace runner {
namespace diff {

inline std::vector<std::string> tokenize(std::string_view s) {
    std::vector<std::string> tokens;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && (static_cast<unsigned char>(s[i]) <= ' ')) {
            ++i;
        }
        if (i >= s.size()) break;
        size_t start = i;
        while (i < s.size() && (static_cast<unsigned char>(s[i]) > ' ')) {
            ++i;
        }
        tokens.emplace_back(s.substr(start, i - start));
    }
    return tokens;
}

inline bool has_float_chars(std::string_view s) {
    for (char c : s) {
        if (c == '.' || c == 'e' || c == 'E') return true;
    }
    return false;
}

inline bool token_equal(std::string_view a, std::string_view b, long double eps = config::float_eps) {
    if (a == b) return true;
    if (!has_float_chars(a) && !has_float_chars(b)) {
        return false;
    }
    try {
        std::size_t pos_a = 0;
        std::size_t pos_b = 0;
        std::string sa(a);
        std::string sb(b);
        long double val_a = std::stold(sa, &pos_a);
        long double val_b = std::stold(sb, &pos_b);
        if (pos_a == sa.length() && pos_b == sb.length()) {
            long double d = std::abs(val_a - val_b);
            return d <= eps * std::max(1.0L, std::abs(val_b));
        }
    } catch (...) {}
    return false;
}

inline bool check_match(const std::string& actual, const std::string& expected) {
    std::vector<std::string> act_tokens = tokenize(actual);
    std::vector<std::string> exp_tokens = tokenize(expected);
    if (act_tokens.size() != exp_tokens.size()) return false;
    for (size_t i = 0; i < act_tokens.size(); ++i) {
        if (!token_equal(act_tokens[i], exp_tokens[i])) {
            return false;
        }
    }
    return true;
}

struct token_item {
    std::string text;
    int newlines_after = 0;
};

inline std::vector<token_item> tokenize_with_lines(std::string_view s) {
    std::vector<token_item> items;
    size_t i = 0;
    while (i < s.size() && static_cast<unsigned char>(s[i]) <= ' ') {
        ++i;
    }
    while (i < s.size()) {
        size_t start = i;
        while (i < s.size() && static_cast<unsigned char>(s[i]) > ' ') {
            ++i;
        }
        std::string text(s.substr(start, i - start));

        int nl_count = 0;
        while (i < s.size() && static_cast<unsigned char>(s[i]) <= ' ') {
            if (s[i] == '\n') {
                ++nl_count;
            }
            ++i;
        }
        items.push_back({std::move(text), std::min(nl_count, 2)});
    }
    return items;
}

inline std::string truncate_token(std::string_view s, size_t max_len = config::preview_max_token_len) {
    if (s.size() > max_len) {
        return std::string(s.substr(0, max_len)) + "...";
    }
    return std::string(s);
}

inline void print_diff(const std::string& actual, const std::string& expected, const std::string& expected_name = "") {
    std::vector<token_item> exp_tokens = tokenize_with_lines(expected);
    std::vector<token_item> act_tokens = tokenize_with_lines(actual);

    // 1. In expected
    if (expected_name.empty()) {
        std::cout << style::color_black << "expected:" << style::reset << "\n";
    } else {
        std::cout << style::color_black << "expected: " << style::color_cyan << expected_name << style::reset << "\n";
    }
    if (exp_tokens.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
    } else {
        size_t exp_limit = std::min(exp_tokens.size(), config::preview_max_tokens);
        bool last_ended_with_nl = false;
        for (size_t i = 0; i < exp_limit; ++i) {
            bool is_in_actual = (i < act_tokens.size());
            bool match = (is_in_actual && token_equal(act_tokens[i].text, exp_tokens[i].text));
            std::string exp_str = truncate_token(exp_tokens[i].text);

            if (match) {
                std::cout << exp_str;
            } else if (is_in_actual) {
                // Actual có in nhưng sai giá trị -> màu vàng nổi bật
                std::cout << style::color_yellow << exp_str << style::reset;
            } else {
                // Actual in thiếu (hết output) -> dùng magenta
                std::cout << style::color_magenta << exp_str << style::reset;
            }

            if (exp_tokens[i].newlines_after > 0) {
                for (int k = 0; k < exp_tokens[i].newlines_after; ++k) {
                    std::cout << "\n";
                }
                last_ended_with_nl = true;
            } else if (i + 1 < exp_limit || exp_tokens.size() > exp_limit) {
                std::cout << " ";
                last_ended_with_nl = false;
            } else {
                std::cout << "\n";
                last_ended_with_nl = true;
            }
        }

        if (exp_tokens.size() > exp_limit) {
            std::cout << style::color_bright_black << "... (" << (exp_tokens.size() - exp_limit) << " more tokens)" << style::reset << "\n";
            last_ended_with_nl = true;
        }
        if (!last_ended_with_nl) {
            std::cout << "\n";
        }
    }

    // 2. In actual
    std::cout << style::color_black << "actual:" << style::reset << "\n";
    if (act_tokens.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
    } else {
        size_t act_limit = std::min(act_tokens.size(), config::preview_max_tokens);
        bool last_ended_with_nl = false;
        for (size_t i = 0; i < act_limit; ++i) {
            bool is_in_expected = (i < exp_tokens.size());
            bool match = (is_in_expected && token_equal(act_tokens[i].text, exp_tokens[i].text));
            std::string act_str = truncate_token(act_tokens[i].text);

            if (match) {
                std::cout << style::color_green << act_str << style::reset;
            } else {
                // Sai giá trị hoặc in thừa token ra ngoài expected -> màu đỏ
                std::cout << style::color_red << act_str << style::reset;
            }

            if (act_tokens[i].newlines_after > 0) {
                for (int k = 0; k < act_tokens[i].newlines_after; ++k) {
                    std::cout << "\n";
                }
                last_ended_with_nl = true;
            } else if (i + 1 < act_limit || act_tokens.size() > act_limit) {
                std::cout << " ";
                last_ended_with_nl = false;
            } else {
                std::cout << "\n";
                last_ended_with_nl = true;
            }
        }

        if (act_tokens.size() > act_limit) {
            std::cout << style::color_bright_black << "... (" << (act_tokens.size() - act_limit) << " more tokens)" << style::reset << "\n";
            last_ended_with_nl = true;
        }
        if (!last_ended_with_nl) {
            std::cout << "\n";
        }
    }
}

inline void print_preview(const std::string& text, const char* text_color = nullptr) {
    if (text.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
        return;
    }
    std::vector<token_item> tokens = tokenize_with_lines(text);
    if (tokens.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
        return;
    }
    size_t limit = std::min(tokens.size(), config::preview_max_tokens);
    bool last_ended_with_nl = false;
    for (size_t i = 0; i < limit; ++i) {
        std::string tok_str = truncate_token(tokens[i].text);
        if (text_color && *text_color) {
            std::cout << text_color << tok_str << style::reset;
        } else {
            std::cout << tok_str;
        }

        if (tokens[i].newlines_after > 0) {
            for (int k = 0; k < tokens[i].newlines_after; ++k) {
                std::cout << "\n";
            }
            last_ended_with_nl = true;
        } else if (i + 1 < limit || tokens.size() > limit) {
            std::cout << " ";
            last_ended_with_nl = false;
        } else {
            std::cout << "\n";
            last_ended_with_nl = true;
        }
    }

    if (tokens.size() > limit) {
        std::cout << style::color_bright_black << "... (" << (tokens.size() - limit) << " more tokens)" << style::reset << "\n";
        last_ended_with_nl = true;
    }
    if (!last_ended_with_nl) {
        std::cout << "\n";
    }
}

} // namespace diff
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_DIFF_HPP