#include "../base/tree.hpp"

#ifndef CP_DS_TREE_HLD
#define CP_DS_TREE_HLD

namespace cp {
namespace ds {

template <typename Edge>
class hld : public tree<Edge> {
public:
    using edge_type = Edge;
    using tree<edge_type>::num_vertices;
    using tree<edge_type>::adj;

    explicit hld(int n = 0, int root = 0) : tree<Edge>(n, root) {
        init(n, root);
    }

    explicit hld(const graph<edge_type>& g, int root = 0) : tree<Edge>(g, root) {
        init(g, root);
    }

    explicit hld(const tree<edge_type>& tr, int root = 0) : tree<Edge>(tr, root) {
        init(tr, root);
    }

    void init(int n, int root = 0) {
        tree<edge_type>::init(n, root);
        inp.assign(n, 0);
        out.assign(n, 0);
        ord.assign(n, 0);
        lead.assign(n, 0);
        heavy.assign(n, -1);
        timer = 0;
        built = false;
    }

    void init(const graph<edge_type>& g, int root = 0) {
        assert(0 <= root && root < g.num_vertices());
        init(g.num_vertices(), root);
        for (const auto& e : g.get_edges()) {
            if (e.from < e.to) {
                tree<edge_type>::add_edge(e);
            }
        }
        build(root);
    }

    void init(const tree<edge_type>& tr, int root = 0) {
        assert(0 <= root && root < tr.num_vertices());
        init(tr.num_vertices(), root);
        for (const auto& e : tr.get_edges()) {
            if (e.from < e.to) {
                this->add_edge(e);
            }
        }
        build(root);
    }

    void build() override {
        if (built) return;
        build(root);
    }

    void build(int root) override {
        assert(0 <= root && root < num_vertices());
        if (built && this->root == root) return;

        tree<edge_type>::build(root);

        if (n == 0) {
            built = true;
            return;
        }

        inp.assign(n, 0);
        out.assign(n, 0);
        ord.assign(n, 0);
        lead.assign(n, 0);
        heavy.assign(n, -1);
        timer = 0;

        for (int u = 0; u < n; ++u) {
            int best = -1;
            for (const auto& e : adj(u)) {
                int v = e.to;
                if (v == par[u]) continue;
                if (best == -1 || siz[v] > siz[best]) {
                    best = v;
                }
            }
            heavy[u] = best;
        }

        lead[root] = root;
        std::vector<int> stk;
        stk.reserve(n);
        stk.push_back(root);

        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();

            inp[u] = timer++;
            ord[inp[u]] = u;
            out[u] = inp[u] + siz[u];

            int h = heavy[u];
            for (const auto& e : adj(u)) {
                int v = e.to;
                if (v == par[u] || v == h) continue;
                lead[v] = v;
                stk.push_back(v);
            }
            if (h != -1) {
                lead[h] = lead[u];
                stk.push_back(h);
            }
        }

        built = true;
    }

    [[nodiscard]] int top(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return lead[u];
    }

    [[nodiscard]] int input(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return inp[u];
    }

    [[nodiscard]] int output(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return out[u];
    }

    [[nodiscard]] int tour(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return ord[u];
    }

    [[nodiscard]] int lca(int u, int v) {
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

    [[nodiscard]] int dist(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        return dep[u] + dep[v] - 2 * dep[lca(u, v)];
    }

    [[nodiscard]] int jump(int u, int k) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        if (k > dep[u]) return -1;
        int d = dep[u] - k;
        while (dep[lead[u]] > d) {
            u = par[lead[u]];
        }
        return ord[inp[u] - dep[u] + d];
    }

    [[nodiscard]] bool is_ancestor(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        return inp[u] <= inp[v] && inp[v] < out[u];
    }

    [[nodiscard]] int rooted_parent(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        std::swap(u, v);
        if (u == v) return u;
        if (!is_ancestor(u, v)) return par[u];
        return jump(v, dep[v] - dep[u] - 1);
    }

    [[nodiscard]] int root_size(int u, int v) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();
        if (u == v) return num_vertices();
        if (!is_ancestor(v, u)) return siz[v];
        return num_vertices() - siz[rooted_parent(u, v)];
    }

    [[nodiscard]] int rooted_lca(int a, int b, int c) {
        assert(0 <= a && a < num_vertices());
        assert(0 <= b && b < num_vertices());
        assert(0 <= c && c < num_vertices());
        if (!built) build();
        return lca(a, b) ^ lca(b, c) ^ lca(c, a);
    }

    template <typename SegTree, typename Tag>
    void range_apply(SegTree& d, int u, int v, const Tag& t, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();

        while (lead[u] != lead[v]) {
            if (dep[lead[u]] < dep[lead[v]]) {
                std::swap(u, v);
            }
            d.range_apply(inp[lead[u]], inp[u] + 1, t);
            u = par[lead[u]];
        }
        if (dep[u] > dep[v]) {
            std::swap(u, v);
        }
        if (inp[u] + (is_edge ? 1 : 0) <= inp[v]) {
            d.range_apply(inp[u] + (is_edge ? 1 : 0), inp[v] + 1, t);
        }
    }

    template <typename F>
    void for_each_path(int u, int v, F&& f, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();

        while (lead[u] != lead[v]) {
            if (dep[lead[u]] < dep[lead[v]]) {
                std::swap(u, v);
            }
            f(inp[lead[u]], inp[u] + 1);
            u = par[lead[u]];
        }
        if (dep[u] > dep[v]) {
            std::swap(u, v);
        }
        if (inp[u] + (is_edge ? 1 : 0) <= inp[v]) {
            f(inp[u] + (is_edge ? 1 : 0), inp[v] + 1);
        }
    }

    template <typename F>
    void for_each_subtree(int u, F&& f, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        int l = inp[u] + (is_edge ? 1 : 0);
        int r = out[u];
        if (l < r) {
            f(l, r);
        }
    }

    template <typename SegTree>
    auto range_query(SegTree& seg, int u, int v, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        assert(0 <= v && v < num_vertices());
        if (!built) build();

        auto res = seg.e();

        while (lead[u] != lead[v]) {
            if (dep[lead[u]] < dep[lead[v]]) {
                std::swap(u, v);
            }
            res = seg.op(res, seg.range_query(inp[lead[u]], inp[u] + 1));
            u = par[lead[u]];
        }
        if (dep[u] > dep[v]) {
            std::swap(u, v);
        }
        if (inp[u] + (is_edge ? 1 : 0) <= inp[v]) {
            res = seg.op(res, seg.range_query(inp[u] + (is_edge ? 1 : 0), inp[v] + 1));
        }
        return res;
    }

    template <typename SegTree, typename Tag>
    void subtree_apply(SegTree& seg, int u, const Tag& t, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        int l = inp[u] + (is_edge ? 1 : 0);
        int r = out[u];
        if (l < r) {
            seg.range_apply(l, r, t);
        }
    }

    template <typename SegTree>
    auto subtree_query(SegTree& seg, int u, bool is_edge = false) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        int l = inp[u] + (is_edge ? 1 : 0);
        int r = out[u];
        return (l < r) ? seg.range_query(l, r) : seg.e();
    }

private:
    using graph<edge_type>::n;
    using tree<edge_type>::siz;
    using tree<edge_type>::par;
    using tree<edge_type>::dep;
    using tree<edge_type>::head;
    using tree<edge_type>::root;
    using tree<edge_type>::built;
    using tree<edge_type>::edge_list;

    std::vector<int> inp;
    std::vector<int> out;
    std::vector<int> ord;
    std::vector<int> lead;
    std::vector<int> heavy;
    int timer = 0;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_TREE_HLD