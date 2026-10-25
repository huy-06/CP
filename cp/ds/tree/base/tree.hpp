#include "../../graph/graph.hpp"

#ifndef CP_DS_GRAPH_TREE
#define CP_DS_GRAPH_TREE

namespace cp {
namespace ds {

template <typename Edge>
class tree : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::adj;

    explicit tree(int n = 0, int root = 0) : root(root) {
        init(n, root);
    }

    explicit tree(const graph<edge_type>& g, int root = 0) {
        init(g, root);
    }

    void init(int n, int root = 0) {
        graph<edge_type>::init(n, std::max(0, n - 1));
        this->root = root;
        par.assign(n, -1);
        dep.assign(n, 0);
        siz.assign(n, 0);
        ord.clear();
        built = false;
    }

    void init(const graph<edge_type>& g, int root = 0) {
        assert(0 <= root && root < g.num_vertices());
        init(g.num_vertices(), root);
        for (const auto& e : g.get_edges()) {
            if (e.from < e.to) {
                add_edge(e);
            }
        }
        build(root);
    }

    void build() override {
        if (built) return;
        build(root);
    }

    virtual void build(int root) {
        assert(0 <= root && root < num_vertices());
        if (built && this->root == root) return;

        this->root = root;
        graph<edge_type>::build();

        par.assign(n, -1);
        dep.assign(n, 0);
        siz.assign(n, 1);
        ord.clear();
        ord.reserve(n);

        if (n == 0) {
            built = true;
            return;
        }

        ord.push_back(root);
        for (int i = 0; i < static_cast<int>(ord.size()); ++i) {
            int u = ord[i];
            for (const auto& e : adj(u)) {
                int v = e.to;
                if (v == par[u]) continue;
                par[v] = u;
                dep[v] = dep[u] + 1;
                ord.push_back(v);
            }
        }

        for (int i = static_cast<int>(ord.size()) - 1; i > 0; --i) {
            int u = ord[i];
            siz[par[u]] += siz[u];
        }

        built = true;
    }

    void add_edge(const edge_type& e) override {
        assert(0 <= e.from && e.from < num_vertices());
        assert(0 <= e.to && e.to < num_vertices());
        graph<edge_type>::add_unedge(e);
        built = false;
    }

    void read_edges(int off = 1, std::istream& is = std::cin) {
        for (int i = 1; i < num_vertices(); ++i) {
            edge_type e;
            is >> e;
            e.from -= off;
            e.to   -= off;
            add_edge(std::move(e));
        }
        build();
    }

    [[nodiscard]] int get_root() {
        if (!built) build();
        return root;
    }

    virtual void set_root(int new_root) {
        assert(0 <= new_root && new_root < num_vertices());
        if (built && this->root == new_root) return;
        build(new_root);
    }

    [[nodiscard]] int parent(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return par[u];
    }

    [[nodiscard]] int depth(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return dep[u];
    }

    [[nodiscard]] int subtree(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return siz[u];
    }

    [[nodiscard]] bool is_leaf(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return siz[u] == 1;
    }

    /// @brief Thứ tự BFS (Top-Down, từ gốc xuống lá).
    [[nodiscard]] const std::vector<int>& bfs_order() {
        if (!built) build();
        return ord;
    }

    [[nodiscard]] const std::vector<int>& top_down() {
        return bfs_order();
    }

    /// @brief Thứ tự ngược BFS (Bottom-Up, từ lá lên gốc).
    [[nodiscard]] std::vector<int> bottom_up() {
        if (!built) build();
        return std::vector<int>(ord.rbegin(), ord.rend());
    }

    [[nodiscard]] std::vector<int> reverse_bfs_order() {
        return bottom_up();
    }

    /// @brief Thứ tự DFS bắt đầu từ root
    [[nodiscard]] std::vector<int> dfs_order() {
        if (!built) build();
        return graph<edge_type>::dfs_order(root);
    }

    [[nodiscard]] std::vector<int> preorder() {
        return dfs_order();
    }

    std::pair<int, std::vector<int>> diameter() {
        if (!built) build();
        if (n <= 1) return {0, n == 1 ? std::vector<int>{0} : std::vector<int>{}};

        auto bfs_farthest = [&](int src) {
            std::vector<int> d(n, -1), p(n, -1);
            std::vector<int> q;
            q.reserve(n);
            q.push_back(src);
            d[src] = 0;

            for (size_t i = 0; i < q.size(); ++i) {
                int u = q[i];
                for (const auto& e : adj(u)) {
                    int v = e.to;
                    if (d[v] == -1) {
                        d[v] = d[u] + 1;
                        p[v] = u;
                        q.push_back(v);
                    }
                }
            }
            int far = q.back();
            return std::tuple{far, d[far], p};
        };

        auto [u, d1, p1] = bfs_farthest(root);
        auto [v, diam, p2] = bfs_farthest(u);

        std::vector<int> path;
        for (int cur = v; cur != -1; cur = p2[cur]) {
            path.push_back(cur);
        }
        std::reverse(path.begin(), path.end());
        return {diam, path};
    }

protected:
    using graph<edge_type>::n;
    using graph<edge_type>::built;

    int root = 0;
    std::vector<int> par;
    std::vector<int> dep;
    std::vector<int> siz;
    std::vector<int> ord;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_TREE