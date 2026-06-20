#ifndef HUY_RANDOM
#define HUY_RANDOM

#include <fstream>
#include <vector>
#include <random>

namespace ran {
    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    template<typename Tp>
    Tp num(Tp l = 0, Tp r = 100000) {
        if constexpr (std::is_integral_v<Tp>) {
            std::uniform_int_distribution<Tp> dist(l, r);
            return dist(rng);
        } else if constexpr (std::is_floating_point_v<Tp>) {
            std::uniform_real_distribution<Tp> dist(l, std::nextafter(r, std::numeric_limits<Tp>::max()));
            return dist(rng);
        }
    }

    char choice(const std::string & s) {
        return s[num<int>(0, s.size() - 1)];
    }

    template<typename Tp>
    Tp choice(const std::vector<Tp>& v) {
        return v[num<int>(0, v.size() - 1)];
    }

    template<typename Tp>
    Tp choice(std::initializer_list<Tp> l) {
        return choice(std::vector<Tp>(l));
    }

    std::string str(int n) {
        std::string s;
        s.reserve(n);
        for (int i = 0; i < n; ++i) {
            s += 'a' + num<int>(0, 25);
        }
        return s;
    }

    template<typename Fun>
    std::string vec(int n, Fun f) {
        std::string s;
        s.reserve(n);
        for (int i = 0; i < n; ++i) {
            s.push_back(f());
        }
        return s;
    }

    template<typename Tp>
    std::vector<Tp> vec(int n, Tp l = 0, Tp r = 100000) {
        std::vector<Tp> v;
        v.reserve(n);
        for (int i = 0; i < n; ++i) {
            v.push_back(num<Tp>(l, r));
        }
        return v;
    }

    template<typename Fun>
    auto vec(int n, Fun f) {
        using Tp = decltype(f());
        std::vector<Tp> v;
        v.reserve(n);
        for (int i = 0; i < n; ++i) {
            v.push_back(f());
        }
        return v;
    }

    std::vector<std::pair<int,int>> tree(int n) {
        std::vector<std::pair<int,int>> edges;
        edges.reserve(n - 1);
        for (int i = 2; i <= n; i++) {
            int p = num<int>(1, i - 1);
            edges.emplace_back(p, i);
        }
        return edges;
    }

    std::vector<std::pair<int,int>> graph(int n, int m, bool directed = false, bool allow_cycle = true) {
        std::set<std::pair<int,int>> used;
        std::vector<std::pair<int,int>> edges;
        edges.reserve(m);

        auto add_edge = [&](int u, int v) {
            if (!directed && u > v) std::swap(u, v);
            if (!used.count({u, v})) {
                used.insert({u, v});
                edges.emplace_back(u, v);
                return true;
            }
            return false;
        };

        if (!allow_cycle) {
            if (!directed) {
                return tree(n);
            } else {
                std::vector<int> perm(n);
                std::iota(perm.begin(), perm.end(), 1);
                std::shuffle(perm.begin(), perm.end(), rng);

                while ((int)edges.size() < m) {
                    int i = num<int>(0, n - 2);
                    int j = num<int>(i + 1, n - 1);
                    add_edge(perm[i], perm[j]);
                }
                return edges;
            }
        }

        while ((int)edges.size() < m) {
            int u = num<int>(1, n);
            int v = num<int>(1, n);
            if (u == v) continue;
            add_edge(u, v);
        }
        return edges;
    }


    std::string convert(std::string s) {
        return s;
    }

    template<typename Tp>
    std::string convert(Tp v) {
        return std::to_string(v);
    }

    template<typename Tp>
    std::string convert(std::vector<Tp> v) {
        std::string s;
        for (int i = 0; i < (int)v.size(); ++i) {
            if (i) s += ' ';
            s += std::to_string(v[i]);
        }
        return s;
    }

    std::string convert(const std::vector<std::pair<int,int>>& edges) {
        std::string s;
        for (int i = 0; i < (int)edges.size(); i++) {
            s += std::to_string(edges[i].first) + " " + std::to_string(edges[i].second);
            if (i + 1 < (int)edges.size()) s += '\n';
        }
        return s;
    }


    std::ofstream fout;

    template<typename Tp>
    Tp write(Tp v) {
        fout << convert(v) << '\n';
        return v;
    }


    template<typename... Args>
    auto write(const Args&... args) {
        ((fout << convert(args) << ' '), ...) << '\n';
        return std::make_tuple(args...);
    }
};

#endif