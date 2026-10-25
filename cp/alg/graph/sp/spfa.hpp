#include <deque>
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_SPFA
#define CP_ALG_GRAPH_SHORTEST_PATH_SPFA

namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<typename Edge::value_type>, std::vector<int>> spfa(const ds::graph<Edge>& g, int src) {
    using Tp = typename Edge::value_type;
    int n = g.num_vertices();

    std::vector<int> par(n, -1);
    std::vector<int> cnt(n, 0);
    std::vector<Tp> dis(n, Edge::inf);
    dis[src] = 0;

    std::vector<bool> inq(n, false);
    std::deque<int> dq;

    dq.push_back(src);
    inq[src] = true;

    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        inq[u] = false;

        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (dis[u] < Edge::inf && dis[v] > dis[u] + e.weight) {
                dis[v] = dis[u] + e.weight;
                par[v] = u;
                if (!inq[v]) {
                    inq[v] = true;
                    ++cnt[v];
                    if (cnt[v] >= n) {
                        return { {}, {} };
                    }
                    if (!dq.empty() && dis[v] < dis[dq.front()]) {
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
} // namespace cp

#endif // CP_ALG_GRAPH_SHORTEST_PATH_SPFA