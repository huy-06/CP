#include <queue>
#include "../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_MST
#define CP_ALG_GRAPH_MST
namespace cp { 
namespace alg {

template<typename Edge>
std::pair<typename Edge::value_type,
          ds::graph<Edge>> mst(const ds::graph<Edge>& g, int src = 0) {
    int n = g.V();
    using value_type = typename Edge::value_type;
    using pii = std::pair<value_type, int>;
    std::vector<bool> vis(n, false);
    std::vector<value_type>   dis(n, Edge::inf);
    std::vector<Edge> par(n);
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    value_type total = 0;
    ds::graph<Edge> ans(n, n - 1);
    dis[src] = 0;
    pq.emplace(0, src);
    while (!pq.empty()) {
        value_type d;
        int u;
        std::tie(d, u) = pq.top();
        pq.pop();
        if (vis[u]) continue;
        vis[u] = true;
        total += d;
        if (u != src) ans.add_unedge(par[u]);
        for (const auto& e : g[u]) {
            int v = e.to;
            value_type w = e.weight;
            if (!vis[v] && dis[v] > w) {
                dis[v] = w;
                par[v] = e;
                pq.emplace(w, v);
            }
        }
    }
    for (auto v : vis) {
        assert(v && "Graph is not connected");
    }
    return { total, ans };
}

} // namespace alg
} // namespace cp
#endif