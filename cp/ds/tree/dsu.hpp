#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_TREE_DSU
#define CP_DS_TREE_DSU
namespace cp {
namespace ds {

class dsu {
public:
    dsu(int n = 0) : n(n), comps(n), f(n, -1) {}

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        a = leader(a);
        b = leader(b);
        if (a == b) return false;
        if (-f[a] < -f[b]) std::swap(a, b);
        f[a] += f[b];
        f[b] = a;
        --comps;
        return true;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        return leader(a) == leader(b);
    }

    int find(int a) {
        assert(0 <= a && a < n);
        return leader(a);
    }

    int operator[](int a) {
        return find(a);
    }

    int size(int a) {
        assert(0 <= a && a < n);
        return -f[leader(a)];
    }

    int num_comps() const {
        return comps;
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> roots(n), group_idx(n, -1);
        std::vector<int> sizes(comps, 0);
        int idx = 0;

        for (int i = 0; i < n; ++i) {
            roots[i] = leader(i);
            if (group_idx[roots[i]] == -1) {
                group_idx[roots[i]] = idx++;
            }
            ++sizes[group_idx[roots[i]]];
        }

        std::vector<std::vector<int>> res(comps);
        for (int i = 0; i < comps; ++i) {
            res[i].reserve(sizes[i]);
        }

        for (int i = 0; i < n; ++i) {
            res[group_idx[roots[i]]].push_back(i);
        }

        return res;
    }

private:
    int n, comps;
    std::vector<int> f;

    int leader(int a) {
        if (f[a] < 0) return a;
        return f[a] = leader(f[a]);
    }
};

} // namespace ds
} // namespace cp
#endif