//<
#ifndef HUY_LIB_DEBUG
#define HUY_LIB_DEBUG

#if __cplusplus >= 201402L

#ifdef int 
#undef int
#endif

#pragma once
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <chrono>
#include <math.h>  
#include <limits>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <bitset>
#include <random>
#include <functional>
#include <ext/pb_ds/assoc_container.hpp>

#ifdef LEETCODE
#include <bits/stdc++.h>
using namespace std;
// #include <bits/extc++.h>
// #include <ext/pb_ds/assoc_container.hpp>

// Data structures leet code

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() {
        val = 0;
        neighbors = vector<Node*>();
    }
    Node(int _val) {
        val = _val;
        neighbors = vector<Node*>();
    }
    Node(int _val, vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
};
#endif

#ifdef _WIN32
#include <io.h>
#define is_terminal(stream) _isatty(_fileno(stream))
#else
#include <unistd.h>
#define is_terminal(stream) isatty(fileno(stream))
#endif

static bool huy_is_terminal_output = is_terminal(stdout) || is_terminal(stderr);


std::ofstream file_output("E:/Code/Tasks/debug.txt");

// #define __output__ file_output
#define __output__ std::cerr

std::ostream& operator<<(std::ostream& out, const __float128& value);    
std::ostream& operator<<(std::ostream& out, const __int128& value);

template <typename ValueType, size_t StaticRank>
class MultiArray;

namespace huy {
namespace ds {

template <typename Tp>
class multiset;

}
}

class HuyDebug {
private:
    template<typename T>
    std::string to_string_debug(const T& var) {
        std::ostringstream oss;
        oss << var;
        return oss.str();
    }

    template<typename T1, typename T2>
    std::string to_string_debug(const std::pair<T1, T2>& p) {
        std::ostringstream oss;
        oss << "(" << to_string_debug(p.first) << ", " << to_string_debug(p.second) << ")";
        return oss.str();
    }

    template<typename... T>
    std::string to_string_debug(const std::tuple<T...>& t) {
        constexpr std::size_t N = sizeof...(T); 
        std::ostringstream oss;
        oss << "(";
        std::apply(
            [&oss](auto&&... elems) {
                std::size_t idx = 0;
                ((oss << to_string_debug(elems) << (++idx < N ? ", " : "")), ...);
            },
            t
        );
        oss << ")";
        return oss.str();
    }

    template<typename T>
    std::string to_string_debug(const std::vector<T>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }

    template<typename T, size_t N>
    std::string to_string_debug(const std::array<T, N>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }

#ifdef _GLIBCXX_STACK
    template<typename T>
    std::string to_string_debug(std::stack<T> a) {
        std::vector<T> v;
        while (!a.empty()) {
            v.push_back(a.top());
            a.pop();
        }
        std::reverse(v.begin(), v.end());
        return to_string_debug(v);
    }
#endif

#ifdef _STL_QUEUE_H
    template<typename T>
    std::string to_string_debug(std::queue<T> a) {
        std::vector<T> v;
        while (!a.empty()) {
            v.push_back(a.front());
            a.pop();
        }
        return to_string_debug(v);
    }

    template<typename T1, typename T2, typename T3>
    std::string to_string_debug(std::priority_queue<T1, T2, T3> a) {
        std::vector<T1> v;
        while (!a.empty()) {
            v.push_back(a.top());
            a.pop();
        }
        std::reverse(v.begin(), v.end());
        return to_string_debug(v);
    }
#endif

#ifdef _GLIBCXX_DEQUE
    template<typename T>
    std::string to_string_debug(const std::deque<T>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef _GLIBCXX_SET
    template<typename T>
    std::string to_string_debug(const std::set<T>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef _UNORDERED_SET_H
    template<typename T>
    std::string to_string_debug(const std::unordered_set<T>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef _STL_MULTISET_H
    template<typename T>
    std::string to_string_debug(const std::multiset<T>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef _STL_MAP_H
    template<typename K, typename V>
    std::string to_string_debug(const std::map<K, V>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef _UNORDERED_MAP_H
    template<typename K, typename V, typename C = std::hash<K>>
    std::string to_string_debug(const std::unordered_map<K, V, C>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }
#endif

#ifdef PB_DS_ASSOC_CNTNR_HPP
    template<typename T, typename Cmp = std::less<T>>
    std::string to_string_debug(const __gnu_pbds::tree<T, 
                                        __gnu_pbds::null_type,
                                        Cmp, 
                                        __gnu_pbds::rb_tree_tag, 
                                        __gnu_pbds::tree_order_statistics_node_update>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }

    template<typename K, typename V, typename C = typename __gnu_pbds::detail::default_hash_fn<K>::type>
    std::string to_string_debug(const __gnu_pbds::gp_hash_table<K, V, C>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();
    }

    template <typename Tp>
    std::string to_string_debug(const huy::ds::multiset<Tp>& a) {
        std::ostringstream oss;
        oss << "[";
        bool ok = false;
        for (const auto& it : a) {
            if (ok) oss << ", ";
            oss << to_string_debug(it);
            ok = true;
        }
        oss << "]";
        return oss.str();        
    }
#endif

#ifdef LEETCODE
    std::string to_string_debug(ListNode* node) {
        std::vector<int> v;
        while (node) {
            v.push_back(node->val);
            node = node->next;
        }
        return to_string_debug(v);
    }

    std::string to_string_debug(TreeNode* head) {
        std::vector<std::vector<int>> v;
        std::queue<TreeNode*> Q;
        Q.push(head);
        for (int i = 0; !Q.empty(); ++i) {
            if (!Q.front()) continue;
            v.push_back({});
            int n = Q.size();
            for (int j = 0; j < n; ++j) {
                v.back().push_back(Q.front()->val);
                if (Q.front()->left) Q.push(Q.front()->left);
                if (Q.front()->right) Q.push(Q.front()->right);
                Q.pop();
            }
            Q.pop();
        }
        return to_string_debug(v);
    }
#endif

    template<typename T, size_t R>
    std::string to_string_debug(const MultiArray<T, R>& tv) {
        auto shape   = tv.dimensions();
        auto strides = tv.strides();
        auto data    = tv.data();
        std::array<size_t, R> idx{};
        std::ostringstream oss;

        std::function<void(size_t)> rec = [&](size_t dim) {
            oss << '[';
            for (size_t i = 0; i < shape[dim]; ++i) {
                idx[dim] = i;
                if (dim + 1 < R) {
                    rec(dim + 1);
                } else {
                    size_t offset = 0;
                    for (size_t d = 0; d < R; ++d) {
                        offset += idx[d] * strides[d];
                    }
                    oss << to_string_debug(data[offset]);
                }
                if (i + 1 < shape[dim]) oss << ", ";
            }
            oss << ']';
        };

        rec(0);
        return oss.str();
    }

    std::vector<std::string> split_string(const std::string& s) {
        int count = 1, depth = 0;
        for (char c : s) {
            if (c == '(' || c == '[' || c == '{') depth++;
            else if (c == ')' || c == ']' || c == '}') depth--;
            else if (c == ',' && depth == 0) count++;
        }

        std::vector<std::string> result;
        result.reserve(count);

        std::string token;
        depth = 0;
        for (char c : s) {
            if (c == '(' || c == '[' || c == '{') {
                depth++;
                token += c;
            } else if (c == ')' || c == ']' || c == '}') {
                depth--;
                token += c;
            } else if (c == ',' && depth == 0) {
                if (!token.empty()) {
                    result.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }

        if (!token.empty()) {
            result.push_back(token);
        }

        for (auto& t : result) {
            size_t start = t.find_first_not_of(" \t\n\r\f\v");
            size_t end = t.find_last_not_of(" \t\n\r\f\v");
            t = (start != std::string::npos && end != std::string::npos) ? t.substr(start, end - start + 1) : "";
        }

        return result;
    }

    std::string normalize_nested_brackets(const std::string& name, const std::string& value) {
        if (value.empty() || value.front() != '[' || value.back() != ']' || value.find('(') != std::string::npos) {
            return name + " = " + value;
        }
    
        std::string result = name + " = ";
        int indent0 = static_cast<int>(name.size()) + 3;
        int level = 0;
    
        for (size_t i = 0; i < value.size(); ++i) {
            char c = value[i];
            if (c == '[') {
                result += '[';
                ++level;
            } else if (c == ']') {
                --level;
                result += ']';
            } else if (c == ',') {
                result += ',';
                char prev = (result.size() >= 2 ? result[result.size() - 2] : '\0');
                if (prev == ']') {
                    result += '\n';
                    result += std::string(indent0 + level, ' ');
                } else {
                    result += ' ';
                }
            } else {
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    result += c;
                }
            }
        }
    
        return result;
    }    

public:
    template<typename... Args>
    void process(const std::string& name, const Args&... args) {
        std::vector<std::string> names = split_string(name);
        std::vector<std::string> values;
        values.reserve(sizeof...(Args));

        (values.emplace_back(to_string_debug(args)), ...);

        for (int i = 0; i < static_cast<int>(names.size()); ++i) {
            if (huy_is_terminal_output)
                __output__ << "\033[33m" << normalize_nested_brackets(names[i], values[i]) << "\033[0m\n";
            else
                __output__ << normalize_nested_brackets(names[i], values[i]) << "\n";
        }
    }
} huy_debug;

// #define debug(...) { __output__ << "\x1b[32mDebug at line \033[35m" << __LINE__ << "\x1b[0m:\n"; huy_debug.process(#__VA_ARGS__ __VA_OPT__(,) __VA_ARGS__); }
#define debug(...) do { \
    if constexpr (sizeof(#__VA_ARGS__) == 1) { \
        /* __VA_ARGS__ empty */ \
    } else { \
       if (huy_is_terminal_output) \
            __output__ << "\x1b[32mDebug at line \033[35m" << __LINE__ << "\x1b[0m:\n"; \
        else \
            __output__ << "Debug at line " << __LINE__ << ":\n"; \
        huy_debug.process(#__VA_ARGS__ __VA_OPT__(,) __VA_ARGS__); \
    } \
} while (0);

// template<typename T>
// std::string bit(T num) {
//     return std::bitset<sizeof(T) * 8> (num).to_string();
// }

#pragma once

#ifdef COUT_CYAN

#include <iostream>
#include <streambuf>
#include <cstring>

#ifdef _WIN32
  #include <io.h>
  #include <windows.h>
  #define isatty _isatty
  #define fileno  _fileno
#else
  #include <unistd.h>
#endif

namespace cout_cyan {

static constexpr const char* kPrefix = "\x1b[36m";
static constexpr const char* kSuffix = "\x1b[0m";

inline bool is_stdout_terminal() {
#ifdef _WIN32
    return isatty(fileno(stdout)) != 0;
#else
    return isatty(fileno(stdout)) != 0;
#endif
}

class colorbuf : public std::streambuf {
public:
    explicit colorbuf(std::streambuf* dest) : dest_(dest) {}

protected:
    virtual int_type overflow(int_type ch) {
        if (traits_type::eq_int_type(ch, traits_type::eof())) return traits_type::not_eof(ch);
        if (!write_prefix())                return traits_type::eof();
        if (dest_->sputc(traits_type::to_char_type(ch)) == traits_type::eof()) return traits_type::eof();
        if (!write_suffix())                return traits_type::eof();
        return ch;
    }

    virtual std::streamsize xsputn(const char* s, std::streamsize n) {
        if (n <= 0) return 0;
        if (!write_prefix()) return 0;
        std::streamsize wrote = dest_->sputn(s, n);
        if (!write_suffix()) return wrote;
        return wrote;
    }

    virtual int sync() {
        return dest_->pubsync();
    }

private:
    std::streambuf* dest_;

    bool write_prefix() {
        const std::size_t L = std::strlen(kPrefix);
        std::streamsize r = dest_->sputn(kPrefix, static_cast<std::streamsize>(L));
        return r == static_cast<std::streamsize>(L);
    }
    bool write_suffix() {
        const std::size_t L = std::strlen(kSuffix);
        std::streamsize r = dest_->sputn(kSuffix, static_cast<std::streamsize>(L));
        return r == static_cast<std::streamsize>(L);
    }
};

class installer {
public:
    installer() : old_(nullptr), colored_(nullptr), installed_(false) {
#ifdef _WIN32
        enable_vt_on_windows();
#endif
        if (is_stdout_terminal()) {
            old_ = std::cout.rdbuf();
            colored_ = new colorbuf(old_);
            std::cout.rdbuf(colored_);
            installed_ = true;
        }
    }

    ~installer() {
        if (installed_) {
            std::cout.rdbuf(old_);
            std::cout << kSuffix;
            delete colored_;
            colored_ = nullptr;
            old_ = nullptr;
            installed_ = false;
        }
    }

    installer(const installer&) = delete;
    installer& operator=(const installer&) = delete;

private:
#ifdef _WIN32
    static void enable_vt_on_windows() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        (void)SetConsoleMode(hOut, dwMode);
    }
#endif

    std::streambuf* old_;
    colorbuf* colored_;
    bool installed_;
};

static installer auto_install;

} // namespace cout_cyan

#endif


#ifdef LEETCODE
// Read data leet coode

namespace leetcode {
    inline void __read__(int& n) {
        std::cin >> n;
    }

    inline void __read__(std::string& s) {
        std::string tmp;
        std::getline(std::cin, tmp);
        s = tmp.substr(1, tmp.size() - 2);
    }

    template<typename T>
    inline void __read__(std::vector<T>& v) {
        std::string s;
        std::getline(std::cin, s);
        std::string space = "[],'\"";
        std::stringstream ss;
        for (auto it : s) {
            if (std::find(space.begin(), space.end(), it) != space.end()) ss << ' ';
            else ss << it;
        }
        v.clear();
        T val;
        while (ss >> val) {
            v.push_back(val);
        }
    }

    template<typename T>
    inline void __read__(std::vector<std::vector<T>>& v) {
        std::string s;
        std::getline(std::cin, s);
        std::string space = "[,'\"";
        std::stringstream ss;
        for (auto it : s) {
            if (it == ']') ss << '\n';
            else if (std::find(space.begin(), space.end(), it) != space.end()) ss << ' ';
            else ss << it;
        }
        v.clear();
        while (std::getline(ss, s)) {
            std::vector<T> tmp;
            std::stringstream ss2(s);
            T val;
            while (ss2 >> val) tmp.push_back(val);
            v.push_back(tmp);
        }
        v.pop_back();
    }

    inline void __read__(ListNode *&node) {
        std::vector<int> v;
        __read__(v);
        if (v.empty()) {
            node = nullptr;
            return;
        }
        node = new ListNode(v[0]);
        ListNode* dummy = node;
        for (int i = 1; i < (int) v.size(); ++i) {
            dummy->next = new ListNode(v[i]);
            dummy = dummy->next;
        }
    }

    inline void __read__(TreeNode *&node) {
        std::vector<std::string> v;
        __read__(v);
        if (v.empty() || v[0] == "null") {
            node = nullptr;
            return;
        }
        node = new TreeNode(std::stoi(v[0]));
        auto build = [&](auto&& self, TreeNode* node, int id = 1) -> void {
            if (id * 2 - 1 < (int) v.size() && v[id * 2 - 1] != "null") {
                node->left = new TreeNode(std::stoi(v[id * 2 - 1]));
                self(self, node->left, id * 2);
            }
            if (id * 2 < (int) v.size() && v[id * 2] != "null") {
                node->right = new TreeNode(std::stoi(v[id * 2]));
                self(self, node->right, id * 2 + 1);
            }
        };
        build(build, node);
    }

    template<typename... Args>
    void read(Args&... args) {
        ( __read__(args), ... );
    }
};

#endif





#ifdef cin
#include <random> 

namespace std {
    namespace RandomInput {
        long long limit = 100;

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> long_dist(0, limit - 1);
        std::uniform_int_distribution<int> int_dist(0, limit - 1);
        std::uniform_int_distribution<char> char_dist('a', 'z');

        void setLimit(long long val) {
            limit = val;
            long_dist = std::uniform_int_distribution<long long>(0, limit - 1);
            int_dist = std::uniform_int_distribution<int>(0, limit - 1);
        }

        long long randomLongLong() {
            return long_dist(gen);
        }

        int randomInt() {
            return int_dist(gen);
        }

        std::string randomString() {
            std::string v;
            int _size = randomInt() % 10 + 1;
            for (int i = 0; i < _size; ++i) {
                v += char_dist(gen);
            }
            return v;
        }

        struct RandomCin {
            std::istream& stream;

            RandomCin(std::istream& inStream) : stream(inStream) {}

            void tie(int n) {}

            RandomCin& operator>>(int& v) {
                v = randomInt();
                return *this;
            }

            RandomCin& operator>>(long long& v) {
                v = randomLongLong();
                return *this;
            }

            RandomCin& operator>>(std::string& v) {
                v = randomString();
                return *this;
            }
        };
    };
    RandomInput::RandomCin randCin(std::cin);
};
#endif

#else
#define debug(...)
#endif

#endif
//>
