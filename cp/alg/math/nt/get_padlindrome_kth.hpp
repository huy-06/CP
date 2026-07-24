#include <string>
#include <algorithm>

#ifndef CP_ALG_GET_PALINDROME_KTH
#define CP_ALG_GET_PALINDROME_KTH
namespace cp {
namespace alg {

std::string get_padlindrome_kth(const std::string& k) {
    auto subtract = [&](std::string s, int exp) -> std::string {
        int d = (int) s.size();

        int i = d - 1 - exp;
        while (i >= 0) {
            if (s[i] > '0') {
                --s[i];
                break;
            } else {
                s[i] = '9';
                --i;
            }
        }

        int j = 0;
        while (j < d && s[j] == '0') {
            ++j;
        }

        if (j == d) {
            return "0";
        }
        return s.substr(j);
    };

    auto get_palindrome = [&](const std::string& half, bool odd) -> std::string {
        std::string rev = half;
        std::reverse(rev.begin(), rev.end());

        if (odd == true) {
            if (rev.empty() == false) {
                rev.erase(rev.begin());
            }
        }

        return half + rev;
    };

    int d = (int) k.size();

    if (d == 1) {
        return std::to_string(k[0] - '0' - 1);
    }

    if (d == 2) {
        int v = std::stoi(k);

        if (v <= 10) {
            return std::to_string(v - 1);
        }

        if (v <= 19) {
            std::string half = std::to_string(v - 10);
            return get_palindrome(half, false);
        }

        std::string half = std::to_string(v - 10);
        return get_palindrome(half, true);
    }

    std::string s1 = "10" + std::string(d - 2, '9');
    std::string s2 = "1" + std::string(d - 1, '9');

    if (k <= s1) {
        std::string half = subtract(k, d - 2);
        return get_palindrome(half, true);
    }

    if (k <= s2) {
        std::string half = subtract(k, d - 1);
        return get_palindrome(half, false);
    }

    std::string half = subtract(k, d - 1);
    return get_palindrome(half, true);
}

std::string get_padlindrome_kth(long long k) {
    return get_padlindrome_kth(std::to_string(k));
}

} // namespace alg
} // namespace cp
#endif