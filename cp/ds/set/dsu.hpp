#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_SET_DSU
#define CP_DS_SET_DS
namespace cp {
namespace ds {

class dsu {
public:
    dsu(int n = 0) : n(n), f(n, -1) {}

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        a = leader(a);
        b = leader(b);
        if (a == b) return false;
        if (-f[a] < -f[b]) std::swap(a, b);
        f[a] += f[b];
        f[b] = a;
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

    std::vector<std::vector<int>> groups() {
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

private:
    int n;
    std::vector<int> f;

    int leader(int a) {
        if (f[a] < 0) return a;
        return f[a] = leader(f[a]);
    }
};

} // namespace ds
} // namespace cp
#endif