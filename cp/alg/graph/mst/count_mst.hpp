#include "../../../ds/edge/weight.hpp"
#include "../../../ds/tree/dsu/dsu.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_COUNT_MST
#define CP_ALG_GRAPH_COUNT_MST

namespace cp {
namespace alg {

template <typename Edge, typename ModType = long long>
ModType count_mst(const ds::graph<Edge>& g, ModType mod = 1'000'000'007) {
    using edge_type = Edge;
    int n = g.num_vertices();
    if (n <= 1) return 1 % mod;

    std::vector<edge_type> all_edges = g.get_edges();
    std::vector<edge_type> edges;
    edges.reserve(all_edges.size());

    for (const auto& e : all_edges) {
        if (e.from < e.to) {
            edges.push_back(e);
        }
    }

    if (edges.empty() && !all_edges.empty()) {
        edges = std::move(all_edges);
    }

    std::sort(edges.begin(), edges.end(), [](const edge_type& a, const edge_type& b) {
        return a.weight < b.weight;
    });

    ds::dsu d(n);
    ModType total_ways = 1;
    int total_edges_used = 0;

    auto power = [&](ModType a, ModType b) {
        ModType res = 1;
        a %= mod;
        while (b > 0) {
            if (b & 1) res = (res * a) % mod;
            a = (a * a) % mod;
            b >>= 1;
        }
        return res;
    };

    auto inv = [&](ModType a) {
        return power(a, mod - 2);
    };

    auto determinant = [&](std::vector<std::vector<ModType>>& mat) -> ModType {
        int m = static_cast<int>(mat.size());
        ModType det = 1;
        for (int i = 0; i < m; ++i) {
            int pivot = i;
            while (pivot < m && mat[pivot][i] == 0) ++pivot;
            if (pivot == m) return 0;
            if (pivot != i) {
                std::swap(mat[i], mat[pivot]);
                det = (mod - det) % mod;
            }
            det = (det * mat[i][i]) % mod;
            ModType inv_diag = inv(mat[i][i]);
            for (int r = i + 1; r < m; ++r) {
                if (mat[r][i] == 0) continue;
                ModType factor = (mat[r][i] * inv_diag) % mod;
                for (int c = i; c < m; ++c) {
                    mat[r][c] = (mat[r][c] - factor * mat[i][c]) % mod;
                    if (mat[r][c] < 0) mat[r][c] += mod;
                }
            }
        }
        return (det % mod + mod) % mod;
    };

    size_t i = 0;
    while (i < edges.size()) {
        size_t j = i;
        while (j < edges.size() && edges[j].weight == edges[i].weight) ++j;

        std::vector<int> nodes;
        for (size_t k = i; k < j; ++k) {
            int u = d.leader(edges[k].from);
            int v = d.leader(edges[k].to);
            if (u != v) {
                nodes.push_back(u);
                nodes.push_back(v);
            }
        }

        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
        int k_nodes = static_cast<int>(nodes.size());

        if (k_nodes > 0) {
            std::vector<int> id_map(n, -1);
            for (int idx = 0; idx < k_nodes; ++idx) {
                id_map[nodes[idx]] = idx;
            }

            ds::dsu sub_d(k_nodes);
            for (size_t k = i; k < j; ++k) {
                int u = d.leader(edges[k].from);
                int v = d.leader(edges[k].to);
                if (u != v) {
                    sub_d.merge(id_map[u], id_map[v]);
                }
            }

            auto sub_groups = sub_d.groups();
            for (const auto& group : sub_groups) {
                int sz = static_cast<int>(group.size());
                if (sz <= 1) continue;

                std::vector<int> local_id(k_nodes, -1);
                for (int idx = 0; idx < sz; ++idx) {
                    local_id[group[idx]] = idx;
                }

                std::vector<std::vector<ModType>> lap(sz - 1, std::vector<ModType>(sz - 1, 0));
                for (size_t k = i; k < j; ++k) {
                    int u = d.leader(edges[k].from);
                    int v = d.leader(edges[k].to);
                    if (u != v) {
                        int lu = local_id[id_map[u]];
                        int lv = local_id[id_map[v]];
                        if (lu != -1 && lv != -1) {
                            if (lu < sz - 1 && lv < sz - 1) {
                                lap[lu][lv] = (lap[lu][lv] - 1 + mod) % mod;
                                lap[lv][lu] = (lap[lv][lu] - 1 + mod) % mod;
                            }
                            if (lu < sz - 1) lap[lu][lu] = (lap[lu][lu] + 1) % mod;
                            if (lv < sz - 1) lap[lv][lv] = (lap[lv][lv] + 1) % mod;
                        }
                    }
                }

                ModType ways = determinant(lap);
                total_ways = (total_ways * ways) % mod;
            }

            for (size_t k = i; k < j; ++k) {
                if (d.merge(edges[k].from, edges[k].to)) {
                    ++total_edges_used;
                }
            }
        }

        i = j;
    }

    if (total_edges_used < n - 1) return 0;
    return total_ways;
}

} // namespace alg
} // namespace cp

#endif // CP_ALG_GRAPH_COUNT_MST
