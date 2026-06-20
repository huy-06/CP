#include <string>
#include <algorithm>

#ifndef CP_ALG_INT128
#define CP_ALG_INT128
namespace cp {
namespace alg {

__int128 stoi128(const std::string& str) {
    size_t pos = 0;
    while (pos < str.size() && std::isspace(str[pos])) 
        ++pos;

    bool negative = false;
    if (pos < str.size() && (str[pos] == '-' || str[pos] == '+')) {
        if (str[pos] == '-') 
            negative = true;
        ++pos;
    }

    __int128 value = 0;
    while (pos < str.size() && std::isdigit(str[pos])) {
        value = value * 10 + (str[pos] - '0');
        ++pos;
    }

    return negative ? -value : value;
}

std::string to_string(__int128 value) {
    if (value == 0) return "0";

    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }

    std::string str;
    while (value > 0) {
        int digit = static_cast<int>(value % 10);
        str.push_back('0' + digit);
        value /= 10;
    }

    if (negative) {
        str.push_back('-');
    }
    
    std::reverse(str.begin(), str.end());
    return str;
}    

} // namespace alg
} // namespace cp
#endif