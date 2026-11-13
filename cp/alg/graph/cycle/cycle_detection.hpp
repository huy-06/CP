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
                for (int x = u; x != v; x = par[x]) {
                    cycle.push_back(x);
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
    std::vector<int> par(n, -1), cycle;

    auto dfs = [&](auto&& self, int u, int p) -> bool {
        vis[u] = true;
        int cnt = 0;

        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (v == p && ++cnt == 1) continue;

            if (vis[v]) {
                cycle.push_back(v);
                for (int x = u; x != v; x = par[x]) {
                    cycle.push_back(x);
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
        if (!vis[i] && dfs(dfs, i, -1)) return cycle;
    }
    return {};
}

template <typename Graph>
bool has_cycle_undirected(const Graph& g) {
    return !find_cycle_undirected(g).empty();
}


/**
 * @brief Tính Girth đồ thị VÔ HƯỚNG (chỉ lấy độ dài, tối ưu tốc độ tối đa).
 * @return Độ dài chu trình ngắn nhất, hoặc -1 nếu không có.
 */
template <typename Graph>
int girth_undirected(const Graph& g) {
    int n = g.num_vertices();
    int min_len = 1E9;

    std::vector<int> dis(n, -1), par(n, -1);
    std::vector<int> ord;
    ord.reserve(n);
    std::queue<int> que;

    for (int s = 0; s < n; ++s) {
        if (g.degree(s) <= 1) continue;

        dis[s] = 0;
        que.push(s);
        ord.push_back(s);

        while (!que.empty()) {
            int u = que.front();
            que.pop();

            if (2 * dis[u] + 1 >= min_len) break;

            for (const auto& e : g.adj(u)) {
                int v = e.to;
                if (dis[v] == -1) {
                    dis[v] = dis[u] + 1;
                    par[v] = u;
                    que.push(v);
                    ord.push_back(v);
                } else if (v != par[u]) {
                    min_len = std::min(min_len, dis[u] + dis[v] + 1);
                }
            }
        }

        while (!que.empty()) que.pop();

        for (int node : ord) {
            dis[node] = -1;
            par[node] = -1;
        }
        ord.clear();

        if (min_len == 3) return 3;
    }

    return (min_len == 1E9 ? -1 : min_len);
}

/**
 * @brief Tìm và TRUY VẾT chu trình ngắn nhất đồ thị VÔ HƯỚNG.
 */
template <typename Graph>
std::vector<int> find_shortest_cycle_undirected(const Graph& g) {
    int n = g.num_vertices();
    int min_len = 1E9;
    std::vector<int> ans;

    std::vector<int> dis(n, -1), par(n, -1);
    std::vector<int> ord;
    ord.reserve(n);
    std::queue<int> que;

    for (int s = 0; s < n; ++s) {
        if (g.degree(s) <= 1) continue;

        dis[s] = 0;
        que.push(s);
        ord.push_back(s);

        while (!que.empty()) {
            int u = que.front();
            que.pop();

            if (2 * dis[u] + 1 >= min_len) break;

            for (const auto& e : g.adj(u)) {
                int v = e.to;
                if (dis[v] == -1) {
                    dis[v] = dis[u] + 1;
                    par[v] = u;
                    que.push(v);
                    ord.push_back(v);
                } else if (v != par[u]) {
                    int len = dis[u] + dis[v] + 1;
                    if (len < min_len) {
                        min_len = len;

                        std::vector<bool> in_path(n, false);
                        for (int x = u; x != -1; x = par[x]) {
                            in_path[x] = true;
                        }

                        int lca = v;
                        while (!in_path[lca]) {
                            lca = par[lca];
                        }

                        std::vector<int> p1, p2;
                        for (int x = u; x != lca; x = par[x]) {
                            p1.push_back(x);
                        }
                        p1.push_back(lca);
                        std::reverse(p1.begin(), p1.end());
                        for (int x = v; x != lca; x = par[x]) {
                            p2.push_back(x);
                        }

                        ans.clear();
                        ans.insert(ans.end(), p1.begin(), p1.end());
                        ans.insert(ans.end(), p2.begin(), p2.end());
                        ans.push_back(lca);
                    }
                }
            }
        }

        while (!que.empty()) que.pop();
        for (int node : ord) {
            dis[node] = -1;
            par[node] = -1;
        }
        ord.clear();

        if (min_len == 3) return ans;
    }

    return ans;
}

// 3. CHU TRÌNH EULER 
template <typename Graph>
std::vector<int> find_eulerian_circuit_directed(const Graph& g) {
    int n = g.num_vertices();
    int m = g.num_edges();
    if (m == 0) return {};

    int s = -1;
    for (int u = 0; u < n; ++u) {
        if (g.indeg(u) != g.outdeg(u)) return {};
        if (g.outdeg(u) > 0 && s == -1) {
            s = u;
        }
    }
    if (s == -1) return {};

    std::vector<int> idx(n, 0);
    std::vector<int> stk = {s}, circuit;

    while (!stk.empty()) {
        int u = stk.back();
        if (idx[u] < g.outdeg(u)) {
            int v = g.adj(u)[idx[u]++].to;
            stk.push_back(v);
        } else {
            circuit.push_back(u);
            stk.pop_back();
        }
    }

    if (static_cast<int>(circuit.size()) != m + 1) return {};
    std::reverse(circuit.begin(), circuit.end());
    return circuit;
}

// CHU TRÌNH TRONG ĐỒ THỊ HÀM (FUNCTIONAL GRAPH) 
inline std::vector<std::vector<int>> find_cycles_functional_graph(const std::vector<int>& f) {
    int n = static_cast<int>(f.size());
    std::vector<int> state(n, 0);
    std::vector<std::vector<int>> all_cycles;

    for (int i = 0; i < n; ++i) {
        if (state[i] != 0) continue;
        int u = i;
        while (state[u] == 0) {
            state[u] = 1;
            u = f[u];
        }
        if (state[u] == 1) {
            std::vector<int> cycle;
            int node = u;
            do {
                cycle.push_back(node);
                node = f[node];
            } while (node != u);
            all_cycles.push_back(cycle);
        }
        u = i;
        while (state[u] == 1) {
            state[u] = 2;
            u = f[u];
        }
    }
    return all_cycles;
}

} // namespace alg
} // namespace cp

#endif // CP_GRAPH_CYCLE_DETECTION