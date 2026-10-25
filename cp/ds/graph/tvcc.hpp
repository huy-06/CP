#include "graph.hpp"

#ifndef CP_DS_GRAPH_TVCC
#define CP_DS_GRAPH_TVCC

namespace cp {
namespace ds {

template <typename Edge>
class tvcc : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::adj;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::graph;
    using graph<edge_type>::init;

    void build() override {
        if (built) return;
        graph<edge_type>::build();

        mrk.assign(n, false);
        cts.clear();
        blocks.clear();

        std::vector<int> in(n, -1), low(n, -1);
        std::vector<int> stk;
        stk.reserve(n);
        int timer = 0;

        auto dfs = [&](auto const& self, int u, int p) -> void {
            in[u] = low[u] = timer++;
            stk.push_back(u);

            int chi = 0;
            bool skip_p = false;

            for (const auto& e : adj(u)) {
                int v = e.to;
                if (v == p && !skip_p) {
                    skip_p = true;
                    continue;
                }

                if (in[v] == -1) {
                    ++chi;
                    self(self, v, u);
                    low[u] = std::min(low[u], low[v]);

                    if (low[v] >= in[u]) {
                        if (p != -1) {
                            mrk[u] = true;
                        }

                        std::vector<int> bcc;
                        while (true) {
                            int x = stk.back();
                            stk.pop_back();
                            bcc.push_back(x);
                            if (x == v) break;
                        }
                        bcc.push_back(u);
                        blocks.push_back(std::move(bcc));
                    }
                } else {
                    low[u] = std::min(low[u], in[v]);
                }
            }

            if (p == -1 && chi > 1) {
                mrk[u] = true;
            }
        };

        for (int i = 0; i < n; ++i) {
            if (in[i] == -1) {
                int base = stk.size();
                dfs(dfs, i, -1);

                if ((int) stk.size() > base) {
                    std::vector<int> bcc;
                    while (stk.size() > base) {
                        bcc.push_back(stk.back());
                        stk.pop_back();
                    }
                    if (!bcc.empty()) {
                        blocks.push_back(std::move(bcc));
                    }
                }
            }
        }

        for (int u = 0; u < n; ++u) {
            if (mrk[u]) {
                cts.push_back(u);
            }
        }

        built = true;
    }

    [[nodiscard]] const std::vector<bool>& mark() {
        if (!built) build();
        return mrk;
    }

    [[nodiscard]] const std::vector<int>& cuts() {
        if (!built) build();
        return cts;
    }

    [[nodiscard]] const std::vector<std::vector<int>>& groups() {
        if (!built) build();
        return blocks;
    }

    graph<edge_type> compress() {
        if (!built) build();

        int new_n = n + static_cast<int>(blocks.size());
        graph<edge_type> bct(new_n);

        for (int i = 0; i < (int) blocks.size(); ++i) {
            int bi = n + i;
            for (int u : blocks[i]) {
                bct.add_unedge(edge_type(u, bi));
            }
        }
        bct.build();
        return bct;
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::built;

    std::vector<bool> mrk;
    std::vector<int>  cts;
    std::vector<std::vector<int>> blocks;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_TVCC