#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef CP_DS_RANDOM
#define CP_DS_RANDOM

namespace cp {
namespace ds {

struct graph_opt {
    bool connected = true;         // Mặc định liên thông
    bool directed = false;         // Vô hướng
    bool dag = false;              // DAG (có hướng, không chu trình)
    bool allow_self_loop = false;  // Không khuyên
    bool allow_multi_edge = false; // Không đa cạnh
    int base = 1;                  // Đánh số đỉnh từ 1
};

class random {
public:
    using graph_opt = cp::ds::graph_opt;

private:
    static auto& engine() {
        static thread_local std::mt19937_64 gen([]{
            std::random_device rd;
            std::seed_seq ss{ 
                static_cast<uint32_t>(rd()), 
                static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
            };
            return std::mt19937_64(ss);
        }());
        return gen;
    }

    template <typename T1, typename T2> 
    struct interval_proxy {
        T1 min_val;
        T2 max_val;
        
        const interval_proxy& operator>>(std::integral auto& var) const {
            using val_t = std::remove_reference_t<decltype(var)>;
            using dist_t = std::conditional_t<(sizeof(val_t) < sizeof(int)) || std::same_as<val_t, bool>, int, val_t>;
            var = static_cast<val_t>(std::uniform_int_distribution<dist_t>(min_val, max_val)(engine()));
            return *this;
        }
        
        const interval_proxy& operator>>(std::floating_point auto& var) const {
            using val_t = std::remove_reference_t<decltype(var)>;
            var = std::uniform_real_distribution<val_t>(min_val, max_val)(engine());
            return *this;
        }
    };

    template <typename Tp> 
    struct list_proxy {
        std::vector<Tp> data;
        const list_proxy& operator>>(auto& var) const {
            if (!data.empty()) {
                var = data[std::uniform_int_distribution<size_t>(0, data.size() - 1)(engine())];
            }
            return *this;
        }
    };

public:
    template<std::integral Tp = int64_t>
    static Tp nint(std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                   std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        using dist_t = std::conditional_t<(sizeof(Tp) < sizeof(int)) || std::same_as<Tp, bool>, int, Tp>;
        return static_cast<Tp>(std::uniform_int_distribution<dist_t>(l, r)(engine()));
    }

    template<std::floating_point Tp = double>
    static Tp nreal(std::type_identity_t<Tp> l = 0.0, 
                    std::type_identity_t<Tp> r = 1.0) {
        return std::uniform_real_distribution<Tp>(l, r)(engine());
    }

    static bool nbool() {
        return nint(0, 1) == 1;
    }

    static char nchar(char l = 'a', char r = 'z') {
        return static_cast<char>(nint(l, r));
    }

    static std::string nstr(int n, std::string_view charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
        if (charset.empty()) return "";
        std::string res(n, '\0');
        for (char& c : res) c = charset[nint<size_t>(0, charset.size() - 1)];
        return res;
    }

    template<std::integral Tp = int64_t>
    static auto nvec(int n, 
                     std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                     std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        std::vector<Tp> res(n);
        std::ranges::generate(res, [&]{ return nint<Tp>(l, r); });
        return res;
    }

    template<typename Tp>
    static auto nvec(int n, std::initializer_list<Tp> list) {
        std::vector<Tp> src(list), res(n);
        if (src.empty()) return res;
        for (auto& x : res) x = src[nint<size_t>(0, src.size() - 1)];
        return res;
    }

    static auto choice(std::ranges::random_access_range auto const& c) {
        assert(!std::ranges::empty(c));
        return *(std::ranges::begin(c) + nint<size_t>(0, std::ranges::size(c) - 1));
    }

    template<typename Tp>
    static Tp choice(std::initializer_list<Tp> list) { 
        assert(list.size() > 0);
        return *(list.begin() + nint<size_t>(0, list.size() - 1)); 
    }
    
    static void shuffle(std::ranges::random_access_range auto& c) {
        std::ranges::shuffle(c, engine());
    }

    static bool prob(double p) {
        return std::bernoulli_distribution(p)(engine());
    }

    // Cây không trọng số (tự pha trộn nhiều dáng: ngẫu nhiên, sâu sqrt(n), sâu róm, path, star)
    static std::vector<std::pair<int, int>> ntree(int n, int base = 1) {
        assert(n >= 1);
        std::vector<std::pair<int, int>> edges;
        if (n == 1) return edges;
        edges.reserve(n - 1);

        std::vector<int> perm(n);
        std::iota(perm.begin(), perm.end(), 0);
        shuffle(perm);

        int mode = nint<int>(0, 9);
        int sq = std::max(1, static_cast<int>(std::sqrt(n)));
        int hub_max = nbool() ? 0 : 2;

        for (int i = 1; i < n; ++i) {
            int p = 0;
            if (mode < 4)   
                p = nint<int>(0, i - 1);                    // Cây nông O(log n)
            else if (mode < 6)
                p = nint<int>(std::max(0, i - sq), i - 1);  // Cây sâu vừa O(sqrt(n))
            else if (mode < 8)
                p = nint<int>(std::max(0, i - 3), i - 1);   // Cây sâu róm O(n)
            else if (mode == 8)
                p = i - 1;                                  // Đường thẳng (Path)
            else
                p = nint<int>(0, std::min(i - 1, hub_max)); // Hình sao / đa tâm (Star / Hub)

            int u = perm[p], v = perm[i];
            if (nbool()) std::swap(u, v);
            edges.emplace_back(u + base, v + base);
        }
        shuffle(edges);
        return edges;
    }

    // Cây có trọng số trong [min_w, max_w]
    template<typename W = int64_t>
    requires std::integral<W> || std::floating_point<W>
    static std::vector<std::tuple<int, int, W>> ntree(
        int n,
        std::type_identity_t<W> min_w,
        std::type_identity_t<W> max_w,
        int base = 1)
    {
        assert(min_w <= max_w);
        std::vector<std::tuple<int, int, W>> res;
        for (auto [u, v] : ntree(n, base)) {
            W w = [&]() {
                if constexpr (std::integral<W>) return nint<W>(min_w, max_w);
                else return nreal<W>(min_w, max_w);
            }();
            res.emplace_back(u, v, w);
        }
        return res;
    }

    // Đồ thị không trọng số (mặc định liên thông, đơn đồ thị, vô hướng, m = -1 tự chọn số cạnh)
    static std::vector<std::pair<int, int>> ngraph(int n, int m = -1, graph_opt opt = {}) {
        assert(n >= 0 && m >= -1);
        if (n == 0) {
            assert(m <= 0);
            return {};
        }
        if (opt.dag) {
            opt.directed = true;
            assert(!opt.allow_self_loop);
        }
        if (n == 1 && !opt.allow_self_loop) {
            assert(m <= 0);
            return {};
        }

        int64_t n64 = n;
        int64_t min_m = (opt.connected && n > 1) ? (n64 - 1) : 0;
        int64_t max_m = 0;
        if (opt.dag)
            max_m = n64 * (n64 - 1) / 2;
        else if (opt.directed)
            max_m = opt.allow_self_loop ? n64 * n64 : n64 * (n64 - 1);
        else
            max_m = opt.allow_self_loop ? n64 * (n64 + 1) / 2 : n64 * (n64 - 1) / 2;

        if (m == -1) {
            int64_t auto_hi = std::max<int64_t>(min_m, 2LL * n64);
            if (!opt.allow_multi_edge) auto_hi = std::min(auto_hi, max_m);
            m = static_cast<int>(nint<int64_t>(min_m, auto_hi));
        } else {
            assert(m >= min_m);
            if (!opt.allow_multi_edge) assert(m <= max_m);
        }

        std::vector<std::pair<int, int>> edges;
        edges.reserve(m);
        std::unordered_set<uint64_t> used;

        auto make_key = [&](int u, int v) -> uint64_t {
            if (!opt.directed || opt.dag) {
                if (u > v) std::swap(u, v);
            }
            return (static_cast<uint64_t>(u) << 32) | static_cast<uint32_t>(v);
        };

        if (opt.connected && n > 1) {
            for (auto [u, v] : ntree(n, 0)) {
                if (opt.dag && u > v) std::swap(u, v);
                if (!opt.allow_multi_edge) used.insert(make_key(u, v));
                edges.emplace_back(u, v);
            }
        }

        if (opt.allow_multi_edge) {
            while (static_cast<int>(edges.size()) < m) {
                int u = nint<int>(0, n - 1);
                int v = opt.allow_self_loop ? nint<int>(0, n - 1) : nint<int>(0, n - 2);
                if (!opt.allow_self_loop && v >= u) ++v;
                if (opt.dag && u > v) std::swap(u, v);
                edges.emplace_back(u, v);
            }
        } else if (m <= max_m / 2) {
            used.reserve(static_cast<size_t>(m) * 2 + 1);
            while (static_cast<int>(edges.size()) < m) {
                int u = nint<int>(0, n - 1);
                int v = opt.allow_self_loop ? nint<int>(0, n - 1) : nint<int>(0, n - 2);
                if (!opt.allow_self_loop && v >= u) ++v;
                if (opt.dag && u > v) std::swap(u, v);
                if (used.insert(make_key(u, v)).second) {
                    edges.emplace_back(u, v);
                }
            }
        } else {
            std::vector<std::pair<int, int>> avail;
            avail.reserve(max_m - used.size());
            for (int u = 0; u < n; ++u) {
                int v_start = (opt.directed && !opt.dag) ? 0 : (opt.allow_self_loop ? u : u + 1);
                for (int v = v_start; v < n; ++v) {
                    if (!opt.allow_self_loop && u == v) continue;
                    if (!used.contains(make_key(u, v))) avail.emplace_back(u, v);
                }
            }
            int rem = m - static_cast<int>(edges.size());
            for (int i = 0; i < rem; ++i) {
                int j = nint<int>(i, static_cast<int>(avail.size()) - 1);
                std::swap(avail[i], avail[j]);
                edges.push_back(avail[i]);
            }
        }

        if (opt.dag) {
            std::vector<int> topo(n);
            std::iota(topo.begin(), topo.end(), 0);
            shuffle(topo);
            for (auto& [u, v] : edges) {
                u = topo[u];
                v = topo[v];
            }
        }

        for (auto& [u, v] : edges) {
            if (!opt.directed && u != v && nbool()) std::swap(u, v);
            u += opt.base;
            v += opt.base;
        }
        shuffle(edges);
        return edges;
    }

    // Overload cho phép truyền thẳng option khi dùng auto m: rng::ngraph(n, {.directed = true})
    static std::vector<std::pair<int, int>> ngraph(int n, graph_opt opt) {
        return ngraph(n, -1, opt);
    }

    // Đồ thị có trọng số (chỉ định m, hoặc m = -1 để tự chọn)
    template<typename W = int64_t>
    requires std::integral<W> || std::floating_point<W>
    static std::vector<std::tuple<int, int, W>> ngraph(
        int n,
        int m,
        std::type_identity_t<W> min_w,
        std::type_identity_t<W> max_w,
        graph_opt opt = {})
    {
        assert(min_w <= max_w);
        std::vector<std::tuple<int, int, W>> res;
        for (auto [u, v] : ngraph(n, m, opt)) {
            W w = [&]() {
                if constexpr (std::integral<W>) return nint<W>(min_w, max_w);
                else return nreal<W>(min_w, max_w);
            }();
            res.emplace_back(u, v, w);
        }
        return res;
    }

    // Đồ thị có trọng số, tự động chọn số cạnh m
    template<typename W = int64_t>
    requires std::integral<W> || std::floating_point<W>
    static auto ngraph(int n, std::type_identity_t<W> min_w, std::type_identity_t<W> max_w, graph_opt opt = {}) {
        return ngraph<W>(n, -1, min_w, max_w, opt);
    }

    random& operator>>(bool& var) {
        var = nbool();
        return *this;
    }

    random& operator>>(char& var) {
        var = nchar();
        return *this;
    }
    
    random& operator>>(std::integral auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nint());
        return *this; 
    }
    
    random& operator>>(std::floating_point auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nreal());
        return *this; 
    }

    auto operator()(auto min, auto max) {
        return interval_proxy<decltype(min), decltype(max)>{min, max};
    }

    template <typename Tp>
    auto operator()(std::initializer_list<Tp> list) {
        return list_proxy<Tp>{list};
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_RANDOM