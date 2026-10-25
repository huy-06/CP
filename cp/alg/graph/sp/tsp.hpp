#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_TSP
#define CP_ALG_GRAPH_SHORTEST_PATH_TSP

namespace cp {
namespace alg {

template<typename Edge>
std::pair<typename Edge::value_type, std::vector<int>> tsp(const ds::graph<Edge>& g) {
    using Tp = typename Edge::value_type;

    int n = g.num_vertices();
    if (n <= 1) return {0, n == 1 ? std::vector<int>{0, 0} : std::vector<int>{}};
    int full = 1 << n;

    std::vector<std::vector<Tp>>  dis(full, std::vector<Tp>(n, Edge::inf));
    std::vector<std::vector<int>> par(full, std::vector<int>(n, -1));
    dis[1][0] = 0;

    for (int mask = 1; mask < full; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u)) || dis[mask][u] == Edge::inf) continue;

            for (const auto& e : g.adj(u)) {
                int v = e.to;
                if (mask & (1 << v)) continue;

                int nmask = mask | (1 << v);
                Tp  nd    = dis[mask][u] + e.weight;

                if (dis[nmask][v] > nd) {
                    dis[nmask][v] = nd;
                    par[nmask][v] = u;
                }
            }
        }
    }

    Tp best = Edge::inf;
    int last = -1;

    for (int u = 1; u < n; ++u) {
        if (dis[full - 1][u] == Edge::inf) continue;
        for (const auto& e : g.adj(u)) {
            if (e.to == 0) {
                Tp total = dis[full - 1][u] + e.weight;
                if (best > total) {
                    best = total;
                    last = u;
                }
            }
        }
    }

    std::vector<int> path;
    if (best == Edge::inf) {
        return { best, path };
    }

    path.reserve(n + 1);
    for (int mask = full - 1; last != -1;) {
        path.push_back(last);
        int p = par[mask][last];
        mask ^= (1 << last);
        last = p;
    }
    path.push_back(0);
    std::reverse(path.begin(), path.end());
    path.push_back(0);

    return { best, path };
}

} // namespace alg
} // namespace cp

#endif // CP_ALG_GRAPH_SHORTEST_PATH_TSP