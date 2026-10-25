#include "graph.hpp"

#ifndef CP_DS_GRAPH_TECC
#define CP_DS_GRAPH_TECC

namespace cp {
namespace ds {

template <typename Edge>
class tecc : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::adj;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::edge_list;
    using graph<edge_type>::graph;
    using graph<edge_type>::init;

    void build() override {
        if (built) return;
        graph<edge_type>::build();

        cid.assign(n, -1);
        cnt = 0;

        std::vector<int> in(n, -1), low(n, -1);
        std::vector<int> stk;
        stk.reserve(n);
        int timer = 0;

        std::vector<std::tuple<int, int, int, bool>> frames;
        frames.reserve(n);

        for (int src = 0; src < n; ++src) {
            if (in[src] != -1) continue;

            frames.push_back({src, -1, 0, false});
            in[src] = low[src] = timer++;
            stk.push_back(src);

            while (!frames.empty()) {
                auto& [u, p, i, skip_p] = frames.back();
                auto nbrs = adj(u);

                if (i < static_cast<int>(nbrs.size())) {
                    int v = nbrs[i].to;
                    ++i;

                    if (v == p && !skip_p) {
                        skip_p = true;
                        continue;
                    }

                    if (in[v] == -1) {
                        in[v] = low[v] = timer++;
                        stk.push_back(v);
                        frames.push_back({v, u, 0, false});
                    } else {
                        low[u] = std::min(low[u], in[v]);
                    }
                } else {
                    if (low[u] == in[u]) {
                        while (true) {
                            int v = stk.back();
                            stk.pop_back();
                            cid[v] = cnt;
                            if (v == u) break;
                        }
                        ++cnt;
                    }
                    frames.pop_back();
                    if (!frames.empty()) {
                        int par = frames.back().u;
                        low[par] = std::min(low[par], low[u]);
                    }
                }
            }
        }

        mrk.assign(num_edges(), false);
        brs.clear();

        for (int i = 0; i < num_edges(); ++i) {
            const auto& e = edge_list[i];
            if (cid[e.from] != cid[e.to]) {
                mrk[i] = true;
                if (e.from < e.to) {
                    brs.push_back(e);
                }
            }
        }

        built = true;
    }

    const std::vector<bool>& mark() {
        if (!built) build();
        return mrk;
    }

    const std::vector<edge_type>& bridges() {
        if (!built) build();
        return brs;
    }

    std::pair<int, const std::vector<int>&> comp() {
        if (!built) build();
        return { cnt, cid };
    }

    [[nodiscard]] int num_components() {
        if (!built) build();
        return cnt;
    }

    [[nodiscard]] int operator[](int u) {
        if (!built) build();
        return cid[u];
    }

    graph<edge_type> compress() {
        if (!built) build();

        std::vector<edge_type> new_edges;
        new_edges.reserve(brs.size() * 2);

        for (const auto& e : edge_list) {
            int u = cid[e.from];
            int v = cid[e.to];
            if (u != v) {
                edge_type new_e = e;
                new_e.from = u;
                new_e.to   = v;
                new_edges.push_back(std::move(new_e));
            }
        }

        std::sort(new_edges.begin(), new_edges.end());
        new_edges.erase(std::unique(new_edges.begin(), new_edges.end(),
            [](const edge_type& a, const edge_type& b) {
                return a.from == b.from && a.to == b.to;
            }), 
            new_edges.end()
        );

        return graph<edge_type>(new_edges);
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::built;

    int cnt = 0;
    std::vector<int> cid;
    std::vector<bool> mrk;
    std::vector<edge_type> brs;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_TECC