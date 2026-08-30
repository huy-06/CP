#include "../base/tree.hpp"

#ifndef CP_DS_TREE_CENTROID
#define CP_DS_TREE_CENTROID
namespace cp {
namespace ds {

template <typename Edge>
class centroid : public tree<Edge> {
public:
    using edge_type = Edge;
    using tree<edge_type>::num_vertices;

    centroid(int n = 0) {
        init(n);
    }

    centroid(const graph<edge_type>& g) {
        init(g);
    }

    centroid(const tree<edge_type>& t) {
        init(t);
    }

    void init(int n, int root = 0) {
        tree<edge_type>::init(n, root);
        removed.assign(n, false);
        built = false;
    }

    void init(const graph<edge_type>& g, int root = 0) {
        assert(0 <= root && root < g.num_vertices());
        init(g.num_vertices());
        for (const auto& e : g.get_edges()) {
            if (e.from < e.to) {
                this->add_edge(e);
            }
        }
        build(root);
    }

    void init(const tree<edge_type>& t, int root = 0) {
        assert(0 <= root && root < t.num_vertices());
        init(t.num_vertices());
        for (const auto& e : t.get_edges()) {
            if (e.from < e.to) {
                this->add_edge(e);
            }
        }
        build(root);
    }

    void build() override {
        if (built) return;
        build(this->root);
    }

    void build(int start_node) override {
        assert(0 <= start_node && start_node < num_vertices());
        if (built) return;

        graph<edge_type>::build(); 

        par.assign(this->n, -1);
        dep.assign(this->n, 0);
        siz.assign(this->n, 0);
        removed.assign(this->n, false);

        this->root = build_centroid_tree(start_node, -1, 0);
        built = true;
    }

    template <class F>
    void run(F&& f, int start_node = 0) {
        if (!built) {
            graph<edge_type>::build();
            siz.assign(this->n, 0);
            built = true;
        }
        std::fill(removed.begin(), removed.end(), false);
        if (this->n > 0) {
            solve_dnc(start_node, f);
        }
    }

    bool is_removed(int u) {
        if (!built) build();
        return removed[u];
    }

private:
    using tree<edge_type>::n;
    using tree<edge_type>::siz;
    using tree<edge_type>::par;
    using tree<edge_type>::dep;
    using tree<edge_type>::built;

    std::vector<bool> removed;

    void get_size(int u, int p) {
        siz[u] = 1;
        for (auto& e : (*this)[u]) {
            int v = e.to;
            if (v != p && !removed[v]) {
                get_size(v, u);
                siz[u] += siz[v];
            }
        }
    }

    int get_centroid(int u, int p, int total) {
        for (auto& e : (*this)[u]) {
            int v = e.to;
            if (v != p && !removed[v] && siz[v] > total / 2) {
                return get_centroid(v, u, total);
            }
        }
        return u;
    }

    int build_centroid_tree(int u, int p, int d) {
        get_size(u, -1);
        int c = get_centroid(u, -1, siz[u]);

        removed[c] = true;
        par[c] = p;
        dep[c] = d;

        for (auto& e : (*this)[c]) {
            int v = e.to;
            if (!removed[v]) {
                build_centroid_tree(v, c, d + 1);
            }
        }
        return c;
    }

    template <class F>
    void solve_dnc(int u, F&& f) {
        get_size(u, -1);
        int c = get_centroid(u, -1, siz[u]);

        removed[c] = true;
        
        f(c);

        for (auto& e : (*this)[c]) {
            int v = e.to;
            if (!removed[v]) {
                solve_dnc(v, f);
            }
        }
    }
};

} // namespace ds
} // namespace cp
#endif