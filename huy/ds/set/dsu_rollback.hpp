#include <vector>
#include <cassert>
#include <numeric>

#ifndef HUY_DS_SET_DSU_ROLLBACK
#define HUY_DS_SET_DSU_ROLLBACK
namespace huy {
namespace ds {

class dsu_rollback {
public:
    dsu_rollback(int n = 0) { 
        init(n);
    }

    void init(int n) {
        this->n = n;
        par.resize(n);
        std::iota(par.begin(), par.end(), 0);
        siz.assign(n, 1);
        st.clear();
        comp = n;
    }

    int find(int v) const {
        assert(0 <= v && v < n);
        while (v != par[v]) v = par[v];
        return v;
    }

    bool same(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        return find(a) == find(b);
    }

    int size(int v) const {
        assert(0 <= v && v < n);
        return siz[find(v)];
    }

    int comps() const {
        return comp;
    }

    int time() const {
        return static_cast<int>(st.size());
    }

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        a = find(a);
        b = find(b);
        if (a == b) {
            st.push_back({-1, -1, -1});
            return false;
        }
        if (siz[a] < siz[b]) std::swap(a, b);
        st.push_back({b, a, siz[a]});
        par[b] = a;
        siz[a] += siz[b];
        comp--;
        return true;
    }

    void rollback() {
        if (st.empty()) return;
        change c = st.back();
        st.pop_back();
        if (c.v == -1) return;
        par[c.v] = c.v;
        siz[c.u] = c.sz_u;
        comp++;
    }

    void rollback(int t) {
        assert(0 <= t && t <= time());
        while (time() > t) rollback();
    }

private:
    struct change {
        int v, u, sz_u;
    };

    int n, comp;
    std::vector<int> par, siz;
    std::vector<change> st;
};
    
} // namespace ds
} // namespace huy
#endif