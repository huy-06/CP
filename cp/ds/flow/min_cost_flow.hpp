#include <queue>
#include "../edge/mcf_edge.hpp"
#include "../graph/graph.hpp"

#ifndef CP_DS_FLOW_MIN_COST_FLOW
#define CP_DS_FLOW_MIN_COST_FLOW

namespace cp {
namespace ds {

template <typename Edge>
class min_cost_flow : public graph<Edge> {
public:
    using edge_type = Edge;
    using cap_type  = typename edge_type::cap_type;
    using cost_type = typename edge_type::cost_type;

    using graph<edge_type>::init;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::num_vertices;

    explicit min_cost_flow(int n = 0, int m = 0) {
        init(n, m);
    }

    void add_edge(const edge_type& e) override {
        if (e.cost < 0) has_negative_cost = true;
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        rev_e.cap  = 0;
        rev_e.cost = -rev_e.cost;
        rev_e.flow = 0;
        graph<edge_type>::add_edge(e);
        graph<edge_type>::add_edge(rev_e);
    }

    void add_edge(int from, int to, cap_type cap, cost_type cost) {
        add_edge(edge_type(from, to, cap, cost, 0));
    }

    void add_unedge(const edge_type& e) override {
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        add_edge(e);
        add_edge(rev_e);
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

    [[nodiscard]] std::vector<edge_type> get_edges() const override {
        if (!built) const_cast<min_cost_flow*>(this)->build();

        std::vector<edge_type> res;
        res.reserve(fwd_ids.size());
        for (int id : fwd_ids) {
            res.push_back(edge_list[id]);
        }
        return res;
    }

    std::pair<cap_type, cost_type> flow(int s, int t, cap_type limit = edge_type::inf_cap) {
        assert(0 <= s && s < num_vertices());
        assert(0 <= t && t < num_vertices());

        build();

        cap_type  flow_val = 0;
        cost_type cost_val = 0;

        std::vector<cost_type> pot(n, 0);
        std::vector<cost_type> dis(n);
        std::vector<int>       pv(n, -1);
        std::vector<int>       pe(n, -1);

        if (has_negative_cost) {
            std::fill(pot.begin(), pot.end(), edge_type::inf_cost);
            std::vector<bool> inq(n, false);
            std::queue<int> q;

            pot[s] = 0;
            q.push(s);
            inq[s] = true;

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                inq[u] = false;

                for (int i = head[u]; i < head[u + 1]; ++i) {
                    const auto& e = edge_list[i];
                    if (e.residual_cap() > 0 && pot[u] < edge_type::inf_cost) {
                        cost_type nd = pot[u] + e.cost;
                        if (pot[e.to] > nd) {
                            pot[e.to] = nd;
                            if (!inq[e.to]) {
                                q.push(e.to);
                                inq[e.to] = true;
                            }
                        }
                    }
                }
            }
        }

        using pci = std::pair<cost_type, int>;

        while (flow_val < limit) {
            std::fill(dis.begin(), dis.end(), edge_type::inf_cost);
            std::priority_queue<pci, std::vector<pci>, std::greater<pci>> pq;

            dis[s] = 0;
            pq.emplace(0, s);

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();

                if (d != dis[u]) continue;

                for (int i = head[u]; i < head[u + 1]; ++i) {
                    const auto& e = edge_list[i];
                    if (e.residual_cap() > 0) {
                        int v = e.to;
                        cost_type nd = d + e.cost + pot[u] - pot[v];
                        if (dis[v] > nd) {
                            dis[v] = nd;
                            pv[v]  = u;
                            pe[v]  = i;
                            pq.emplace(nd, v);
                        }
                    }
                }
            }

            if (dis[t] == edge_type::inf_cost) {
                break;
            }

            for (int v = 0; v < n; ++v) {
                if (dis[v] < edge_type::inf_cost) {
                    pot[v] += dis[v];
                }
            }

            cap_type delta = limit - flow_val;
            for (int v = t; v != s; v = pv[v]) {
                const auto& e = edge_list[pe[v]];
                delta = std::min(delta, e.residual_cap());
            }

            for (int v = t; v != s; v = pv[v]) {
                int id = pe[v];
                edge_list[id].flow                += delta;
                edge_list[edge_list[id].rev].flow -= delta;
            }

            flow_val += delta;
            cost_val += delta * pot[t];
        }

        return { flow_val, cost_val };
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::head;
    using graph<edge_type>::built;
    using graph<edge_type>::edge_list;

    bool has_negative_cost = false;
    std::vector<int> fwd_ids;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_FLOW_MIN_COST_FLOW