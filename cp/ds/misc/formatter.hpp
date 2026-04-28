//<
#include <ostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include "../../cst/style.hpp"

#ifndef CP_DS_FORMATTER
#define CP_DS_FORMATTER
namespace cp {
namespace internal {

namespace style = cp::cst::style;

inline thread_local int bracket_depth = 0;

inline std::string_view get_bracket_color() {
    int d = bracket_depth % 6;
    if (d == 0) return style::color_white;
    if (d == 1) return style::color_magenta;
    if (d == 2) return style::color_cyan;
    if (d == 3) return style::color_green;
    if (d == 4) return style::color_blue;
    return style::color_yellow;
}

inline void open_bracket(std::ostream& os, const char* str) {
    os << get_bracket_color() << str << style::reset;
    ++bracket_depth;
}

inline void close_bracket(std::ostream& os, const char* str) {
    --bracket_depth;
    os << get_bracket_color() << str << style::reset;
}

template <typename Tp, typename Enable = void>
struct formatter {
    static void print(std::ostream& os, const Tp& item) {
        if constexpr (std::is_arithmetic_v<Tp>) {
            os << style::color_blue << item << style::reset;
        } else {
            os << style::color_cyan << item << style::reset;
        }
    }
};

template <typename Tp>
void print_item(std::ostream& os, const Tp& item) {
    formatter<Tp>::print(os, item);
}

template <>
struct formatter<std::string> {
    static void print(std::ostream& os, const std::string& s) {
        os << style::color_yellow << '"' << s << '"' << style::reset;
    }
};

template <>
struct formatter<char> {
    static void print(std::ostream& os, const char& c) {
        os << style::color_yellow << '\'' << c << '\'' << style::reset;
    }
};

template <typename A, typename B>
struct formatter<std::pair<A, B>> {
    static void print(std::ostream& os, const std::pair<A, B>& p) {
        open_bracket(os, "(");
        print_item(os, p.first);
        os << ", ";
        print_item(os, p.second);
        close_bracket(os, ")");
    }
};

template <typename... Tp>
struct formatter<std::tuple<Tp...>> {
    template <std::size_t... Id>
    static void print_tuple(std::ostream& os, const std::tuple<Tp...>& t, std::index_sequence<Id...>) {
        open_bracket(os, "(");
        (..., (os << (Id == 0 ? "" : ", "), print_item(os, std::get<Id>(t))));
        close_bracket(os, ")");
    }

    static void print(std::ostream& os, const std::tuple<Tp...>& t) {
        print_tuple(os, t, std::index_sequence_for<Tp...>{});
    }
};

template <typename... Args>
struct formatter<std::vector<Args...>> {
    static void print(std::ostream& os, const std::vector<Args...>& v) {
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
    }
};

template <typename... Args>
struct formatter<std::array<Args...>> {
    static void print(std::ostream& os, const std::array<Args...>& v) {
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
    }
};


template <typename... Args>
struct formatter<std::set<Args...>> {
    static void print(std::ostream& os, const std::set<Args...>& v) {
        os << style::color_green << "set" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::multiset<Args...>> {
    static void print(std::ostream& os, const std::multiset<Args...>& v) {
        os << style::color_green << "multiset" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::unordered_set<Args...>> {
    static void print(std::ostream& os, const std::unordered_set<Args...>& v) {
        os << style::color_green << "unordered_set" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::map<Args...>> {
    static void print(std::ostream& os, const std::map<Args...>& v) {
        os << style::color_green << "map" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x.first);
            os << ": ";
            print_item(os, x.second);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::unordered_map<Args...>> {
    static void print(std::ostream& os, const std::unordered_map<Args...>& v) {
        os << style::color_green << "unordered_map" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x.first);
            os << ": ";
            print_item(os, x.second);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::stack<Args...>> {
    static void print(std::ostream& os, std::stack<Args...> s) {
        using Tp = typename std::stack<Args...>::value_type;
        std::vector<Tp> v;
        while (!s.empty()) {
            v.push_back(s.top());
            s.pop();
        }
        std::reverse(v.begin(), v.end());
        os << style::color_green << "stack" << style::reset;
        open_bracket(os, "(");
        print_item(os, v);
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::queue<Args...>> {
    static void print(std::ostream& os, std::queue<Args...> q) {
        using Tp = typename std::queue<Args...>::value_type;
        std::vector<Tp> v;
        while (!q.empty()) {
            v.push_back(q.front());
            q.pop();
        }
        os << style::color_green << "queue" << style::reset;
        open_bracket(os, "(");
        print_item(os, v);
        close_bracket(os, ")");
    }
};

template <typename... Args>
struct formatter<std::priority_queue<Args...>> {
    static void print(std::ostream& os, std::priority_queue<Args...> pq) {
        using Tp = typename std::priority_queue<Args...>::value_type;
        std::vector<Tp> v;
        while (!pq.empty()) {
            v.push_back(pq.top());
            pq.pop();
        }
        std::reverse(v.begin(), v.end());
        os << style::color_green << "priority_queue" << style::reset;
        open_bracket(os, "(");
        print_item(os, v);
        close_bracket(os, ")");
    }
};

} // namespace internal
} // namespace cp
#endif
//>