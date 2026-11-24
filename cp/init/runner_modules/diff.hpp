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

inline bool is_whitespace(char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

inline bool skip_ansi_sequence(std::string_view s, size_t& pos) noexcept {
    if (pos >= s.size() || static_cast<unsigned char>(s[pos]) != 0x1B) {
        return false;
    }
    size_t i = pos + 1;
    if (i < s.size() && s[i] == '[') {
        ++i;
        while (i < s.size()) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if (c >= 0x40 && c <= 0x7E) {
                pos = i + 1;
                return true;
            }
            ++i;
        }
        pos = s.size();
        return true;
    } else if (i < s.size()) {
        pos = i + 1;
        return true;
    }
    pos = s.size();
    return true;
}

inline std::vector<std::string> tokenize(std::string_view s) {
    std::vector<std::string> tokens;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && is_whitespace(s[i])) {
            ++i;
        }
        if (i >= s.size()) break;
        size_t start = i;
        while (i < s.size() && !is_whitespace(s[i])) {
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
    while (i < s.size() && is_whitespace(s[i])) {
        ++i;
    }
    while (i < s.size()) {
        size_t start = i;
        while (i < s.size() && !is_whitespace(s[i])) {
            ++i;
        }
        std::string text(s.substr(start, i - start));

        int nl_count = 0;
        while (i < s.size() && is_whitespace(s[i])) {
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
        size_t exp_limit = std::min(exp_tokens.size(), config::get_out_tokens());
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
        size_t act_limit = std::min(act_tokens.size(), config::get_actual_tokens());
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

    // 3. Smart Mismatch Context (SHOW-DIFF)
    if (config::show_diff) {
        size_t mismatch_idx = std::string::npos;
        size_t common_len = std::min(act_tokens.size(), exp_tokens.size());
        for (size_t i = 0; i < common_len; ++i) {
            if (!token_equal(act_tokens[i].text, exp_tokens[i].text)) {
                mismatch_idx = i;
                break;
            }
        }
        if (mismatch_idx == std::string::npos && act_tokens.size() != exp_tokens.size()) {
            mismatch_idx = common_len;
        }

        if (mismatch_idx != std::string::npos) {
            std::string exp_val = (mismatch_idx < exp_tokens.size()) ? truncate_token(exp_tokens[mismatch_idx].text) : "(end of expected)";
            std::string act_val = (mismatch_idx < act_tokens.size()) ? truncate_token(act_tokens[mismatch_idx].text) : "(missing, end of actual)";

            std::cout << style::color_black << "first mismatch: "
                      << style::color_white << "token " << style::color_blue << "#" << (mismatch_idx + 1)
                      << style::color_white << " [expected: " << style::color_yellow << exp_val << style::reset
                      << style::color_white << ", actual: " << style::color_red << act_val << style::reset
                      << style::color_white << "]" << style::reset << "\n";

            if (config::diff_context_radius > 0) {
                size_t radius = config::diff_context_radius;
                size_t start = (mismatch_idx >= radius) ? (mismatch_idx - radius) : 0;

                // Expected snippet
                std::cout << style::color_black << "  expected: " << style::reset;
                if (start > 0) std::cout << style::color_bright_black << "... " << style::reset;
                size_t end_exp = std::min(exp_tokens.size(), mismatch_idx + radius + 1);
                for (size_t i = start; i < end_exp; ++i) {
                    std::string tok = truncate_token(exp_tokens[i].text);
                    if (i == mismatch_idx) {
                        std::cout << style::color_yellow << "[" << tok << "]" << style::reset;
                    } else {
                        std::cout << tok;
                    }
                    if (i + 1 < end_exp || exp_tokens.size() > end_exp) std::cout << " ";
                }
                if (exp_tokens.size() > end_exp) std::cout << style::color_bright_black << "..." << style::reset;
                if (mismatch_idx >= exp_tokens.size()) std::cout << style::color_yellow << "[(end of expected)]" << style::reset;
                std::cout << "\n";

                // Actual snippet
                std::cout << style::color_black << "  actual:   " << style::reset;
                if (start > 0) std::cout << style::color_bright_black << "... " << style::reset;
                size_t end_act = std::min(act_tokens.size(), mismatch_idx + radius + 1);
                for (size_t i = start; i < end_act; ++i) {
                    std::string tok = truncate_token(act_tokens[i].text);
                    if (i == mismatch_idx) {
                        std::cout << style::color_red << "[" << tok << "]" << style::reset;
                    } else {
                        std::cout << style::color_green << tok << style::reset;
                    }
                    if (i + 1 < end_act || act_tokens.size() > end_act) std::cout << " ";
                }
                if (act_tokens.size() > end_act) std::cout << style::color_bright_black << "..." << style::reset;
                if (mismatch_idx >= act_tokens.size()) std::cout << style::color_red << "[(missing, end of actual)]" << style::reset;
                std::cout << "\n";
            }
        }
    }
}

inline void print_preview(const std::string& text, const char* text_color = nullptr, size_t max_tokens = 0) {
    if (text.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
        return;
    }
    std::vector<token_item> tokens = tokenize_with_lines(text);
    if (tokens.empty()) {
        std::cout << style::color_bright_black << "(empty)" << style::reset << "\n";
        return;
    }
    size_t limit = (max_tokens > 0) ? std::min(tokens.size(), max_tokens) : std::min(tokens.size(), config::preview_max_tokens);
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

inline void print_debug(const std::string& text, size_t max_tokens = 0) {
    if (text.empty()) return;

    size_t limit = (max_tokens > 0) ? max_tokens : config::get_debug_tokens();

    if (limit == std::numeric_limits<std::size_t>::max()) {
        std::cout << style::color_yellow << text << style::reset;
        if (text.back() != '\n') std::cout << "\n";
        return;
    }

    size_t total_tokens = 0;
    size_t cut_pos = text.size();
    bool in_token = false;
    size_t i = 0;
    const size_t n = text.size();

    while (i < n) {
        if (static_cast<unsigned char>(text[i]) == 0x1B) {
            size_t next_pos = i;
            skip_ansi_sequence(text, next_pos);
            i = next_pos;
            continue;
        }

        if (is_whitespace(text[i])) {
            if (in_token) {
                in_token = false;
                if (total_tokens == limit) {
                    cut_pos = i;
                }
            }
            ++i;
        } else {
            if (!in_token) {
                in_token = true;
                ++total_tokens;
                if (total_tokens == limit + 1 && cut_pos == n) {
                    cut_pos = i;
                }
            }
            ++i;
        }
    }

    if (in_token && total_tokens == limit) {
        cut_pos = n;
    }

    if (total_tokens <= limit) {
        std::cout << style::color_yellow << text << style::reset;
        if (!text.empty() && text.back() != '\n') std::cout << "\n";
    } else {
        std::cout << style::color_yellow;
        std::cout.write(text.data(), cut_pos);
        std::cout << style::reset;
        if (cut_pos > 0 && text[cut_pos - 1] != '\n') {
            std::cout << "\n";
        }
        std::cout << style::color_bright_black << "... (" << (total_tokens - limit) << " more debug tokens)" << style::reset << "\n";
    }
}

} // namespace diff
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_DIFF_HPP