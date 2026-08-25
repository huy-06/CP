#include <queue>
#include "../edge/flow_cost.hpp"
#include "../graph/graph.hpp"

#ifndef CP_DS_FLOW_MIN_COST_FLOW
#define CP_DS_FLOW_MIN_COST_FLOW
namespace cp {
namespace ds {

template <typename Edge>
class min_cost_flow : public graph<Edge> {
public:
    using edge_type   = Edge;
    using value_type1 = typename edge_type::value_type1; // Cap / Flow type
    using value_type2 = typename edge_type::value_type2; // Cost type

    using graph<edge_type>::init;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::num_vertices;

    min_cost_flow(int n = 0, int m = 0) {
        init(n, m);
    }

    void add_edge(edge_type e) {
        graph<edge_type>::add_edge(e);
        std::swap(e.from, e.to);
        e.cap  = 0;
        e.cost = -e.cost;
        graph<edge_type>::add_edge(std::move(e));
    }

    void read_edge(int m, int off = 1, std::istream& is = std::cin) {
        for (int i = 0; i < m; ++i) {
            edge_type e;
            is >> e;
            e.from -= off;
            e.to   -= off;
            add_edge(std::move(e));
        }
        build();
    }

    void build() override {
        if (built) return;

        head.assign(n + 1, 0);
        
        for (const auto& e : edge_list) {
            ++head[e.from + 1];
        }
        for (int i = 1; i <= n; ++i) {
            head[i] += head[i - 1];
        }

        std::vector<int> cur = head;
        std::vector<int> pos(edge_list.size());
        std::vector<edge_type> sorted(edge_list.size());

        for (size_t i = 0; i < edge_list.size(); ++i) {
            pos[i] = cur[edge_list[i].from]++;
            sorted[pos[i]] = edge_list[i];
        }

        fwd_ids.clear();
        fwd_ids.reserve(edge_list.size() / 2);

        for (size_t i = 0; i < edge_list.size(); i += 2) {
            int id_fwd = pos[i];
            int id_rev = pos[i + 1];

            sorted[id_fwd].rev = id_rev;
            sorted[id_rev].rev = id_fwd;

            fwd_ids.push_back(id_fwd);
        }

        edge_list = std::move(sorted);
        built     = true;
    }

    std::vector<edge_type> get_edges() const {
        if (!built) const_cast<min_cost_flow*>(this)->build();
        
        std::vector<edge_type> res;
        res.reserve(fwd_ids.size());
        
        for (int id : fwd_ids) {
            res.push_back(edge_list[id]);
        }
        return res;
    }

    std::pair<value_type1, value_type2> flow(int s, int t, value_type1 maxf = edge_type::inf1) {
        assert(0 <= s && s < num_vertices());
        assert(0 <= t && t < num_vertices());
        
        build();
        
        value_type1 flow = 0;
        value_type2 cost = 0;
        
        std::vector<value_type2> pot(n, edge_type::inf2);
        std::vector<value_type2> dis(n);
        std::vector<int>         pre_v(n, -1);
        std::vector<int>         pre_e(n, -1);
        
        pot[s] = 0;
        for (int k = 0; k < n - 1; ++k) {
            bool changed = false;
            for (const auto& e : edge_list) {
                if (e.cap > e.flow && pot[e.from] < edge_type::inf2) {
                    value_type2 nd = pot[e.from] + e.cost;
                    if (pot[e.to] > nd) {
                        pot[e.to] = nd;
                        changed   = true;
                    }
                }
            }
            if (!changed) break;
        }

        using pii = std::pair<value_type2, int>;
        
        while (flow < maxf) {
            std::fill(dis.begin(), dis.end(), edge_type::inf2);
            std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
            
            dis[s] = 0;
            pq.emplace(0, s);
            
            while (!pq.empty()) {
                value_type2 d; int u;
                std::tie(d, u) = pq.top();
                pq.pop();
                
                if (d != dis[u]) continue;
                
                for (int i = head[u]; i < head[u + 1]; ++i) {
                    const auto& e = edge_list[i];
                    
                    if (e.cap > e.flow) {
                        int v = e.to;
                        value_type2 nd = d + e.cost + pot[u] - pot[v];
                        if (dis[v] > nd) {
                            dis[v]   = nd;
                            pre_v[v] = u;
                            pre_e[v] = i;
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
                const auto& e = edge_list[pre_e[v]];
                addf = std::min(addf, e.cap - e.flow);
            }
            
            for (int v = t; v != s; v = pre_v[v]) {
                int id = pre_e[v];
                edge_list[id].flow                += addf;
                edge_list[edge_list[id].rev].flow -= addf;
            }
            
            flow += addf;
            cost += addf * pot[t];
        }
        
        return { flow, cost };
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::head;
    using graph<edge_type>::built;
    using graph<edge_type>::edge_list;
    
    std::vector<int> fwd_ids;
};

} // namespace ds
} // namespace cp
#endif