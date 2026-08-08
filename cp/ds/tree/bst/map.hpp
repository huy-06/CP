#include <ext/pb_ds/assoc_container.hpp>

#ifndef CP_DS_TREE_MAP
#define CP_DS_TREE_MAP
namespace cp {
namespace ds {

template <typename K, typename V>
using map = __gnu_pbds::tree<
    K, 
    V, 
    std::less<K>,
    __gnu_pbds::rb_tree_tag,
    __gnu_pbds::tree_order_statistics_node_update
>;

} // namespace ds
//<
namespace internal {

template <typename K, typename V>
struct formatter<ds::map<K, V>> {
    static void print(std::ostream& os, const ds::map<K, V>& v) {
        os << style::color_green << "map" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x.first);
            os << ": ";
            print_item(os, x.second);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

} // namespace internal
//>
} // namespace cp
#endif
