#include <algorithm>
#include <vector>
#include <string>

#ifndef CP_ALG_IS_PALINDROME
#define CP_ALG_IS_PALINDROME
namespace cp {
namespace alg {

template <typename Array>
bool is_palindrome(const Array& a) {
    if (a.empty()) return true;
    return std::equal(
        a.begin(), 
        a.begin() + a.size() / 2, 
        a.rbegin()
    );
}

} // namespace alg
} // namespace cp
#endif