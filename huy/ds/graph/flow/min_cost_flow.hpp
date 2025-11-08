#include <queue>
#include <cassert>
#include "../graph.hpp"

#ifndef HUY_DS_GRAPH_FLOW_MIN_COST_FLOW
#define HUY_DS_GRAPH_FLOW_MIN_COST_FLOW
namespace huy {
namespace ds {

template <typename Edge>
class min_cost_flow : public graph<Edge> {
public:
    using edge_type = Edge;
    using value_type1 = typename edge_type::value_type1;
    using value_type2 = typename edge_type::value_type2;
    using graph<edge_type>::init;
    using graph<edge_type>::build;
    using graph<edge_type>::V;
    using graph<edge_type>::E;
    using graph<edge_type>::edge_at;
    using graph<edge_type>::index;

    min_cost_flow(int n = 0, int m = 0) {
        init(n, m);
    }

    void add_edge(edge_type e) {
        graph<edge_type>::add_edge(std::move(e));
        std::swap(e.from, e.to);
        e.cap  = 0;
        e.cost = -e.cost;
        graph<edge_type>::add_edge(std::move(e));
    }

    void read_edge(int m, int off = 1) {
        for (int i = 0; i < m; ++i) {
            edge_type e;
            std::cin >> e;
            e.from -= off;
            e.to   -= off;
            add_edge(std::move(e));
        }
        build();
    }

    std::pair<value_type1, value_type2> flow(int s, int t, value_type1 maxf = edge_type::inf1) {
        assert(0 <= s && s < V());
        assert(0 <= t && t < V());
        build();
        int n = V();
        value_type1 flow = 0;
        value_type2 cost = 0;
        std::vector<value_type2> pot(n, edge_type::inf2), dis(n);
        std::vector<int> pre_v(n, -1), pre_e(n, -1);
        pot[s] = 0;
        for (int k = 0; k < n - 1; ++k) {
            bool changed = false;
            for (int i = 0; i < E(); ++i) {
                auto& e = *edge_at(i);
                if (e.cap > e.flow && pot[e.from] < edge_type::inf2) {
                    value_type2 nd = pot[e.from] + e.cost;
                    if (pot[e.to] > nd) {
                        pot[e.to] = nd;
                        changed = true;
                    }
                }
            }
            if (!changed) break;
        }
        while (flow < maxf) {
            std::fill(dis.begin(), dis.end(), edge_type::inf2);
            dis[s] = 0;
            using pii = std::pair<value_type2, int>;
            std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
            pq.emplace(0, s);
            while (!pq.empty()) {
                value_type2 d;
                int u;
                std::tie(d, u) = pq.top();
                pq.pop();
                if (d != dis[u]) continue;
                for (auto it = (*this)[u].begin(); it != (*this)[u].end(); ++it) {
                    auto& e = *it;
                    if (e.cap > e.flow) {
                        int v = e.to;
                        value_type2 nd = d + e.cost + pot[u] - pot[v];
                        if (dis[v] > nd) {
                            dis[v] = nd;
                            pre_v[v] = u;
                            pre_e[v] = index(e);
                            pq.emplace(nd, v);
                        }
                    }
                }
            }
            if (dis[t] == edge_type::inf2) break;
            for (int v = 0; v < n; ++v) {
                if (dis[v] < edge_type::inf2) pot[v] += dis[v];
            }
            value_type1 addf = maxf - flow;
            for (int v = t; v != s; v = pre_v[v]) {
                auto& e = *edge_at(pre_e[v]);
                addf = std::min(addf, e.cap - e.flow);
            }
            for (int v = t; v != s; v = pre_v[v]) {
                auto& e = *edge_at(pre_e[v]);
                e.flow += addf;
                auto& rev = *edge_at(pre_e[v] ^ 1);
                rev.flow -= addf;
            }
            flow += addf;
            cost += addf * pot[t];
        }
        return { flow, cost };
    }

    std::vector<edge_type> get_edges() const {
        std::vector<edge_type> res;
        res.reserve(E() >> 1);
        for (int i = 0; i < E(); i += 2) {
            const auto& e = *edge_at(i);
            res.push_back(e);
        }
        return res;
    }
};

} // namespace ds
} // namespace huy
#endif