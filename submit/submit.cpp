/// @author Huy
/// @since  08:08:32 08/11/2025
#include <bits/stdc++.h>
#include <vector>
#include <array>

#ifndef HUY_DS_LINK_CUT_TREE
#define HUY_DS_LINK_CUT_TREE

namespace huy {
namespace ds {

template <typename Tp>
class link_cut_tree {
public:
    using value_type = Tp;

    std::vector<std::array<int, 2>> chi;
    std::vector<int> par;
    std::vector<int> rev;

    std::vector<int> siz;
    std::vector<value_type> val;
    std::vector<value_type> sum;
    std::vector<value_type> add;

    link_cut_tree(int n = 0) {
        init(n);
    }

    void init(int n) {
        this->n = n;
        chi.assign(n, {-1, -1});
        par.assign(n, -1);
        rev.assign(n, 0);
        val.assign(n, 0);
        sum.assign(n, 0);
        add.assign(n, 0);
        siz.assign(n, 1);
    }

    inline bool is_root(int x) {
        int p = par[x];
        if (p == -1) return true;
        return !(chi[p][0] == x || chi[p][1] == x);
    }

    inline void pull(int x) {
        siz[x] = 1;
        sum[x] = val[x];
        for (int d = 0; d < 2; ++d) {
            int c = chi[x][d];
            if (c != -1) {
                siz[x] += siz[c];
                sum[x] += sum[c];
            }
        }
    }

    inline void apply_add(int x, int v) {
        if (x == -1) return;
        add[x] += v;
        val[x] += v;
        sum[x] += v * siz[x];
    }

    inline void apply_rev(int x) {
        if (x == -1) return;
        rev[x] ^= 1;
        std::swap(chi[x][0], chi[x][1]);
    }

    inline void push(int x) {
        if (x == -1) return;
        if (rev[x]) {
            if (chi[x][0] != -1) apply_rev(chi[x][0]);
            if (chi[x][1] != -1) apply_rev(chi[x][1]);
            rev[x] = 0;
        }
        if (add[x] != 0) {
            int v = add[x];
            if (chi[x][0] != -1) apply_add(chi[x][0], v);
            if (chi[x][1] != -1) apply_add(chi[x][1], v);
            add[x] = 0;
        }
    }


    void rotate(int x) {
        int y = par[x];
        int z = par[y];
        int dx = (chi[y][1] == x);
        int b = chi[x][dx ^ 1];

        if (!is_root(y)) {
            if (chi[z][0] == y) chi[z][0] = x;
            else if (chi[z][1] == y) chi[z][1] = x;
        }
        par[x] = z;

        if (b != -1) par[b] = y;
        chi[y][dx] = b;

        chi[x][dx ^ 1] = y;
        par[y] = x;

        pull(y);
        pull(x);
    }

    void push_path(int x) {
        static std::vector<int> stk;
        stk.clear();
        int u = x;
        stk.push_back(u);
        while (!is_root(u)) {
            u = par[u];
            stk.push_back(u);
        }
        for (int i = int(stk.size()) - 1; i >= 0; --i) push(stk[i]);
    }

    void splay(int x) {
        push_path(x);
        while (!is_root(x)) {
            int y = par[x];
            int z = par[y];
            if (!is_root(y)) {
                if ((chi[y][0] == x) ^ (chi[z][0] == y)) rotate(x);
                else rotate(y);
            }
            rotate(x);
        }
    }

    int access(int x) {
        int last = -1;
        for (int v = x; v != -1; v = par[v]) {
            splay(v);
            chi[v][1] = last;
            pull(v);
            last = v;
        }
        return last;
    }

    void make_root(int x) {
        access(x);
        splay(x);
        apply_rev(x);
    }

    int find_root(int x) {
        access(x);
        splay(x);
        while (true) {
            push(x);
            if (chi[x][0] == -1) break;
            x = chi[x][0];
        }
        splay(x);
        return x;
    }

    bool connected(int x, int y) {
        if (x == y) return true;
        return find_root(x) == find_root(y);
    }

    bool link(int u, int v) {
        make_root(u);
        if (find_root(v) == u) return false;
        par[u] = v;
        return true;
    }

    bool cut(int u, int v) {
        make_root(u);
        access(v);
        splay(v);
        if (chi[v][0] == u && chi[u][1] == -1) {
            chi[v][0] = -1;
            par[u] = -1;
            pull(v);
            return true;
        }
        make_root(v);
        access(u);
        splay(u);
        if (chi[u][0] == v && chi[v][1] == -1) {
            chi[u][0] = -1;
            par[v] = -1;
            pull(u);
            return true;
        }
        return false;
    }

    int lca(int u, int v) {
        if (!connected(u, v)) return -1;
        access(u);
        return access(v);
    }

    void set_value(int x, const value_type& v) {
        access(x);
        splay(x);
        val[x] = v;
        pull(x);
    }

    void add_path(int u, int v, const value_type& delta) {
        make_root(u);
        access(v);
        splay(v);
        apply_add(v, delta);
    }

    value_type query_path_sum(int u, int v) {
        make_root(u);
        access(v);
        splay(v);
        return sum[v];
    }

private:
    int n;
};

} // namespace ds
} // namespace huy

#endif

signed main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);


    return 0;
}