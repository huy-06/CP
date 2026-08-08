#include "set.hpp"
#include "../../misc/formatter.hpp"

#ifndef CP_DS_TREE_MULTISET
#define CP_DS_TREE_MULTISET
namespace cp {
namespace ds {

template <typename Tp>
class multiset {
public:
    using value_type = Tp;
    
    struct iterator {
        typename set<std::pair<value_type, int>>::const_iterator it;

        iterator(typename set<std::pair<value_type, int>>::const_iterator i) : it(i) {}

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

    void insert(const value_type& x) {
        s.insert({x, tick++});
    }

    void erase(iterator it) {
        s.erase(it.it);
    }

    bool erase_one(const value_type& x) {
        auto it = s.lower_bound({x, INT_MIN});
        if (it != s.end() && it->first == x) {
            s.erase(it);
            return true;
        }
        return false;
    }

    size_t erase(const value_type& x) {
        size_t cnt = 0;
        while (erase_one(x)) cnt++;
        return cnt;
    }

    iterator find(const value_type& x) const {
        auto it = s.lower_bound({x, INT_MIN});
        return (it != s.end() && it->first == x) ? iterator(it) : end();
    }

    iterator lower_bound(const value_type& x) const {
        return iterator(s.lower_bound({x, INT_MIN}));
    }

    iterator upper_bound(const value_type& x) const {
        return iterator(s.lower_bound({x, 2e9}));
    }

    size_t count(const value_type& x) const {
        return s.order_of_key({x, 2e9}) - s.order_of_key({x, INT_MIN});
    }

    // Trả về vị trí phần tử < x
    size_t order_of_key(const value_type& x) const {
        return s.order_of_key({x, INT_MIN});
    }

    value_type find_by_order(size_t k) const { 
        return s.find_by_order(k)->first;
    }

private:
    set<std::pair<value_type, int>> s;
    int tick = 0;
};

} // namespace ds
//<
namespace internal {

template <typename Tp>
struct formatter<ds::multiset<Tp>> {
    static void print(std::ostream& os, const ds::multiset<Tp>& v) {
        os << style::color_green << "multiset" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

} // namespace internal
//>
} // namespace cp
#endif
