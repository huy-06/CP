#include <queue>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include "../graph/graph.hpp"

#ifndef CP_DS_GRAPH_FLOW_MAX_FLOW
#define CP_DS_GRAPH_FLOW_MAX_FLOW
namespace cp {
namespace ds {

template <typename Edge>
class max_flow : public graph<Edge> {
public:
    using edge_type = Edge;
    using value_type = typename edge_type::value_type;
    using neighbors = typename graph<edge_type>::neighbors;
    using const_neighbors = typename graph<edge_type>::const_neighbors;

    using graph<edge_type>::init;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::num_edges;

    max_flow(int n = 0, int m = 0) {
        init(n, m);
    }
    
    void add_edge(edge_type e) {
        graph<edge_type>::add_edge(e);
        std::swap(e.from, e.to);
        e.cap = 0;
        graph<edge_type>::add_edge(std::move(e));
    }
    
    void add_unedge(edge_type e) {
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        add_edge(std::move(e));
        add_edge(std::move(rev_e));
    }

    void build() {
        if (built) return;

        head.assign(n + 1, 0);
        indeg_cnt.assign(n, 0);

        for (const auto& e : edge_list) {
            ++head[e.from + 1];
            ++indeg_cnt[e.to];
        }

        for (int i = 1; i <= n; ++i) {
            head[i] += head[i - 1];
        }

        std::vector<edge_type> sorted_edges(edge_list.size());
        std::vector<int> cur = head;
        std::vector<int> new_pos(edge_list.size());

        for (size_t i = 0; i < edge_list.size(); ++i) {
            new_pos[i] = cur[edge_list[i].from]++;
            sorted_edges[new_pos[i]] = edge_list[i];
        }

        edge_list = std::move(sorted_edges);

        rev_idx.assign(edge_list.size(), 0);
        is_forward.assign(edge_list.size(), false);
        forward_edge_idx.clear();
        forward_edge_idx.reserve(edge_list.size() / 2);

        for (size_t i = 0; i < edge_list.size(); i += 2) {
            int pos1 = new_pos[i];
            int pos2 = new_pos[i + 1];
            rev_idx[pos1] = pos2;
            rev_idx[pos2] = pos1;
            is_forward[pos1] = true;
            forward_edge_idx.push_back(pos1);
        }

        built = true;
    }

    int indeg(int u) const {
        if (!built) const_cast<max_flow*>(this)->build();
        return graph<edge_type>::indeg(u);
    }

    int outdeg(int u) const {
        if (!built) const_cast<max_flow*>(this)->build();
        return graph<edge_type>::outdeg(u);
    }

    neighbors adj(int u) {
        if (!built) build();
        return graph<edge_type>::adj(u);
    }

    const_neighbors adj(int u) const {
        if (!built) const_cast<max_flow*>(this)->build();
        return graph<edge_type>::adj(u);
    }

    neighbors operator[](int u) {
        return adj(u);
    }

    const_neighbors operator[](int u) const {
        return adj(u);
    }
    
    void sort(int u) { 
        assert(false && "Cannot sort a max_flow graph!"); 
    }
    
    void sort() {
        assert(false && "Cannot sort a max_flow graph!");
    }

    bool change_edge(edge_type e) {
        build();
        int u = e.from;
        for (int i = head[u]; i < head[u + 1]; ++i) {
            if (!is_forward[i]) continue;
            edge_type& ce = edge_list[i];
            if (ce.to == e.to) {
                ce.cap = e.cap;
                return true;
            }
        }
        return false;
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
    
    void read_unedge(int m, int off = 1, std::istream& is = std::cin) {
        for (int i = 0; i < m; ++i) {
            edge_type e;
            is >> e;
            e.from -= off;
            e.to   -= off;
            add_unedge(std::move(e));
        }
        build();
    }

    value_type flow(int s, int t) {
        assert(0 <= s && s < num_vertices());
        assert(0 <= t && t < num_vertices());
        build();
        value_type ans = 0;
        ptr.assign(num_vertices(), 0);
        level.assign(num_vertices(), -1);
        while (bfs(s, t)) {
            for (int u = 0; u < num_vertices(); ++u) {
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
        std::vector<bool> vis(num_vertices(), false);
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
        if (!built) const_cast<max_flow*>(this)->build();
        std::vector<edge_type> res;
        res.reserve(forward_edge_idx.size());
        for (int idx : forward_edge_idx) {
            res.push_back(edge_list[idx]);
        }
        return res;
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::built;
    using graph<edge_type>::head;
    using graph<edge_type>::indeg_cnt;
    using graph<edge_type>::edge_list;

    std::vector<int> ptr;
    std::vector<int> level;
    
    std::vector<int> rev_idx;
    std::vector<bool> is_forward;
    std::vector<int> forward_edge_idx;

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
            edge_type& e = edge_list[i];
            int v = e.to;
            if (level[v] != level[u] + 1) continue;
            value_type rem = e.cap - e.flow;
            if (rem <= 0) continue;
            value_type tr = dfs(v, t, std::min(pushed, rem));
            if (tr > 0) {
                e.flow += tr;
                edge_list[rev_idx[i]].flow -= tr;
                return tr;
            }
        }
        return 0;
    }
};

} // namespace ds
} // namespace cp
#endif