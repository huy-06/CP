#include <queue>
#include "../../../ds/graph/graph.hpp"

#ifndef CP_GRAPH_TOPOLOGICAL_SORT
#define CP_GRAPH_TOPOLOGICAL_SORT

namespace cp {
namespace alg {

/**
 * @brief Sắp xếp topo trên đồ thị CÓ HƯỚNG bằng thuật toán Kahn (Queue / BFS).
 * @param g Đồ thị (instance của class graph<Edge>).
 * @return std::vector<int> danh sách đỉnh theo thứ tự topo.
 *         Nếu đồ thị có chu trình (không phải DAG), trả về vector rỗng {}.
 */
template <typename Graph>
std::vector<int> topological_sort(const Graph& g) {
    int n = g.num_vertices();

    std::vector<int> indeg(n);
    for (int i = 0; i < n; ++i) {
        indeg[i] = g.indeg(i);
    }

    std::vector<int> ord;
    ord.reserve(n);

    for (int i = 0; i < n; ++i) {
        if (indeg[i] == 0) {
            ord.push_back(i);
        }
    }

    int i = 0;
    while (i < static_cast<int>(ord.size())) {
        int u = ord[i++];
        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (--indeg[v] == 0) {
                ord.push_back(v);
            }
        }
    }

    if (static_cast<int>(ord.size()) != n) {
        return {};
    }
    return ord;
}

/**
 * @brief Kiểm tra đồ thị có phải là DAG (Directed Acyclic Graph) hay không.
 */
template <typename Graph>
bool is_dag(const Graph& g) {
    return !topological_sort(g).empty();
}

/**
 * @brief Sắp xếp Topo có THỨ TỰ TỪ ĐIỂN NHỎ NHẤT (Lexicographically Smallest).
 * @param g Đồ thị (instance của class graph<Edge>).
 * @return std::vector<int> topo order nhỏ nhất, hoặc {} nếu có chu trình.
 */
template <typename Graph>
std::vector<int> lexicographical_topological_sort(const Graph& g) {
    int n = g.num_vertices();

    std::vector<int> indeg(n);
    for (int i = 0; i < n; ++i) {
        indeg[i] = g.indeg(i);
    }

    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    for (int i = 0; i < n; ++i) {
        if (indeg[i] == 0) {
            pq.push(i);
        }
    }

    std::vector<int> ord;
    ord.reserve(n);

    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();
        ord.push_back(u);

        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (--indeg[v] == 0) {
                pq.push(v);
            }
        }
    }

    if (static_cast<int>(ord.size()) != n) {
        return {};
    }
    return ord;
}

} // namespace alg
} // namespace cp

#endif // CP_GRAPH_TOPOLOGICAL_SORT