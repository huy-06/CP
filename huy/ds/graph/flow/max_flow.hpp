#include <queue>
#include <cassert>
#include "../graph.hpp"

#ifndef HUY_DS_GRAPH_FLOW_MAX_FLOW
#define HUY_DS_GRAPH_FLOW_MAX_FLOW
namespace huy {
namespace ds {

template <typename Edge>
class max_flow : public graph<Edge> {
public:
    using edge_type = Edge;
    using value_type = typename edge_type::value_type;
    using graph<edge_type>::init;
    using graph<edge_type>::build;
    using graph<edge_type>::V;
    using graph<edge_type>::E;

    max_flow(int n = 0, int m = 0) {
        init(n, m);
    }
    
    void add_edge(edge_type e) {
        graph<edge_type>::add_edge(std::move(e));
        std::swap(e.from, e.to);
        e.cap = 0;
        graph<edge_type>::add_edge(std::move(e));
    }

    bool change_edge(edge_type e) {
        build();
        int u = e.from;
        for (int i = head[u]; i < head[u + 1]; ++i) {
            int idx = edge_idxs[i];
            if ((idx & 1) != 0) continue;
            edge_type& ce = edge_list[idx];
            if (ce.to == e.to) {
                ce.cap = e.cap;
                return true;
            }
        }
        return false;
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


    value_type flow(int s, int t) {
        assert(0 <= s && s < V());
        assert(0 <= t && t < V());
        build();
        value_type ans = 0;
        ptr.assign(V(), 0);
        level.assign(V(), -1);
        while (bfs(s, t)) {
            for (int u = 0; u < V(); ++u) {
                ptr[u] = head[u];
            }
            while (true) {
                value_type pushed = dfs(s, t, edge_type::inf);
                if (pushed == 0) break;
                ans += pushed;
            }
        }
        return ans;
    }

    std::vector<bool> min_cut(int s) {
        build();
        std::vector<bool> vis(V(), false);
        std::queue<int> q;
        vis[s] = true;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const auto& e : (*this)[u]) {
                int v = e.to;
                if (!vis[v] && e.cap - e.flow > 0) {
                    vis[v] = true;
                    q.push(v);
                }
            }
        }
        return vis;
    }

    std::vector<edge_type> get_edges() const {
        std::vector<edge_type> res;
        res.reserve(E() >> 1 | 1);
        for (int i = 0; i < E(); i += 2) {
            res.push_back(edge_list[i]);
        }
        return res;
    }

private:
    using graph<edge_type>::head;
    using graph<edge_type>::edge_list;
    using graph<edge_type>::edge_idxs;

    std::vector<int> ptr;
    std::vector<int> level;

    bool bfs(int s, int t) {
        std::fill(level.begin(), level.end(), -1);
        std::queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const auto& e : (*this)[u]) {
                int v = e.to;
                if (level[v] == -1 && e.cap - e.flow > 0) {
                    level[v] = level[u] + 1;
                    if (v == t) return true;
                    q.push(v);
                }
            }
        }
        return false;
    }

    value_type dfs(int u, int t, value_type pushed) {
        if (pushed == 0) return 0;
        if (u == t) return pushed;
        for (int& i = ptr[u]; i < head[u + 1]; ++i) {
            int eidx = edge_idxs[i];
            edge_type& e = edge_list[eidx];
            int v = e.to;
            if (level[v] != level[u] + 1) continue;
            value_type rem = e.cap - e.flow;
            if (rem <= 0) continue;
            value_type tr = dfs(v, t, std::min(pushed, rem));
            if (tr > 0) {
                e.flow += tr;
                int r = eidx ^ 1;
                if (0 <= r && r < E()) edge_list[r].flow -= tr;
                return tr;
            }
        }
        return 0;
    }
};

} // namespace ds
} // namespace huy
#endif