#include "set.hpp"

#ifndef CP_DS_TREE_MULTISET
#define CP_DS_TREE_MULTISET
namespace cp {
namespace ds {


// --- Multiset ngắn gọn & mạnh mẽ ---
template <typename Tp>
class multiset {
public:
    using value_type = Tp;
    
    struct iterator {
        typename cp::ds::set<std::pair<value_type, int>>::const_iterator it;

        iterator(typename cp::ds::set<std::pair<value_type, int>>::const_iterator i) : it(i) {}

        const value_type& operator*() const {
            return it->first;
        }

        const value_type* operator->() const {
            return &it->first;
        }

        iterator& operator++() {
            ++it;
            return *this;
        }

        iterator& operator--() {
            --it;
            return *this;
        }

        bool operator==(const iterator& o) const {
            return it == o.it;
        }

        bool operator!=(const iterator& o) const {
            return it != o.it;
        }
    };

    iterator begin() const {
        return iterator(s.begin());
    }

    iterator end() const {
        return iterator(s.end());
    }

    size_t size() const {
        return s.size();
    }

    bool empty() const {
        return s.empty();
    }

    void clear() {
        s.clear();
        tick = 0;
    }

    // Insert: O(log N)
    void insert(const value_type& x) {
        s.insert({x, tick++});
    }

    // Erase Iterator: O(log N) - Xóa chính xác phần tử tại it
    void erase(iterator it) {
        s.erase(it.it);
    }

    // Erase One: O(log N) - Xóa ĐÚNG 1 phần tử có giá trị x (nếu có)
    bool erase_one(const value_type& x) {
        auto it = s.lower_bound({x, -2e9});
        if (it != s.end() && it->first == x) {
            s.erase(it);
            return true;
        }
        return false;
    }

    // Erase All: O(k log N) - Xóa TẤT CẢ phần tử bằng x (chuẩn std::multiset)
    size_t erase(const value_type& x) {
        size_t cnt = 0;
        while (erase_one(x)) cnt++;
        return cnt;
    }

    // Find: O(log N)
    iterator find(const value_type& x) const {
        auto it = s.lower_bound({x, -2e9});
        return (it != s.end() && it->first == x) ? iterator(it) : end();
    }

    // Lower/Upper Bound: O(log N)
    iterator lower_bound(const value_type& x) const {
        return iterator(s.lower_bound({x, -2e9}));
    }

    iterator upper_bound(const value_type& x) const {
        return iterator(s.lower_bound({x, 2e9}));
    }

    // Count: O(log N) - Siêu nhanh nhờ order_of_key
    size_t count(const value_type& x) const {
        return s.order_of_key({x, 2e9}) - s.order_of_key({x, -2e9});
    }

    // Order of Key: O(log N) - Đếm số lượng phần tử < x
    size_t order_of_key(const value_type& x) const {
        return s.order_of_key({x, -2e9});
    }

    // Find by Order: O(log N) - Trả về phần tử thứ k (0-indexed)
    value_type find_by_order(size_t k) const { 
        return s.find_by_order(k)->first;
    }

private:
    cp::ds::set<std::pair<value_type, int>> s;
    int tick = 0;

};

} // namespace ds
} // namespace cp
#endif
