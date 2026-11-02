#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>

#ifndef CP_DS_XOR_BASIS
#define CP_DS_XOR_BASIS
namespace cp {
namespace ds {

// Template Hệ cơ sở XOR (XOR Basis) trên trường GF(2)
template <typename Tp, int MAX_BITS = sizeof(Tp) * 8>
class xor_basis {
public:
    static constexpr int bits = MAX_BITS;
    using value_type = Tp;

    xor_basis() { 
        clear();
    }

    void clear() {
        basis.fill(0);
        rank = 0;
        is_normalized = false;
        p.clear();
    }

    // Trả về true nếu thêm thành công (tăng số chiều không gian)
    bool insert(value_type x) {
        for (int i = bits - 1; i >= 0; --i) {
            if (!((x >> i) & 1)) {
                continue;
            }
            if (!basis[i]) {
                basis[i] = x;
                ++rank;
                is_normalized = false;
                return true;
            }
            x ^= basis[i];
        }
        return false;
    }

    // Kiểm tra x có sinh ra được từ hệ cơ sở không (x thuộc V)
    bool contains(value_type x) const {
        for (int i = bits - 1; i >= 0; --i) {
            if (!((x >> i) & 1)) {
                continue;
            }
            if (!basis[i]) {
                return false;
            }
            x ^= basis[i];
        }
        return true;
    }

    // Lấy giá trị lớn nhất có thể tạo ra
    value_type max(value_type x = 0) const {
        value_type res = x;
        for (int i = bits - 1; i >= 0; --i) {
            res = std::max(res, res ^ basis[i]);
        }
        return res;
    }

    // Lấy giá trị nhỏ nhất có thể tạo ra (không tính số 0 ban đầu nếu mảng không rỗng)
    value_type min(value_type x = 0) const {
        value_type res = x;
        for (int i = bits - 1; i >= 0; --i) {
            res = std::min(res, res ^ basis[i]);
        }
        return res;
    }

    int size() const {
        return rank;
    }

    value_type get_basis(int i) const {
        return basis[i];
    }

    value_type operator[](int i) const {
        return basis[i];
    }

    // Chuẩn hóa hệ cơ sở (RREF - Reduced Row Echelon Form)
    void normalize() {
        if (is_normalized) {
            return;
        }
        for (int i = bits - 1; i >= 0; --i) {
            if (!basis[i]) {
                continue;
            }
            for (int j = i - 1; j >= 0; --j) {
                if ((basis[i] >> j) & 1) {
                    basis[i] ^= basis[j];
                }
            }
        }
        p.clear();
        for (int i = 0; i < bits; ++i) {
            if (basis[i]) {
                p.push_back(basis[i]);
            }
        }
        is_normalized = true;
    }

    // Tìm phần tử lớn thứ K (0-indexed, k = 0 trả về 0)
    // Trả về -1 nếu K vượt quá số phần tử có thể tạo ra
    value_type get_kth(unsigned long long k) {
        normalize();
        if (k >= (1ULL << rank)) {
            return -1; 
        }
        value_type res = 0;
        for (int i = 0; i < (int)p.size(); ++i) {
            if ((k >> i) & 1) {
                res ^= p[i];
            }
        }
        return res;
    }

    // Phép HỢP (Union Span) 
    xor_basis merge(const xor_basis& other) const {
        xor_basis res = *this;
        for (int i = 0; i < bits; ++i) {
            if (other.basis[i]) {
                res.insert(other.basis[i]);
            }
        }
        return res;
    }

    xor_basis operator+(const xor_basis& other) const {
        return merge(other);
    }

    xor_basis& operator+=(const xor_basis& other) {
        *this = merge(other);
        return *this;
    }

    // Phép GIAO (Intersection) - Thuật toán Zassenhaus
    xor_basis intersect(const xor_basis& other) const {
        using double_type = typename std::conditional<bits <= 32, unsigned long long, unsigned __int128>::type;
        std::array<double_type, bits * 2> b2 = {0};
        
        auto insert2 = [&](double_type x) {
            for (int i = bits * 2 - 1; i >= 0; --i) {
                if (!((x >> i) & 1)) {
                    continue;
                }
                if (!b2[i]) {
                    b2[i] = x;
                    return;
                }
                x ^= b2[i];
            }
        };

        for (int i = 0; i < bits; ++i) {
            if (basis[i]) {
                insert2((static_cast<double_type>(basis[i]) << bits) | basis[i]);
            }
            if (other.basis[i]) {
                insert2(static_cast<double_type>(other.basis[i]) << bits);
            }
        }

        xor_basis res;
        for (int i = 0; i < bits; ++i) {
            if (b2[i]) {
                res.insert(static_cast<value_type>(b2[i] & ((1ULL << bits) - 1)));
            }
        }
        return res;
    }

    xor_basis operator&(const xor_basis& other) const {
        return intersect(other);
    }

    // Kiểm tra tính bao hàm (Subset)
    bool is_subset_of(const xor_basis& other) const {
        for (int i = 0; i < bits; ++i) {
            if (basis[i] && !other.contains(basis[i])) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<value_type, bits> basis;
    std::vector<value_type> p; 
    bool is_normalized;
    int rank;
};

} // namespace ds
} // namespace cp
#endif