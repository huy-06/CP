#include <vector>
#include "montgomery.hpp"

#ifndef CP_DS_MODULAR_COMBINATORICS
#define CP_DS_MODULAR_COMBINATORICS

namespace cp {
namespace ds {

/**
 * @brief Lớp tiện ích tính toán Tổ hợp (Combinatorics) tổng quát.
 *
 * Phiên bản mặc định dành cho các kiểu dữ liệu `Tp` hỗ trợ phép chia `/` trực tiếp
 * (ví dụ: `double`, `long double`, phân số, số nguyên lớn...) mà không có hàm thành viên `.inv()`.
 * Hỗ trợ tự động mở rộng bảng giai thừa khi truy vấn vượt quá giới hạn hiện tại.
 *
 * @tparam Tp Kiểu dữ liệu giá trị trả về (cần hỗ trợ `Tp(0)`, `Tp(1)`, `+`, `-`, `*`, `/`).
 * @tparam Enable Tham số SFINAE dùng để phân nhánh với các kiểu modulo có `.inv()`.
 *
 * @note Đây là lớp thuần tĩnh (không thể tạo đối tượng), gọi trực tiếp qua `combinatorics<Tp>::ten_ham(...)`.
 *
 * ### Ví dụ sử dụng
 * ```cpp
 * using comb = cp::ds::combinatorics<double>;
 * comb::init(100);               // (Tùy chọn) Tiền xử lý trước đến 100
 * double c = comb::binom(10, 3); // C(10, 3) = 120
 * ```
 */
template <typename Tp, typename Enable = void>
class combinatorics {
public:
    using value_type = Tp;

    combinatorics() = delete;

    /**
     * @brief Tiền xử lý bảng giai thừa `0!, 1!, ..., n!`.
     *
     * Nếu `n` nhỏ hơn hoặc bằng kích thước đã tính trước thì hàm không làm gì cả.
     *
     * @param n Giá trị lớn nhất cần tiền xử lý (`n >= 0`).
     * @note Độ phức tạp: `O(n - s)` với `s` là kích thước đã khởi tạo trước đó.
     */
    static void init(int n) {
        if (n <= s) 
            return;

        facts.resize(n + 1);
        for (int i = s + 1; i <= n; ++i) {
            facts[i] = facts[i - 1] * value_type(i);
        }
        s = n;
    }

    /**
     * @brief Tính giai thừa `n!`.
     *
     * Tự động mở rộng bảng tiền xử lý theo chiến lược nhân đôi dung lượng nếu `n > s`.
     *
     * @param n Số nguyên không âm (`n >= 0`).
     * @return Giá trị `n!`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type fact(int n) {
        if (n > s) 
            init(std::max(n, s << 1));

        return facts[n];
    }

    /**
     * @brief Tính tổ hợp `C(n, k)`: số cách chọn `k` phần tử từ `n` phần tử (không xét thứ tự).
     *
     * Công thức: `n! / (k! * (n - k)!)`.
     *
     * @param n Tổng số phần tử.
     * @param k Số phần tử được chọn.
     * @return Giá trị `C(n, k)`, trả về `0` nếu `k < 0` hoặc `k > n`.
     * @note Độ phức tạp: `O(1)` khấu hao (phụ thuộc độ phức tạp phép `/` của `Tp`).
     */
    static value_type binom(int n, int k) {
        if (k < 0 || k > n) 
            return value_type(0);

        return fact(n) / (fact(k) * fact(n - k));
    }

    /**
     * @brief Tính chỉnh hợp `A(n, k)`: số cách chọn có thứ tự `k` phần tử từ `n` phần tử.
     *
     * Công thức: `n! / (n - k)!`.
     *
     * @param n Tổng số phần tử.
     * @param k Số phần tử được chọn.
     * @return Giá trị `A(n, k)`, trả về `0` nếu `k < 0` hoặc `k > n`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type perm(int n, int k) {
        if (k < 0 || k > n) 
            return value_type(0);

        return fact(n) / fact(n - k);
    }

    /**
     * @brief Tính tổ hợp lặp `H(n, k)` (Stars and Bars).
     *
     * Đếm số cách chia `k` phần tử giống nhau vào `n` nhóm phân biệt (mỗi nhóm có thể rỗng),
     * hoặc số nghiệm nguyên không âm của `x_1 + x_2 + ... + x_n = k`.
     *
     * Công thức: `C(n + k - 1, k)`.
     *
     * @param n Số nhóm (`n >= 0`).
     * @param k Số phần tử giống nhau (`k >= 0`).
     * @return Giá trị `H(n, k)`. Trả về `1` nếu `n == 0 && k == 0`; trả về `0` nếu `n < 0` hoặc `k < 0`.
     */
    static value_type multichoose(int n, int k) {
        if (n < 0 || k < 0) 
            return value_type(0);

        if (n == 0 && k == 0) 
            return value_type(1);

        return binom(n + k - 1, k);
    }

    /**
     * @brief Giải bài toán Ballot: đếm đường đi trên lưới từ `(0, 0)` đến `(n, m)` không vượt lên trên đường `y = x`.
     *
     * Mọi điểm `(x, y)` trên đường đi luôn thỏa mãn `x >= y`.
     *
     * Công thức: `C(n + m, m) - C(n + m, m - 1)`.
     *
     * **Ứng dụng:** Đếm số dãy gồm `n` số `+1` và `m` số `-1` sao cho mọi tổng tiền tố đều `>= 0`.
     *
     * @param n Số bước sang phải (`n >= m`).
     * @param m Số bước lên trên (`m >= 0`).
     * @return Số đường đi hợp lệ, trả về `0` nếu `m < 0` hoặc `n < m`.
     */
    static value_type ballot(int n, int m) {
        if (m < 0 || n < m) 
            return value_type(0);

        if (m == 0) 
            return value_type(1);

        return binom(n + m, m) - binom(n + m, m - 1);
    }

    /**
     * @brief Tính số Catalan thứ `n`: `C_n = C(2n, n) / (n + 1)`.
     *
     * **Ứng dụng:**
     * - Số dãy ngoặc đúng có độ dài `2n` (`n` cặp ngoặc).
     * - Số cây nhị phân có `n` nút.
     * - Số cách chia đa giác lồi `n + 2` cạnh thành các tam giác không giao nhau.
     *
     * @param n Chỉ số Catalan (`n >= 0`).
     * @return Số Catalan thứ `n`, trả về `0` nếu `n < 0`.
     */
    static value_type catalan(int n) {
        if (n < 0) 
            return value_type(0);

        return ballot(n, n);
    }

    /**
     * @brief Tính số Catalan bậc `k`: hệ số `[x^n] C(x)^k`.
     *
     * Công thức: `(k / (n + k)) * C(2n + k - 1, n) = ballot(n + k - 1, n)`.
     *
     * **Ứng dụng:**
     * - Số cách ghép `k` dãy ngoặc hợp lệ nối tiếp nhau có tổng cộng `n` cặp ngoặc.
     * - Số rừng gồm `k` cây nhị phân có tổng cộng `n` đỉnh.
     * - Số dãy ngoặc hợp lệ bắt đầu với `k` dấu `(` dư sẵn.
     *
     * @param n Tổng số cặp ngoặc / số đỉnh (`n >= 0`).
     * @param k Bậc của chuỗi Catalan (`k >= 0`).
     * @return Hệ số `[x^n] C(x)^k`, trả về `0` nếu tham số âm hoặc (`k == 0 && n > 0`).
     */
    static value_type catalan_k(int n, int k) {
        if (n < 0 || k < 0) 
            return value_type(0);

        if (k == 0) 
            return n == 0 ? value_type(1) : value_type(0);

        if (n == 0) 
            return value_type(1);
            
        return ballot(n + k - 1, n);
    }

private:
    inline static int s = 0;
    inline static std::vector<value_type> facts{value_type(1)};
};

/**
 * @brief Lớp tiện ích tính toán Tổ hợp theo Modulo (Modular Combinatorics).
 *
 * Phiên bản chuyên biệt (SFINAE) tự động kích hoạt khi `Tp` có phương thức `.inv()`
 * (ví dụ: `MontgomeryModInt`). Tiền xử lý đồng thời bảng giai thừa, nghịch đảo giai thừa
 * và nghịch đảo modulo trong `O(N + log MOD)` để trả lời mọi truy vấn tổ hợp trong `O(1)`.
 *
 * @tparam Tp Kiểu số nguyên modulo có phương thức `inv()`.
 *
 * @note Đây là lớp thuần tĩnh (không thể khởi tạo đối tượng).
 *       Bảng sẽ tự động mở rộng khi cần, nhưng nên gọi `init(MAXN)` trước nếu biết giới hạn.
 *
 * ### Ví dụ sử dụng
 * ```cpp
 * using mint = ...; // Kiểu ModInt có .inv()
 * using comb = cp::ds::combinatorics<mint>;
 *
 * void solve() {
 *     comb::init(200000);
 *     mint c1 = comb::binom(100000, 50000);      // O(1)
 *     mint c2 = comb::binom_large(1e18, 1000);   // O(k) với n lớn, k nhỏ
 *     mint cat = comb::catalan(500);             // O(1)
 * }
 * ```
 */
template <typename Tp>
class combinatorics<Tp, std::void_t<decltype(std::declval<Tp>().inv())>> {
public:
    using value_type = Tp;

    combinatorics() = delete;

    /**
     * @brief Tiền xử lý bảng `facts` (`i!`), `inv_facts` (`(i!)^-1`) và `invs` (`i^-1`) đến `n`.
     *
     * Chỉ gọi `.inv()` đúng 1 lần cho `n!`, sau đó suy ngược lại toàn bộ bảng bằng phép nhân.
     * Nếu `n <= s` thì hàm không thực hiện gì.
     *
     * @param n Giới hạn lớn nhất cần tiền xử lý (`n >= 0`, cần `n < MOD` để `n!` khả nghịch).
     * @note Độ phức tạp: `O((n - s) + log MOD)`.
     */
    static void init(int n) {
        if (n <= s) 
            return;

        facts.resize(n + 1);
        inv_facts.resize(n + 1);
        invs.resize(n + 1);

        for (int i = s + 1; i <= n; ++i) {
            facts[i] = facts[i - 1] * value_type(i);
        }

        inv_facts[n] = facts[n].inv();
        for (int i = n; i > s + 1; --i) {
            inv_facts[i - 1] = inv_facts[i] * value_type(i);
            invs[i] = inv_facts[i] * facts[i - 1];
        }

        if (n > s) {
            invs[s + 1] = inv_facts[s + 1] * facts[s];
        }

        s = n;
    }

    /**
     * @brief Trả về giai thừa `n! % MOD`.
     *
     * Tự động mở rộng bảng tiền xử lý nếu `n > s`.
     *
     * @param n Số nguyên không âm (`n >= 0`).
     * @return Giá trị `n! % MOD`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type fact(int n) {
        if (n > s) 
            init(std::max(n, s << 1));

        return facts[n];
    }

    /**
     * @brief Trả về nghịch đảo modulo của giai thừa: `(n!)^-1 % MOD`.
     *
     * Tự động mở rộng bảng tiền xử lý nếu `n > s`.
     *
     * @param n Số nguyên không âm (`n >= 0`).
     * @return Giá trị `(n!)^-1 % MOD`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type inv_fact(int n) {
        if (n > s) 
            init(std::max(n, s << 1));

        return inv_facts[n];
    }

    /**
     * @brief Trả về nghịch đảo modulo của `n`: `n^-1 % MOD`.
     *
     * Tự động mở rộng bảng tiền xử lý nếu `n > s`.
     *
     * @param n Số nguyên cần lấy nghịch đảo (`n >= 1`, với `n = 0` trả về `0`).
     * @return Giá trị `n^-1 % MOD`.
     * @note Độ phức tạp: `O(1)` khấu hao. Nhanh hơn gọi `Tp(n).inv()` khi gọi nhiều lần.
     */
    static value_type inv(int n) {
        if (n > s) 
            init(std::max(n, s << 1));

        return invs[n];
    }

    /**
     * @brief Tính tổ hợp `C(n, k) % MOD`: số cách chọn `k` phần tử từ `n` phần tử (không thứ tự).
     *
     * Công thức: `fact(n) * inv_fact(k) * inv_fact(n - k)`.
     *
     * @param n Tổng số phần tử.
     * @param k Số phần tử được chọn.
     * @return Giá trị `C(n, k) % MOD`, trả về `0` nếu `k < 0` hoặc `k > n`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type binom(int n, int k) {
        if (k < 0 || k > n) 
            return value_type(0);

        return fact(n) * inv_fact(k) * inv_fact(n - k);
    }

    /**
     * @brief Tính chỉnh hợp `A(n, k) % MOD`: số cách chọn có thứ tự `k` phần tử từ `n` phần tử.
     *
     * Công thức: `fact(n) * inv_fact(n - k)`.
     *
     * @param n Tổng số phần tử.
     * @param k Số phần tử được chọn.
     * @return Giá trị `A(n, k) % MOD`, trả về `0` nếu `k < 0` hoặc `k > n`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type perm(int n, int k) {
        if (k < 0 || k > n) 
            return value_type(0);

        return fact(n) * inv_fact(n - k);
    }

    /**
     * @brief Tính tổ hợp `C(n, k) % MOD` khi `n` rất lớn (`<= 10^18`) nhưng `k` nhỏ.
     *
     * Công thức: `(n * (n - 1) * ... * (n - k + 1)) * inv_fact(k)`.
     *
     * @param n Tổng số phần tử (kiểu `long long`).
     * @param k Số phần tử được chọn (`k < MOD`).
     * @return Giá trị `C(n, k) % MOD`, trả về `0` nếu `k < 0` hoặc `n < k`.
     * @note Độ phức tạp: `O(k)` (tự động dùng `O(1)` nếu `n <= s`).
     */
    static value_type binom_large(long long n, int k) {
        if (k < 0 || n < k) 
            return value_type(0);

        if (k == 0 || k == n) 
            return value_type(1);

        if (n <= s) 
            return binom(static_cast<int>(n), k);

        value_type num(1);
        for (int i = 0; i < k; ++i) {
            num *= value_type(n - i);
        }
        return num * inv_fact(k);
    }

    /**
     * @brief Tính chỉnh hợp / giai thừa giảm `n^{underline{k}}` khi `n` lớn (`<= 10^18`) và `k` nhỏ.
     *
     * Công thức: `n * (n - 1) * ... * (n - k + 1) % MOD`.
     *
     * @param n Giá trị bắt đầu (kiểu `long long`).
     * @param k Số thừa số cần nhân (`k >= 0`).
     * @return Giá trị giai thừa giảm theo modulo, trả về `0` nếu `k < 0` hoặc `n < k`.
     * @note Độ phức tạp: `O(k)` (tự động dùng `O(1)` nếu `n <= s`).
     */
    static value_type perm_large(long long n, int k) {
        if (k < 0 || n < k) 
            return value_type(0);

        if (k == 0) 
            return value_type(1);

        if (n <= s) 
            return perm(static_cast<int>(n), k);

        value_type res(1);
        for (int i = 0; i < k; ++i) {
            res *= value_type(n - i);
        }
        return res;
    }

    /**
     * @brief Tính giai thừa tăng (Rising Factorial): `n^{overline{k}} % MOD`.
     *
     * Công thức: `n * (n + 1) * ... * (n + k - 1) = perm_large(n + k - 1, k)`.
     *
     * @param n Giá trị bắt đầu (kiểu `long long`).
     * @param k Số thừa số liên tiếp (`k >= 0`).
     * @return Giá trị `n^{overline{k}} % MOD`, trả về `0` nếu `k < 0`, `1` nếu `k == 0`.
     * @note Độ phức tạp: `O(k)`.
     */
    static value_type rising_fact(long long n, int k) {
        if (k < 0) 
            return value_type(0);

        if (k == 0) 
            return value_type(1);

        return perm_large(n + k - 1, k);
    }

    /**
     * @brief Tính tổ hợp lặp `H(n, k) % MOD` (Stars and Bars).
     *
     * Đếm số cách chia `k` phần tử giống nhau vào `n` nhóm phân biệt (mỗi nhóm có thể rỗng).
     * Công thức: `C(n + k - 1, k)`.
     *
     * @param n Số nhóm (`n >= 0`).
     * @param k Số phần tử giống nhau (`k >= 0`).
     * @return Giá trị `H(n, k) % MOD`. Trả về `1` nếu `n == 0 && k == 0`; `0` nếu `n < 0` hoặc `k < 0`.
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type multichoose(int n, int k) {
        if (n < 0 || k < 0) 
            return value_type(0);

        if (n == 0 && k == 0) 
            return value_type(1);

        return binom(n + k - 1, k);
    }

    /**
     * @brief Giải bài toán Ballot: đếm đường đi trên lưới từ `(0, 0)` đến `(n, m)` luôn thỏa `x >= y`.
     *
     * Công thức: `(C(n + m, m) - C(n + m, m - 1)) % MOD`.
     *
     * **Ứng dụng:** Đếm số dãy gồm `n` số `+1` và `m` số `-1` có mọi tổng tiền tố `>= 0`.
     *
     * @param n Số bước sang phải (`n >= m`).
     * @param m Số bước lên trên (`m >= 0`).
     * @return Số đường đi hợp lệ theo modulo, trả về `0` nếu `m < 0` hoặc `n < m`.
     * @note Độ phức tạp: `O(1)` khấu hao. Không cần chia cho `n + 1`.
     */
    static value_type ballot(int n, int m) {
        if (m < 0 || n < m) 
            return value_type(0);

        if (m == 0) 
            return value_type(1);

        return binom(n + m, m) - binom(n + m, m - 1);
    }

    /**
     * @brief Tính số Catalan thứ `n` theo modulo: `C_n = C(2n, n) * (n + 1)^-1 % MOD`.
     *
     * **Ứng dụng:**
     * - Số dãy ngoặc đúng độ dài `2n`.
     * - Số cây nhị phân có `n` nút.
     * - Số đường đi Dyck từ `(0, 0)` đến `(n, n)` không vượt lên trên `y = x`.
     *
     * @param n Chỉ số Catalan (`n >= 0`).
     * @return Số Catalan thứ `n` theo modulo, trả về `0` nếu `n < 0`.
     * @note Độ phức tạp: `O(1)` khấu hao. Yêu cầu `n + 1` khả nghịch theo `MOD`.
     */
    static value_type catalan(int n) {
        if (n < 0) 
            return value_type(0);

        return binom(n << 1, n) * inv(n + 1);
    }

    /**
     * @brief Tính số Catalan bậc `k` theo modulo: hệ số `[x^n] C(x)^k`.
     *
     * Công thức: `(k / (n + k)) * C(2n + k - 1, n) = ballot(n + k - 1, n)`.
     *
     * **Ứng dụng:**
     * - Số cách nối `k` dãy ngoặc hợp lệ có tổng cộng `n` cặp ngoặc.
     * - Số rừng gồm `k` cây nhị phân có tổng cộng `n` đỉnh.
     *
     * @param n Tổng số cặp ngoặc / số đỉnh (`n >= 0`).
     * @param k Bậc của chuỗi Catalan (`k >= 0`).
     * @return Hệ số `[x^n] C(x)^k` theo modulo, trả về `0` nếu tham số âm hoặc (`k == 0 && n > 0`).
     * @note Độ phức tạp: `O(1)` khấu hao.
     */
    static value_type catalan_k(int n, int k) {
        if (n < 0 || k < 0) 
            return value_type(0);

        if (k == 0) 
            return n == 0 ? value_type(1) : value_type(0);

        if (n == 0) 
            return value_type(1);

        return ballot(n + k - 1, n);
    }

private:
    inline static int s = 0;
    inline static std::vector<value_type> facts{value_type(1)};
    inline static std::vector<value_type> inv_facts{value_type(1)};
    inline static std::vector<value_type> invs{value_type(0)};
};

} // namespace ds
} // namespace cp

#endif