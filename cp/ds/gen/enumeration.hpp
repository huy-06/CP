#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

#ifndef CP_DS_GEN_ENUMERATION
#define CP_DS_GEN_ENUMERATION

namespace cp {
namespace ds {
namespace gen {

/**
 * @brief Duyệt qua tất cả các đoạn con liên tiếp a[l..r] với 0 <= l <= r < n.
 * @note Thường dùng để vét cạn tìm đoạn con có tổng lớn nhất, thỏa mãn điều kiện,...
 */
template <class Tp>
struct subarrays {
    const std::vector<Tp>& a;

    explicit subarrays(const std::vector<Tp>& arr) : a(arr) {}

    struct iterator {
        const std::vector<Tp>& a;
        int n, l, r;
        bool done;
        std::vector<Tp> cur;

        const std::vector<Tp>& operator*() {
            cur.assign(a.begin() + l, a.begin() + r + 1);
            return cur;
        }

        iterator& operator++() {
            ++r;
            if (r == n) {
                ++l;
                r = l;
            }
            if (l == n) done = true;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {a, (int) a.size(), 0, 0, a.empty()};
    }

    iterator end() const {
        return {a, (int) a.size(), 0, 0, true};
    }
};

/**
 * @brief Duyệt tất cả 2^n tập con của mảng a (có thể chứa tập con trùng nhau nếu mảng có phần tử trùng).
 */
template <class Tp>
struct subsets {
    const std::vector<Tp>& a;
    explicit subsets(const std::vector<Tp>& arr) : a(arr) {}

    struct iterator {
        const std::vector<Tp>& a;
        int mask;
        std::vector<Tp> cur;

        const std::vector<Tp>& operator*() {
            cur.clear();
            for (int i = 0; i < (int) a.size(); ++i) {
                if ((mask >> i) & 1) cur.push_back(a[i]);
            }
            return cur;
        }

        iterator& operator++() {
            ++mask;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return mask != other.mask;
        }
    };

    iterator begin() const {
        return {a, 0, {}};
    }

    iterator end() const {
        return {a, 1 << (int) a.size(), {}};
    }
};

/**
 * @brief Duyệt các tập con KHÔNG TRÙNG LẶP khi mảng a có các phần tử giống nhau.
 * @note Ví dụ: a = {1, 2, 2} sẽ sinh ra: {}, {1}, {2}, {1, 2}, {2, 2}, {1, 2, 2} (6 tập thay vì 8).
 */
template <class Tp>
struct unique_subsets {
    std::vector<std::pair<Tp, int>> freq;

    explicit unique_subsets(std::vector<Tp> a) {
        std::map<Tp, int> cnt;
        for (const auto& x : a) cnt[x]++;
        for (const auto& p : cnt) freq.push_back(p);
    }

    struct iterator {
        const std::vector<std::pair<Tp, int>>& freq;
        std::vector<int> take;
        bool done;
        std::vector<Tp> cur;

        const std::vector<Tp>& operator*() {
            cur.clear();
            for (size_t i = 0; i < freq.size(); ++i) {
                for (int c = 0; c < take[i]; ++c) cur.push_back(freq[i].first);
            }
            return cur;
        }

        iterator& operator++() {
            size_t pos = 0;
            while (pos < freq.size()) {
                if (take[pos] < freq[pos].second) {
                    take[pos]++;
                    break;
                } else {
                    take[pos] = 0;
                    pos++;
                }
            }
            if (pos == freq.size()) done = true;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {freq, std::vector<int>(freq.size(), 0), freq.empty(), {}};
    }

    iterator end() const {
        return {freq, {}, true, {}};
    }
};

/**
 * @brief Duyệt tất cả hoán vị của mảng a (tự động loại bỏ trùng lặp nếu mảng có phần tử trùng).
 */
template <class Tp>
struct permutations {
    std::vector<Tp> a;
    explicit permutations(std::vector<Tp> arr) : a(std::move(arr)) {
        std::ranges::sort(a);
    }

    struct iterator {
        std::vector<Tp> cur;
        bool done;

        const std::vector<Tp>& operator*() const {
            return cur;
        }

        iterator& operator++() {
            if (!std::ranges::next_permutation(cur).found) {
                done = true;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {a, a.empty()};
    }

    iterator end() const {
        return {{}, true};
    }
};

/**
 * @brief Duyệt tất cả tổ hợp chập k của mảng a (chọn k phần tử không phân biệt thứ tự).
 */
template <class Tp>
struct combinations {
    const std::vector<Tp>& a;
    int k;

    combinations(const std::vector<Tp>& arr, int k_) : a(arr), k(k_) {}

    struct iterator {
        const std::vector<Tp>& a;
        int n, k;
        std::vector<bool> sel;
        std::vector<Tp> cur;
        bool done;

        iterator(const std::vector<Tp>& arr, int k_, bool is_end)
            : a(arr), n(arr.size()), k(k_), done(is_end) {
            if (!done) {
                if (k < 0 || k > n) { done = true; return; }
                sel.assign(n, false);
                std::fill(sel.begin(), sel.begin() + k, true);
            }
        }

        const std::vector<Tp>& operator*() {
            cur.clear();
            for (int i = 0; i < n; ++i) {
                if (sel[i]) cur.push_back(a[i]);
            }
            return cur;
        }

        iterator& operator++() {
            if (!std::ranges::prev_permutation(sel).found) {
                done = true;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {a, k, false};
    }

    iterator end() const {
        return {a, k, true};
    }
};

/**
 * @brief Duyệt tất cả chỉnh hợp không lặp chập k của mảng a (chọn k phần tử CÓ PHÂN BIỆT thứ tự, A_n^k).
 */
template <class Tp>
struct arrangements {
    std::vector<Tp> a;
    int k;

    arrangements(std::vector<Tp> arr, int k_) : a(std::move(arr)), k(k_) {
        std::ranges::sort(a);
    }

    struct iterator {
        std::vector<Tp> a;
        int n, k;
        bool done;
        std::vector<Tp> cur;

        iterator(std::vector<Tp> arr, int k_, bool is_end)
            : a(std::move(arr)), n(a.size()), k(k_), done(is_end) {
            if (!done && (k < 0 || k > n)) done = true;
        }

        const std::vector<Tp>& operator*() {
            cur.assign(a.begin(), a.begin() + k);
            return cur;
        }

        iterator& operator++() {
            // Lật ngược đoạn [k, n) để next_permutation chỉ thay đổi prefix độ dài k
            std::reverse(a.begin() + k, a.end());
            if (!std::ranges::next_permutation(a).found) {
                done = true;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {a, k, false};
    }

    iterator end() const {
        return {a, k, true};
    }
};

/**
 * @brief Duyệt tất cả dãy độ dài n với mỗi phần tử nhận giá trị từ 0 đến base - 1.
 * @note Ví dụ: base = 2 là xâu nhị phân, base = 3 là xâu tam phân,...
 */
struct base_k {
    int n, base;
    base_k(int n_, int base_) : n(n_), base(base_) {}

    struct iterator {
        int n, base;
        std::vector<int> cur;
        bool done;

        const std::vector<int>& operator*() const {
            return cur;
        }

        iterator& operator++() {
            int pos = n - 1;
            while (pos >= 0) {
                if (cur[pos] + 1 < base) {
                    cur[pos]++;
                    break;
                } else {
                    cur[pos] = 0;
                    pos--;
                }
            }
            if (pos < 0) done = true;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {n, base, std::vector<int>(n, 0), n <= 0};
    }

    iterator end() const {
        return {n, base, {}, true};
    }
};

/**
 * @brief Duyệt tất cả mask nguyên từ 0 đến (2^n - 1).
 */
struct masks {
    int total;

    explicit masks(int n) : total(1 << n) {}

    struct iterator {
        int mask;

        int operator*() const {
            return mask;
        }

        iterator& operator++() {
            ++mask; return *this;
        }

        bool operator!=(const iterator& other) const {
            return mask != other.mask;
        }
    };

    iterator begin() const {
        return {0};
    }

    iterator end() const {
        return {total};
    }
};

/**
 * @brief Duyệt tất cả submask của một bitmask cho trước.
 */
struct submasks {
    int mask;

    explicit submasks(int m) : mask(m) {}

    struct iterator {
        int mask, sub;
        bool done;

        int operator*() const {
            return sub;
        }

        iterator& operator++() {
            if (sub == 0) done = true;
            else sub = (sub - 1) & mask;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return done != other.done;
        }
    };

    iterator begin() const {
        return {mask, mask, false};
    }

    iterator end() const {
        return {mask, 0, true};
    }
};

} // namespace gen
} // namespace ds
} // namespace cp

#endif // CP_DS_GEN_ENUMERATION