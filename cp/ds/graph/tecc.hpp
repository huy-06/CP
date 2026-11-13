#include "graph.hpp"

#ifndef CP_DS_GRAPH_TECC
#define CP_DS_GRAPH_TECC

namespace cp {
namespace ds {

/**
 * @brief Phân rã thành phần liên thông 2-cạnh (Two-Edge-Connected Components - 2-ECC) và tìm cầu (Bridges).
 * 
 * @tparam Edge Kiểu cạnh của đồ thị (yêu cầu có các trường `from`, `to`).
 */
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

    /**
     * @brief Chạy thuật toán Tarjan (khử đệ quy) tìm cầu và phân rã các 2-ECC.
     * @note Độ phức tạp O(V + E). Các hàm truy vấn bên dưới sẽ tự động gọi hàm này nếu chưa build.
     */
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

    /**
     * @brief Kiểm tra cạnh theo chỉ số trong `edge_list` có phải là cầu hay không.
     * @return const std::vector<bool>& Mảng đánh dấu: `mark()[i] == true` nếu cạnh thứ i là cầu.
     */
    const std::vector<bool>& mark() {
        if (!built) build();
        return mrk;
    }

    /**
     * @brief Lấy danh sách toàn bộ các cạnh cầu của đồ thị.
     * @return const std::vector<edge_type>& Danh sách các cạnh cầu (đã chuẩn hóa `from < to`).
     */
    const std::vector<edge_type>& bridges() {
        if (!built) build();
        return brs;
    }

    /**
     * @brief Lấy số lượng và mảng id thành phần liên thông 2-cạnh của tất cả đỉnh.
     * @return std::pair<int, const std::vector<int>&> Cặp {số_lượng_TPLT, mảng_cid}.
     */
    std::pair<int, const std::vector<int>&> comp() {
        if (!built) build();
        return { cnt, cid };
    }

    /**
     * @brief Lấy tổng số lượng thành phần liên thông 2-cạnh (2-ECC).
     * @return int Số lượng thành phần (các ID được đánh số từ `0` đến `cnt - 1`).
     */
    [[nodiscard]] int num_components() {
        if (!built) build();
        return cnt;
    }

    /**
     * @brief Truy vấn nhanh ID thành phần liên thông 2-cạnh chứa đỉnh `u`.
     * @param u Đỉnh cần tra cứu (0 <= u < n).
     * @return int ID của thành phần 2-ECC chứa `u`.
     * @example int component_u = g[u];
     */
    [[nodiscard]] int operator[](int u) {
        if (!built) build();
        return cid[u];
    }

    /**
     * @brief Co (compress) các 2-ECC thành các siêu đỉnh để tạo thành Cây cầu (Bridge-Block Tree/Forest).
     * @return graph<edge_type> Đồ thị mới dạng Cây/Rừng, trong đó:
     *         - Mỗi đỉnh mới đại diện cho một 2-ECC.
     *         - Mỗi cạnh mới tương ứng với một cầu nối giữa 2 thành phần.
     */
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