#include "tree.hpp"

#ifndef CP_DS_TREE_HLD
#define CP_DS_TREE_HLD
namespace cp {
namespace ds {

template <typename Edge>
class hld : public tree<Edge> {
public:
    using edge_type = Edge;
    using tree<edge_type>::num_vertices;

    hld(int n = 0) {
        init(n);
    }

    hld(const graph<edge_type>& g, int u = 0) {
        init(g, u);
    }

    hld(const tree<edge_type>& tree, int u = 0) {
        init(tree, u);
    }

    void init(int n) {
        tree<edge_type>::init(n);
        inp.assign(n, 0);
        out.assign(n, 0);
        ord.assign(n, 0);
        lead.assign(n, 0);
        heavy.assign(n, -1);
        timer = 0;
        built = false;
    }

    void init(const graph<edge_type>& g, int u = 0) {
        assert(0 <= u && u < g.num_vertices());
        init(g.num_vertices());
        for (const auto& e : g.get_edges()) {
            if (e.from < e.to) {
                tree<edge_type>::add_edge(e);
            }
        }
        build(u);
    }

    void init(const tree<edge_type>& tree, int u = 0)  {
        assert(0 <= u && u < tree.num_vertices());
        init(tree.num_vertices());
        for (const auto& e : tree.get_edges()) {
            if (e.from < e.to) {
                add_edge(e);
            }
        }
        build(u);
    }

    void build(int u = 0) {
        assert(0 <= u && u < num_vertices());
        if (built) return;
        tree<edge_type>::build(u);
        dfs1(u);
        lead[u] = u;
        dfs2(u);
        built = true;
    }

    int top(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return lead[u];
    }

    int input(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return inp[u];
    }

    int output(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return out[u];
    }

    int tour(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return ord[u];
    }

    int lca(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        while (lead[u] != lead[v]) {
            if (dep[lead[u]] > dep[lead[v]]) {
                u = par[lead[u]];
            } else {
                v = par[lead[v]];
            }   
        }
        return dep[u] < dep[v] ? u : v;
    }

    int dist(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        return dep[u] + dep[v] - 2 * dep[lca(u, v)];
    }

    int jump(int u, int k) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        int d = dep[u] - k;
        while (dep[lead[u]] > d) {
            u = par[lead[u]];
        }
        return ord[inp[u] - dep[u] + d];
    }

    bool is_ancestor(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        return inp[u] <= inp[v] && inp[v] < out[u];
    }

    int rooted_parent(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        std::swap(u, v);
        if (u == v) return u;
        if (!is_ancestor(u, v)) return par[u];
        for (const auto& e : (*this)[u]) {
            if (par[e.to] != u) continue;
            if (is_ancestor(e.to, v)) return e.to;
        }
        return -1;
    }

    int root_size(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        if (u == v) return num_vertices();
        if (!is_ancestor(v, u)) return siz[v];
        return num_vertices() - siz[rooted_parent(u, v)];
    }

    int rooted_lca(int a, int b, int c) {
        assert(0 <= a && a < num_vertices());
        assert(0 <= b && b < num_vertices());
        assert(0 <= c && c < num_vertices());
        if (!built) build();
        return lca(lca(a, b), c);
    }

    template <typename Data>
    void range_apply(Data& d, int u, int v, const typename Data::value_tag& t) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        while (lead[u] != lead[v]) {
            if (dep[lead[u]] < dep[lead[v]]) {
                std::swap(u, v);
            }
            d.range_apply(inp[lead[u]], inp[u] + 1, t);
            u = par[lead[u]];
        }
        if (u == v) return;
        if (dep[u] > dep[v]) {
            std::swap(u, v);
        }
        d.range_apply(inp[u], inp[v] + 1, t);
    }

    template <typename Data>
    typename Data::value_type range_query(Data& d, int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        using value_type = typename Data::value_type;
        value_type res;
        while (lead[u] != lead[v]) {
            if (dep[lead[u]] < dep[lead[v]]) {
                std::swap(u, v);
            }
            value_type tmp = d.range_query(inp[lead[u]], inp[u] + 1);
            res = res + tmp;
            u = par[lead[u]];
        }
        if (u == v) return res;
        if (dep[u] > dep[v]) {
            std::swap(u, v);
        }
        // inp[u] + 1
        value_type tmp = d.range_query(inp[u], inp[v] + 1);
        res = res + tmp;
        return res;
    }

private:
    using tree<edge_type>::siz;
    using tree<edge_type>::par;
    using tree<edge_type>::dep;
    using tree<edge_type>::head;
    using tree<edge_type>::built;
    using tree<edge_type>::edge_list;

    std::vector<int> inp;
    std::vector<int> out;
    std::vector<int> ord;
    std::vector<int> lead;
    std::vector<int> heavy;
    int timer;

    void dfs1(int u) {
        int best = -1;
        siz[u] = 1;
        for (auto& e : (*this)[u]) {
            int v = e.to;
            if (v == par[u]) continue;
            par[v] = u;
            dep[v] = dep[u] + 1;
            dfs1(v);
            siz[u] += siz[v];
            if (best == -1 || siz[v] > siz[best]) best = v;
        }
        heavy[u] = best;
    }

    void dfs2(int u) {
        inp[u] = timer++;
        ord[inp[u]] = u;
        if (heavy[u] != -1) {
            lead[heavy[u]] = lead[u];
            dfs2(heavy[u]);
        }
        for (auto& e : (*this)[u]) {
            int v = e.to;
            if (v == par[u] || v == heavy[u]) continue;
            lead[v] = v;
            dfs2(v);
        }
        out[u] = timer;
    }
};

} // namespace ds
} // namespace cp
#endif