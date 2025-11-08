#include "../../../ds/graph/graph.hpp"

#ifndef HUY_ALG_SP_GRAPH_SPFA
#define HUY_ALG_SP_GRAPH_SPFA
namespace huy {
namespace alg {

template <typename Edge>
std::pair<std::vector<typename Edge::value_type>,
          std::vector<int>> spfa(const ds::graph<Edge>& g, int src) {
    using Tp = typename Edge::value_type;
    int n = g.V();
    std::vector<Tp>   dis(n, Edge::inf);
    std::vector<int>  par(n, -1);
    std::vector<bool> inq(n, false);
    std::deque<int> dq;
    dis[src] = 0;
    inq[src] = true;
    dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        inq[u] = false;
        for (auto& e : g[u]) {
            int v = e.to;
            if (dis[v] > dis[u] + e.weight) {
                dis[v] = dis[u] + e.weight;
                par[v] = u;
                if (!inq[v]) {
                    inq[v] = true;
                    if (e.weight == 0) {
                        dq.push_front(v);
                    } else {
                        dq.push_back(v);
                    }
                }
            }
        }
    }
    return { dis, par };
}

} // namespace alg
} // namespace huy
#endif