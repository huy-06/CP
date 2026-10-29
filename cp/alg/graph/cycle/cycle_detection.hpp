#include "../../../ds/graph/graph.hpp"

#ifndef CP_GRAPH_CYCLE_DETECTION
#define CP_GRAPH_CYCLE_DETECTION

namespace cp {
namespace alg {

/**
 * @brief Tìm chu trình trong đồ thị CÓ HƯỚNG bằng DFS.
 * @return std::vector<int> chứa chu trình (u1 -> u2 -> ... -> u1).
 *         Trả về vector rỗng nếu đồ thị là DAG (không có chu trình).
 */
template <typename Graph>
std::vector<int> find_cycle_directed(const Graph& g) {
    int n = g.num_vertices();

    std::vector<int> col(n, 0);
    std::vector<int> par(n, -1);
    std::vector<int> cycle;

    auto dfs = [&](auto& self, int u) -> bool {
        col[u] = 1;
        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (col[v] == 1) {
                cycle.push_back(v);
                for (int cur = u; cur != v; cur = par[cur]) {
                    cycle.push_back(cur);
                }
                cycle.push_back(v);
                std::reverse(cycle.begin(), cycle.end());
                return true;
            }
            if (col[v] == 0) {
                par[v] = u;
                if (self(self, v)) return true;
            }
        }
        col[u] = 2;
        return false;
    };

    for (int i = 0; i < n; ++i) {
        if (col[i] == 0) {
            if (dfs(dfs, i)) return cycle;
        }
    }
    return {};
}

template <typename Graph>
bool has_cycle_directed(const Graph& g) {
    return !find_cycle_directed(g).empty();
}

/**
 * @brief Tìm chu trình trong đồ thị VÔ HƯỚNG (bỏ qua cạnh đi ngược lại đỉnh cha trực tiếp).
 * @return std::vector<int> chu trình (u1 -> u2 -> ... -> u1),
 *         hoặc vector rỗng nếu đồ thị là Rừng cây (Forest/Tree).
 */
template <typename Graph>
std::vector<int> find_cycle_undirected(const Graph& g) {
    int n = g.num_vertices();

    std::vector<bool> vis(n, false);
    std::vector<int> par(n, -1);
    std::vector<int> cycle;

    auto dfs = [&](auto& self, int u, int p) -> bool {
        vis[u] = true;
        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (v == p) continue;

            if (vis[v]) {
                cycle.push_back(v);
                for (int cur = u; cur != v; cur = par[cur]) {
                    cycle.push_back(cur);
                }
                cycle.push_back(v);
                std::reverse(cycle.begin(), cycle.end());
                return true;
            }

            par[v] = u;
            if (self(self, v, u)) return true;
        }
        return false;
    };

    for (int i = 0; i < n; ++i) {
        if (!vis[i]) {
            if (dfs(dfs, i, -1)) return cycle;
        }
    }
    return {};
}

template <typename Graph>
bool has_cycle_undirected(const Graph& g) {
    return !find_cycle_undirected(g).empty();
}

} // namespace alg
} // namespace cp

#endif // CP_GRAPH_CYCLE_DETECTION