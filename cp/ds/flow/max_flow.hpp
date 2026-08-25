#include <queue>
#include "../edge/flow.hpp"
#include "../graph/graph.hpp"

#ifndef CP_DS_FLOW_MAX_FLOW
#define CP_DS_FLOW_MAX_FLOW
namespace cp {
namespace ds {

template <typename Edge>
class max_flow : public graph<Edge> {
public:
    using edge_type       = Edge;
    using value_type      = typename edge_type::value_type;
    using neighbors       = typename graph<edge_type>::neighbors;
    using const_neighbors = typename graph<edge_type>::const_neighbors;

    using graph<edge_type>::init;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::num_vertices;

    max_flow(int n = 0, int m = 0) { 
        init(n, m); 
    }

    void build() override {
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

        std::vector<int> cur = head;
        std::vector<int> pos(edge_list.size());
        std::vector<edge_type> sorted(edge_list.size());

        for (int i = 0; i < (int) edge_list.size(); ++i) {
            pos[i] = cur[edge_list[i].from]++;
            sorted[pos[i]] = edge_list[i];
        }

        is_fwd.assign(edge_list.size(), false);
        rev_idx.assign(edge_list.size(), 0);
        
        fwd_ids.clear();
        fwd_ids.reserve(edge_list.size() / 2);

        for (int i = 0; i < (int) edge_list.size(); i += 2) {
            int id_fwd = pos[i];
            int id_rev = pos[i + 1];
            
            rev_idx[id_fwd] = id_rev;
            rev_idx[id_rev] = id_fwd;
            
            is_fwd[id_fwd] = true;
            fwd_ids.push_back(id_fwd);
        }

        edge_list = std::move(sorted);
        built     = true;
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

    bool change_edge(edge_type e) {
        build();
        int u = e.from;
        for (int i = head[u]; i < head[u + 1]; ++i) {
            if (!is_fwd[i]) continue;
            
            edge_type& ce = edge_list[i];
            if (ce.to == e.to) {
                ce.cap = e.cap;
                return true;
            }
        }
        return false;
    }

    std::vector<edge_type> get_edges() const {
        if (!built) const_cast<max_flow*>(this)->build();

        std::vector<edge_type> res;
        res.reserve(fwd_ids.size());

        for (int id : fwd_ids) {
            res.push_back(edge_list[id]);
        }
        return res;
    }

    value_type flow(int s, int t) {
        assert(0 <= s && s < num_vertices());
        assert(0 <= t && t < num_vertices());
        
        build();
        
        ptr.resize(n);
        lvl.resize(n);
        
        value_type ans = 0;
        while (bfs(s, t)) {
            std::copy(head.begin(), head.begin() + n, ptr.begin());
            while (true) {
                value_type pushed = dfs(s, t, edge_type::inf);
                if (pushed == 0) 
                    break;
                ans += pushed;
            }
        }
        return ans;
    }

    std::vector<bool> min_cut(int s) {
        build();
        std::vector<bool> vis(n, false);
        std::queue<int>   q;
        
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

private:
    using graph<edge_type>::n;
    using graph<edge_type>::head;
    using graph<edge_type>::built;
    using graph<edge_type>::indeg_cnt;
    using graph<edge_type>::edge_list;

    std::vector<int> ptr;
    std::vector<int> lvl;
    std::vector<bool> is_fwd;
    std::vector<int>  fwd_ids;
    std::vector<int>  rev_idx;

    bool bfs(int s, int t) {
        std::fill(lvl.begin(), lvl.end(), -1);
        std::queue<int> q;
        
        lvl[s] = 0;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front(); 
            q.pop();
            
            for (int i = head[u]; i < head[u + 1]; ++i) {
                const auto& e = edge_list[i];
                int         v = e.to;
                
                if (lvl[v] == -1 && e.cap - e.flow > 0) {
                    lvl[v] = lvl[u] + 1;
                    if (v == t) 
                        return true;
                    q.push(v);
                }
            }
        }
        return false;
    }

    value_type dfs(int u, int t, value_type pushed) {
        if (pushed == 0 || u == t) 
            return pushed;
        
        for (int& i = ptr[u]; i < head[u + 1]; ++i) {
            edge_type& e = edge_list[i];
            int        v = e.to;
            
            if (lvl[v] != lvl[u] + 1) 
                continue;
            
            value_type rem = e.cap - e.flow;
            if (rem <= 0) 
                continue;
            
            value_type tr = dfs(v, t, std::min(pushed, rem));
            if (tr > 0) {
                e.flow                     += tr;
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