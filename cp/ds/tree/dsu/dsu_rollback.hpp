#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_TREE_DSU_ROLLBACK
#define CP_DS_TREE_DSU_ROLLBACK

namespace cp {
namespace ds {

class dsu_rollback {
public:
    explicit dsu_rollback(int n = 0) : n(n), comp(n), f(n, -1) {
        history.reserve(n);
    }

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        a = leader(a);
        b = leader(b);
        if (a == b) return false;

        if (-f[a] < -f[b]) std::swap(a, b);

        history.push_back({a, f[a], b, f[b]});

        f[a] += f[b];
        f[b] = a;
        --comp;
        return true;
    }

    [[nodiscard]] bool same(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        return leader(a) == leader(b);
    }

    [[nodiscard]] int find(int a) const {
        assert(0 <= a && a < n);
        return leader(a);
    }

    [[nodiscard]] int operator[](int a) const {
        return find(a);
    }

    [[nodiscard]] int size(int a) const {
        assert(0 <= a && a < n);
        return -f[leader(a)];
    }

    [[nodiscard]] int comps() const noexcept {
        return comp;
    }

    [[nodiscard]] int time() const noexcept {
        return static_cast<int>(history.size());
    }

    void rollback() {
        if (history.empty()) return;
        state s = history.back();
        history.pop_back();

        f[s.u] = s.fu;
        f[s.v] = s.fv;
        ++comp;
    }

    void rollback(int t) {
        assert(0 <= t && t <= time());
        while (time() > t) {
            rollback();
        }
    }

    [[nodiscard]] int snapshot() const noexcept {
        return time();
    }

    [[nodiscard]] std::vector<std::vector<int>> groups() const {
        std::vector<std::vector<int>> res(n);
        for (int i = 0; i < n; ++i) {
            res[leader(i)].push_back(i);
        }
        res.erase(
            std::remove_if(res.begin(), res.end(), [](const std::vector<int>& v) {
                return v.empty();
            }),
            res.end()
        );
        return res;
    }

    [[nodiscard]] int leader(int a) const noexcept {
        while (f[a] >= 0) {
            a = f[a];
        }
        return a;
    }

private:
    int n = 0;
    int comp = 0;
    std::vector<int> f;

    struct state {
        int u, fu;
        int v, fv;
    };
    std::vector<state> history;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_TREE_DSU_ROLLBACK