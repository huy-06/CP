#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_HAMILTON
#define CP_ALG_GRAPH_SHORTEST_HAMILTON
namespace cp {
namespace alg {

template<typename Edge>
std::pair<typename Edge::value_type, 
          std::vector<int>> hamilton(const ds::graph<Edge>& g, int src) {
    using Tp = typename Edge::value_type;
    int n = g.num_vertices();
    
    if (src < 0 || src >= n) return { Edge::inf, {} };
    if (n == 0) return { 0, {} };
    if (n == 1) return { 0, {src} };

    int full = (1 << n);
    
    std::vector<std::vector<Tp>>  dis(full, std::vector<Tp>(n, Edge::inf));
    std::vector<std::vector<int>> par(full, std::vector<int>(n, -1));

    dis[1 << src][src] = 0;

    for (int mask = 1; mask < full; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u)) || dis[mask][u] == Edge::inf) {
                continue;
            }

            for (const auto& e : g.adj(u)) {
                int v = e.to;
                Tp  w = e.weight;
                
                if (mask & (1 << v)) {
                    continue;
                }

                int nmask = mask | (1 << v);
                Tp  nd    = dis[mask][u] + w;

                if (dis[nmask][v] > nd) {
                    dis[nmask][v] = nd;
                    par[nmask][v] = u;
                }
            }
        }
    }

    Tp best = Edge::inf;
    int last = -1;
    int full_mask = full - 1;

    for (int u = 0; u < n; ++u) {
        if (dis[full_mask][u] < best) {
            best = dis[full_mask][u];
            last = u;
        }
    }

    std::vector<int> path;
    if (best == Edge::inf) {
        return { best, path };
    }

    path.reserve(n);
    int curr = last;
    int curr_mask = full_mask;

    while (curr != -1) {
        path.push_back(curr);
        int p = par[curr_mask][curr];
        curr_mask ^= (1 << curr);
        curr = p;
    }
    
    std::reverse(path.begin(), path.end());
    
    return { best, path };
}

} // namespace alg
} // namespace cp
#endif