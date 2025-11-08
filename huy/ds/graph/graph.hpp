#include <vector>
#include <algorithm>

#ifndef HUY_DS_GRAPH
#define HUY_DS_GRAPH
namespace huy {
namespace ds {

template <typename Edge>
class graph {
public:
    using edge_type = Edge;

    struct neighbors {
        struct iterator {
            using iterator_category = std::forward_iterator_tag;
            using value_type        = edge_type;
            using difference_type   = std::ptrdiff_t;
            using pointer           = edge_type*;
            using reference         = edge_type&;

            typename std::vector<int>::const_iterator it;
            std::vector<edge_type>* edges_ptr;

            edge_type& operator*() const {
                return (*edges_ptr)[*it];
            }

            iterator& operator++() {
                ++it;
                return *this;
            }

            bool operator!=(const iterator& other) const {
                return it != other.it;
            }
        };

        typename std::vector<int>::const_iterator b, e;
        std::vector<edge_type>* edges_ptr;

        iterator begin() const {
            return iterator{ b, edges_ptr };
        }

        iterator end() const {
            return iterator{ e, edges_ptr };
        }

        edge_type& operator[](int i) {
            assert(0 <= i && i < size());
            return (*edges_ptr)[b[i]];
        }

        const edge_type& operator[](int i) const {
            assert(0 <= i && i < size());
            return (*edges_ptr)[b[i]];
        }

        int size() const {
            return static_cast<int>(std::distance(b, e));
        }

        int distance(const iterator& other) const {
            return std::distance(b, other.it);
        }
    };

    graph(int n = 0, int m = 0) {
        init(n, m);
    }

    void init(int n = 0, int m = 0) {
        this->n = n;
        built = false;
        edge_list.clear();
        edge_list.reserve(m);
        head.assign(n + 1, 0);
        edge_idxs.clear();
        edge_idxs.reserve(m);
        indeg_cnt.assign(n, 0);
    }

    void init(const std::vector<edge_type>& edges) {
        int max_v = 0;
        for (auto &e : edges) {
            max_v = std::max({max_v, e.from, e.to});
        }
        init(max_v + 1, static_cast<int>(edges.size()));
        for (auto &e : edges) {
            add_edge(e);
        }
        build();
    }

    void init(const std::vector<std::vector<edge_type>>& adj) {
        int m = 0;
        for (const auto& nbrs : adj) {
            m += static_cast<int>(nbrs.size());
        }
        init(static_cast<int>(adj.size()), m);
        for (int u = 0; u < static_cast<int>(adj.size()); ++u) {
            for (const auto& e : adj[u]) {
                edge_type new_e = e;
                new_e.from = u;
                add_edge(std::move(new_e));
            }
        }
        build();
    }


    void build() {
        if (built) return;
        for (int i = 1; i <= n; ++i) {
            head[i] += head[i - 1];
        }
        edge_idxs.resize(edge_list.size());
        std::vector<int> cur(head.begin(), head.begin() + n);
        for (int i = 0; i < static_cast<int>(edge_list.size()); ++i) {
            edge_idxs[cur[edge_list[i].from]++] = i;
        }
        built = true;
    }

    void add_edge(const edge_type& e) {
        assert(0 <= e.from && e.from < V());
        assert(0 <= e.to && e.to < V());
        edge_list.push_back(e);
        ++head[e.from + 1];
        ++indeg_cnt[e.to];
        built = false;
    }

    void add_edge(edge_type&& e) {
        assert(0 <= e.from && e.from < V());
        assert(0 <= e.to && e.to < V());
        edge_list.emplace_back(std::move(e));
        ++head[e.from + 1];
        ++indeg_cnt[e.to];
        built = false;
    }

    void add_unedge(edge_type e) {
        add_edge(e);
        std::swap(e.from, e.to);
        add_edge(std::move(e));
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

    void read_unedge(int m, int off = 1) {
        for (int i = 0; i < m; ++i) {
            edge_type e;
            std::cin >> e;
            e.from -= off;
            e.to   -= off;
            add_unedge(std::move(e));
        }
        build();
    }

    int V() const {
        return n;
    }

    int E() const {
        return static_cast<int>(edge_list.size());
    }

    int indeg(int u) const {
        assert(0 <= u && u < V());
        return indeg_cnt[u];
    }

    int outdeg(int u) const {
        assert(0 <= u && u < V());
        if (!built) const_cast<graph*>(this)->build();
        return head[u + 1] - head[u];
    }

    neighbors operator[](int u) {
        assert(0 <= u && u < V());
        if (!built) build();
        return neighbors {
            edge_idxs.begin() + head[u],
            edge_idxs.begin() + head[u + 1],
            &edge_list
        };
    }
    
    neighbors operator[](int u) const {
        assert(0 <= u && u < V());
        if (!built) const_cast<graph*>(this)->build();
        return neighbors {
            const_cast<std::vector<int>&>(edge_idxs).begin() + head[u],
            const_cast<std::vector<int>&>(edge_idxs).begin() + head[u + 1],
            const_cast<std::vector<edge_type>*>(&edge_list)
        };
    }

    void print() {
//<
#ifdef HUY_DEBUG
        std::stringstream ss;
        for (auto e : edge_list) {
            ss << e << '\n';
        }
        std::ofstream out("../../debug/test_input.txt");
        out << n << ' ' << edge_idxs.size() << '\n';
        std::string s;
        while (std::getline(ss, s)) {
            for (char c : s) {
                if (std::isdigit(c)) {
                    out << c;
                } else {
                    out << ' ';
                }
            }
            out << '\n';
        }
        out.close();
        int ret = system("python ../../debug/draw_graph.py");
        if (ret != 0) {
            std::cerr << "Failed to run draw_graph.py (return code " << ret << ")\n";
            return;
        }
#endif
//>
    }

    std::vector<edge_type>& get_edges() const {
        return edge_list;
    }

    void sort(int u) {
        assert(0 <= u && u < V());
        if (!built) build();
        std::sort(edge_idxs.begin() + head[u], edge_idxs.begin() + head[u + 1],
            [&](int i, int j) {
                return edge_list[i].to < edge_list[j].to;
            }
        );
    }

    void sort() {
        if (!built) build();
        for (int u = 0; u < n; ++u) {
            sort(u);
        }
    }

    int index(const edge_type& e) const {
        assert(!edge_list.empty());
        return static_cast<int>(&e - &edge_list[0]);
    }

    edge_type* edge_at(int i) {
        assert(0 <= i && i < E());
        return &edge_list[i];
    }

    const edge_type* edge_at(int i) const {
        assert(0 <= i && i < E());
        return &edge_list[i];
    }

protected:
    int n;
    bool built;
    std::vector<int> head;
    std::vector<int> edge_idxs;
    std::vector<int> indeg_cnt;
    std::vector<edge_type> edge_list;
};

} // namespace ds
} // namespace huy
#endif