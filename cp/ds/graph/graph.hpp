#include <vector>
#include <algorithm>

#ifndef CP_DS_GRAPH
#define CP_DS_GRAPH
namespace cp {
namespace ds {

template <typename Edge>
class graph {
public:
    template <typename Tp>
    struct neighbors_base {
        Tp *b, *e;

        Tp* begin() const {
            return b;
        }

        Tp* end() const {
            return e;
        }

        int size() const {
            return static_cast<int>(e - b);
        }

        bool empty() const {
            return b == e;
        }

        Tp& operator[](int i) const {
            return b[i];
        }
    };

    using edge_type = Edge;
    using neighbors = neighbors_base<edge_type>;
    using const_neighbors = neighbors_base<const edge_type>;

    graph(int n = 0, int m = 0) {
        init(n, m);
    }

    graph(const std::vector<edge_type>& edges) {
        init(edges);
    }

    void init(int n = 0, int m = 0) {
        this->n = n;
        built = false;
        head.assign(n + 1, 0);
        indeg_cnt.assign(n, 0);
        edge_list.clear();
        edge_list.reserve(m);
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

        for (const auto& e : edge_list) {
            sorted_edges[cur[e.from]++] = e;
        }

        edge_list = std::move(sorted_edges);
        built = true;
    }

    void add_edge(const edge_type& e) {
        assert(0 <= e.from && e.from < num_vertices());
        assert(0 <= e.to && e.to < num_vertices());
        edge_list.push_back(e);
        built = false;
    }

    void add_edge(edge_type&& e) {
        assert(0 <= e.from && e.from < num_vertices());
        assert(0 <= e.to && e.to < num_vertices());
        edge_list.emplace_back(std::move(e));
        built = false;
    }

    void add_unedge(edge_type e) {
        add_edge(e);
        std::swap(e.from, e.to);
        add_edge(std::move(e));
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

    int size() const {
        return n;
    }

    int num_vertices() const {
        return n;
    }

    int num_edges() const {
        return static_cast<int>(edge_list.size());
    }

    int indeg(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return indeg_cnt[u];
    }

    int outdeg(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return head[u + 1] - head[u];
    }

    neighbors adj(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        return neighbors {
            edge_list.data() + head[u],
            edge_list.data() + head[u + 1]
        };
    }

    const_neighbors adj(int u) const {
        assert(0 <= u && u < num_vertices());
        if (!built) const_cast<graph*>(this)->build();
        return const_neighbors {
            edge_list.data() + head[u],
            edge_list.data() + head[u + 1]
        };
    }

    neighbors operator[](int u) {
        return adj(u);
    }

    const_neighbors operator[](int u) const {
        return adj(u);
    }

    void print() { 
//<
#ifdef CP_DEBUG
        std::stringstream ss;
        for (auto e : edge_list) {
            ss << e << '\n';
        }
        std::ofstream out("cp/debug/test_input.txt");
        out << n << ' ' << edge_list.size() << '\n';
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
        int ret = system("python cp/debug/draw_graph.py");
        if (ret != 0) {
            std::cerr << "Failed to run draw_graph.py (return code " << ret << ")\n";
            return;
        }
#endif
//>
    }

    std::vector<edge_type> get_edges() const {
        return edge_list;
    }

    void sort(int u) {
        assert(0 <= u && u < num_vertices());
        if (!built) build();
        std::sort(edge_list.begin() + head[u], edge_list.begin() + head[u + 1],
            [&](const edge_type& a, const edge_type& b) {
                return a.to < b.to;
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
        return static_cast<int>(&e - edge_list.data());
    }

    edge_type* edge_at(int i) {
        assert(0 <= i && i < num_edges());
        return &edge_list[i];
    }

    const edge_type* edge_at(int i) const {
        assert(0 <= i && i < num_edges());
        return &edge_list[i];
    }

protected:
    int n;
    bool built;
    std::vector<int> head;
    std::vector<int> indeg_cnt;
    std::vector<edge_type> edge_list;
};

} // namespace ds
} // namespace cp
#endif