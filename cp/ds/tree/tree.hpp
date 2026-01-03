#include "../edge/edge.hpp"
#include "../graph/graph.hpp"

#ifndef CP_DS_GRAPH_TREE
#define CP_DS_GRAPH_TREE
namespace cp {
namespace ds {

template <typename Edge>
class tree : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::V;

    tree(int n = 0) {
        init(n);
    }

    tree(const graph<edge_type>& g, int u = 0) {
        init(g, u);
    }

    void init(int n) {
        graph<edge_type>::init(n);
        par.assign(n, -1);
        dep.assign(n, 0);
        siz.assign(n, 0);
        built = false;
    }

    void init(const graph<edge_type>& g, int u = 0) {
        assert(0 <= u && u < g.num_vertices());
        init(g.num_vertices());
        for (const auto& e : g.get_edges()) {
            if (e.from < e.to) {
                add_edge(e);
            }
        }
        build(u);
    }

    void build(int u = 0) {
        assert(0 <= u && u < num_vertices());
        if (built) return;
        graph<edge_type>::build();
        dfs(u);
        built = true;
    }

    void add_edge(const edge_type& e) {
        assert(0 <= e.from && e.from < num_vertices());
        assert(0 <= e.to && e.to < num_vertices());
        graph<edge_type>::add_unedge(e);
        built = false;
    }

    void read_edge(int off = 1) {
        for (int i = 1; i < num_vertices(); ++i) {
            edge_type e;
            std::cin >> e;
            e.from -= off;
            e.to   -= off;
            add_edge(std::move(e));
        }
        build();
    }

    int parent(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return par[u];
    }

    int depth(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return dep[u];
    }

    int subsize(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return siz[u];
    }

protected:
    using graph<edge_type>::built;

    std::vector<int> par;
    std::vector<int> dep;
    std::vector<int> siz;

    void add_unedge(edge_type e) = delete;

    void read_unedge(int m, int off = 1) = delete;

    void dfs(int u) {
        siz[u] = 1;
        for (auto e : (*this)[u]) {
            int v = e.to;
            if (v == par[u]) {
                continue;
            }
            dep[v] = dep[u] + 1;
            par[v] = u;
            dfs(v);
            siz[u] += siz[v];
        }
    }
};
    
} // namespace ds
} // namespace cp
#endif