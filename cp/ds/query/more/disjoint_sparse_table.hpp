#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

#ifndef CP_DS_DISJOINT_SPARSE_TABLE
#define CP_DS_DISJOINT_SPARSE_TABLE
namespace cp {
namespace ds {

/**
 * @brief Disjoint Sparse Table cho truy vấn O(1) trên đoạn tĩnh.
 * Phù hợp với các phép toán có tính kết hợp (như nhân, cộng, min, max, gcd...).
 * KHÔNG hỗ trợ cập nhật (Static range queries).
 * 
 * @tparam Tp Kiểu dữ liệu của các phần tử.
 * @tparam op Con trỏ hàm định nghĩa phép toán kết hợp 2 phần tử (ví dụ: cộng, max, min).
 *            Hàm này phải thỏa mãn tính kết hợp: op(a, op(b, c)) == op(op(a, b), c).
 * @tparam e Con trỏ hàm trả về phần tử đơn vị (Identity element) của phép toán `op`.
 * 
 * @par Ví dụ sử dụng:
 * @code
 * using Tp = int;
 * Tp op(Tp a, Tp b) { return std::min(a, b); }
 * Tp e() { return 1e9; } // Vô cùng lớn đối với phép tìm min
 * 
 * cp::ds::disjoint_sparse_table<Tp, op, e> dst(data);
 * @endcode
 */
template <class Tp, Tp (*op)(Tp, Tp), Tp (*e)()>
class disjoint_sparse_table {
public:
    using value_type = Tp;

    disjoint_sparse_table() : n(0) {}

    disjoint_sparse_table(const std::vector<value_type>& data) {
        init(data);
    }

    void init(const std::vector<value_type>& data) {
        n = static_cast<int>(data.size());
        if (n == 0) return;

        int levels = 0;
        while ((1 << levels) < n) levels++;
        
        data_copy = data; 
        table.assign(levels, std::vector<value_type>(n));

        for (int k = 0; k < levels; ++k) {
            int len = 1 << (k + 1);
            int mid_offset = 1 << k;

            for (int start = 0; start < n; start += len) {
                int mid = start + mid_offset;
                if (mid >= n) break;

                value_type cur = data[mid - 1];
                table[k][mid - 1] = cur;
                for (int i = mid - 2; i >= start; --i) {
                    cur = op(data[i], cur);
                    table[k][i] = cur;
                }

                cur = data[mid];
                table[k][mid] = cur;
                int end = std::min(start + len, n);
                for (int i = mid + 1; i < end; ++i) {
                    cur = op(cur, data[i]);
                    table[k][i] = cur;
                }
            }
        }
    }

    /**
     * @brief Truy vấn đoạn [l, r) - Nửa khoảng mở
     * @param l chỉ số bắt đầu (inclusive)
     * @param r chỉ số kết thúc (exclusive)
     * @return Kết quả gộp đoạn [l, r-1]
     */
    value_type range_query(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        
        if (l == r) {
            return e();
        }

        int r_inclusive = r - 1;

        if (l == r_inclusive) {
            return data_copy[l];
        }

        int k = 31 - __builtin_clz(l ^ r_inclusive);
        
        return op(table[k][l], table[k][r_inclusive]);
    }

private:
    int n;
    std::vector<value_type> data_copy;
    std::vector<std::vector<value_type>> table;
};

} // namespace ds
} // namespace cp
#endif