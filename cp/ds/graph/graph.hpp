#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <span>
#include "../edge/edge.hpp"

#ifndef CP_DS_GRAPH
#define CP_DS_GRAPH

namespace cp {
namespace ds {

template <typename Edge>
class graph {
public:
    using edge_type       = Edge;
    using neighbors       = std::span<edge_type>;
    using const_neighbors = std::span<const edge_type>;

    explicit graph(int n = 0, int m = 0) {
        init(n, m);
    }

    explicit graph(const std::vector<edge_type>& edges) {
        init(edges);
    }

    virtual ~graph() = default;

    void init(int n = 0, int m = 0) {
        this->n = n;
        built = false;
        head.assign(n + 1, 0);
        indeg_cnt.assign(n, 0);
        edge_list.clear();
        edge_list.reserve(m);
    }

    void init(const std::vector<edge_type>& edges) {
        int max_v = -1;
        for (const auto& e : edges) {
            max_v = std::max({max_v, e.from, e.to});
        }
        init(max_v + 1, static_cast<int>(edges.size()));
        for (const auto& e : edges) {
            add_edge(e);
        }
        build();
    }

    void init(const std::vector<std::vector<edge_type>>& adj_list) {
        int m = 0;
        for (const auto& nbrs : adj_list) {
            m += static_cast<int>(nbrs.size());
        }
        init(static_cast<int>(adj_list.size()), m);
        for (int u = 0; u < static_cast<int>(adj_list.size()); ++u) {
            for (const auto& e : adj_list[u]) {
                edge_type new_e = e;
                new_e.from = u;
                add_edge(std::move(new_e));
            }
        }
        build();
    }

    virtual void build() {
        if (built) return;

        head.assign(n + 1, 0);
        indeg_cnt.assign(n, 0);

        for (const auto& e : edge_list) {
            assert(0 <= e.from && e.from < n);
            assert(0 <= e.to && e.to < n);
            ++head[e.from + 1];
            ++indeg_cnt[e.to];
        }

        for (int i = 1; i <= n; ++i) {
            head[i] += head[i - 1];
        }

        std::vector<int> cur = head;
        std::vector<edge_type> sorted(edge_list.size());

        for (const auto& e : edge_list) {
            sorted[cur[e.from]++] = e;
        }

        edge_list = std::move(sorted);
        built = true;
    }

    virtual void add_edge(const edge_type& e) {
        assert(0 <= e.from && e.from < num_vertices());
        assert(0 <= e.to && e.to < num_vertices());
        edge_list.push_back(e);
        built = false;
    }

    virtual void add_unedge(const edge_type& e) {
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        graph::add_edge(e);
        graph::add_edge(rev_e);
    }

    void read_edges(int m, int off = 1, std::istream& is = std::cin) {
        edge_list.reserve(edge_list.size() + m);
        for (int i = 0; i < m; ++i) {
            edge_type e;
            is >> e;
            e.from -= off;
            e.to   -= off;
            add_edge(std::move(e));
        }
        build();
    }

    void read_unedges(int m, int off = 1, std::istream& is = std::cin) {
        edge_list.reserve(edge_list.size() + 2 * m);
        for (int i = 0; i < m; ++i) {
            edge_type e;
            is >> e;
            e.from -= off;
            e.to   -= off;
            add_unedge(std::move(e));
        }
        build();
    }

    [[nodiscard]] constexpr int size() const noexcept {
        return n;
    }

    [[nodiscard]] constexpr int num_vertices() const noexcept {
        return n;
    }

    [[nodiscard]] int num_edges() const noexcept {
        return static_cast<int>(edge_list.size());
    }

    int indeg(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return indeg_cnt[u];
    }

    int outdeg(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return head[u + 1] - head[u];
    }

    int degree(int u) const {
        return outdeg(u);
    }

    neighbors adj(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return neighbors(edge_list.data() + head[u], head[u + 1] - head[u]);
    }

    const_neighbors adj(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return const_neighbors(edge_list.data() + head[u], head[u + 1] - head[u]);
    }

    neighbors operator[](int u) {
        return adj(u);
    }

    const_neighbors operator[](int u) const {
        return adj(u);
    }

    [[nodiscard]] virtual std::vector<edge_type> get_edges() const {
        return edge_list;
    }

    void sort(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        std::sort(edge_list.begin() + head[u], edge_list.begin() + head[u + 1],
            [](const edge_type& a, const edge_type& b) {
                return a.to < b.to;
            }
        );
    }

    void sort() {
        if (!built) build();
        for (int u = 0; u < n; ++u) {
            sort(u);
        }
    }

    int index(const edge_type& e) const {   
        assert(!edge_list.empty());
        return static_cast<int>(&e - edge_list.data());
    }

    edge_type* edge_at(int i) {
        assert(0 <= i && i < num_edges());
        return &edge_list[i];
    }

    const edge_type* edge_at(int i) const {
        assert(0 <= i && i < num_edges());
        return &edge_list[i];
    }

    /// @brief Duyệt BFS từ đỉnh src
    std::vector<int> bfs_order(int src = 0) const {
        assert(0 <= src && src < num_vertices());
        if (!built) const_cast<graph*>(this)->build();

        std::vector<int> ord;
        ord.reserve(n);
        std::vector<bool> vis(n, false);

        vis[src] = true;
        ord.push_back(src);

        int head_idx = 0;
        while (head_idx < static_cast<int>(ord.size())) {
            int u = ord[head_idx++];
            for (const auto& e : adj(u)) {
                int v = e.to;
                if (!vis[v]) {
                    vis[v] = true;
                    ord.push_back(v);
                }
            }
        }
        return ord;
    }

    /// @brief Duyệt DFS từ đỉnh src 
    std::vector<int> dfs_order(int src = 0) const {
        assert(0 <= src && src < num_vertices());
        if (!built) const_cast<graph*>(this)->build();

        std::vector<int> ord;
        ord.reserve(n);
        std::vector<bool> vis(n, false);
        std::vector<int> stk;
        stk.reserve(n);

        stk.push_back(src);
        vis[src] = true;

        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();
            ord.push_back(u);

            const_neighbors nbrs = adj(u);
            for (int i = static_cast<int>(nbrs.size()) - 1; i >= 0; --i) {
                int v = nbrs[i].to;
                if (!vis[v]) {
                    vis[v] = true;
                    stk.push_back(v);
                }
            }
        }
        return ord;
    }

    std::vector<int> bfs_order_all() const {
        if (!built) const_cast<graph*>(this)->build();
        std::vector<int> ord;
        ord.reserve(n);
        std::vector<bool> vis(n, false);

        for (int i = 0; i < n; ++i) {
            if (vis[i]) continue;
            vis[i] = true;
            ord.push_back(i);
            int head_idx = static_cast<int>(ord.size()) - 1;
            while (head_idx < static_cast<int>(ord.size())) {
                int u = ord[head_idx++];
                for (const auto& e : adj(u)) {
                    int v = e.to;
                    if (!vis[v]) {
                        vis[v] = true;
                        ord.push_back(v);
                    }
                }
            }
        }
        return ord;
    }

    std::vector<int> dfs_order_all() const {
        if (!built) const_cast<graph*>(this)->build();
        std::vector<int> ord;
        ord.reserve(n);
        std::vector<bool> vis(n, false);
        std::vector<int> stk;
        stk.reserve(n);

        for (int i = 0; i < n; ++i) {
            if (vis[i]) continue;
            stk.push_back(i);
            vis[i] = true;
            while (!stk.empty()) {
                int u = stk.back();
                stk.pop_back();
                ord.push_back(u);
                const_neighbors nbrs = adj(u);
                for (int j = static_cast<int>(nbrs.size()) - 1; j >= 0; --j) {
                    int v = nbrs[j].to;
                    if (!vis[v]) {
                        vis[v] = true;
                        stk.push_back(v);
                    }
                }
            }
        }
        return ord;
    }

protected:
    int n = 0;
    bool built = false;
    std::vector<int> head;
    std::vector<int> indeg_cnt;
    std::vector<edge_type> edge_list;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH