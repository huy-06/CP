#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstdlib>
#include "../../alg/utils/random.hpp"

#ifndef CP_DS_RANDOMIZED_HEAP
#define CP_DS_RANDOMIZED_HEAP

namespace cp {
namespace ds {

/**
 * @brief Randomized Heap (Mergeable Heap)
 * 
 * Độ phức tạp:
 * - push: O(log N)
 * - pop: O(log N)
 * - top: O(1)
 * - absorb (merge): O(log N + log M)
 * 
 * @tparam Tp Kiểu dữ liệu
 * @tparam Compare Hàm so sánh (mặc định std::less<Tp> tạo ra Max Heap giống std::priority_queue)
 */
template <typename Tp, typename Compare = std::less<Tp>>
class randomized_heap {
public:
    using value_type = Tp;

    randomized_heap() : root(nullptr), siz(0) {}

    randomized_heap(const std::vector<value_type>& data) : root(nullptr), siz(0) {
        for (const auto& x : data) push(x);
    }

    randomized_heap(const randomized_heap& rhs) : root(nullptr), siz(0) {
        if (rhs.root) deep_copy(root, rhs.root);
        siz = rhs.siz;
    }

    randomized_heap(randomized_heap&& rhs) noexcept : root(rhs.root), siz(rhs.siz) {
        rhs.root = nullptr;
        rhs.siz = 0;
    }

    ~randomized_heap() {
        clear();
    }

    randomized_heap& operator=(const randomized_heap& rhs) {
        if (this != &rhs) {
            clear();
            if (rhs.root) deep_copy(root, rhs.root);
            siz = rhs.siz;
        }
        return *this;
    }

    randomized_heap& operator=(randomized_heap&& rhs) noexcept {
        if (this != &rhs) {
            clear();
            root = rhs.root;
            siz = rhs.siz;
            rhs.root = nullptr;
            rhs.siz = 0;
        }
        return *this;
    }

    bool empty() const {
        return root == nullptr;
    }

    size_t size() const {
        return siz;
    }

    void push(const value_type& val) {
        heap_node* node = new heap_node(val);
        root = merge(root, node);
        siz++;
    }

    const value_type& top() const {
        assert(!empty() && "Heap is empty");
        return root->val;
    }

    void pop() {
        assert(!empty() && "Heap is empty");
        heap_node* old_root = root;
        root = merge(root->l, root->r);
        delete old_root;
        siz--;
    }

    /**
     * @brief Hợp nhất heap khác vào heap này.
     * @note Sau khi gọi, heap `rhs` sẽ trở nên rỗng.
     */
    void absorb(randomized_heap& rhs) {
        if (this == &rhs) return;
        root = merge(root, rhs.root);
        siz += rhs.siz;
        
        rhs.root = nullptr;
        rhs.siz = 0;
    }

    void clear() {
        clear_recursive(root);
        root = nullptr;
        siz = 0;
    }

private:
    struct heap_node {
        value_type val;
        heap_node *l, *r;
        heap_node(const value_type& v) : val(v), l(nullptr), r(nullptr) {}
    };

    heap_node* root;
    size_t siz;
    Compare comp; 

    heap_node* merge(heap_node* a, heap_node* b) {
        if (!a) return b;
        if (!b) return a;

        if (comp(a->val, b->val)) std::swap(a, b);

        if (alg::random() & 1) std::swap(a->l, a->r);

        a->l = merge(a->l, b);
        return a;
    }

    void clear_recursive(heap_node* u) {
        if (!u) return;
        clear_recursive(u->l);
        clear_recursive(u->r);
        delete u;
    }

    void deep_copy(heap_node*& dest, heap_node* src) {
        if (!src) {
            dest = nullptr;
            return;
        }
        dest = new heap_node(src->val);
        deep_copy(dest->l, src->l);
        deep_copy(dest->r, src->r);
    }
};

} // namespace ds
} // namespace cp

#endif