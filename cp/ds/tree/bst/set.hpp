#include "map.hpp"

#ifndef CP_DS_TREE_SET
#define CP_DS_TREE_SET
namespace cp {
namespace ds {

template <typename Tp>
using set = map<Tp, __gnu_pbds::null_type>;

} // namespace ds
//<
namespace internal {

template <typename Tp>
struct formatter<ds::set<Tp>> {
    static void print(std::ostream& os, const ds::set<Tp>& v) {
        os << style::color_green << "set" << style::reset;
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
