#include <vector>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <numeric>

template <typename Tp>
class vec {
public:
    vec() : data() {}

    explicit vec(size_t n, const Tp& v = Tp()) : data(n, v) {}

    template <typename InputIt>
    vec(InputIt first, InputIt last) : data(first, last) {}

    vec(const vec& other) : data(other.data) {}

    vec(vec&& other) noexcept : data(std::move(other.data)) {}

    vec(std::initializer_list<Tp> init) : data(init) {}

    vec(const std::vector<Tp> init) : data(init) {}    

    ~vec() = default;


    vec& operator=(const vec& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    vec& operator=(vec&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    vec& operator=(std::initializer_list<Tp> init) {
        data = init;
        return *this;
    }


    Tp& operator[](size_t index) {
        return data[index];
    }

    const Tp& operator[](size_t index) const {
        return data[index];
    }

    Tp& front() {
        return data.front();
    }

    const Tp& front() const {
        return data.front();
    }

    Tp& back() {
        return data.back();
    }

    const Tp& back() const {
        return data.back();
    }

    Tp* raw_data() noexcept {
        return data.data();
    }

    const Tp* raw_data() const noexcept {
        return data.data();
    }

    auto begin() noexcept { return data.begin(); }
    auto end() noexcept { return data.end(); }
    auto begin() const noexcept { return data.cbegin(); }
    auto end() const noexcept { return data.cend(); }

    auto cbegin() const noexcept { return data.cbegin(); }
    auto cend() const noexcept { return data.cend(); }

    auto rbegin() noexcept { return data.rbegin(); }
    auto rend() noexcept { return data.rend(); }
    auto rbegin() const noexcept { return data.crbegin(); }
    auto rend() const noexcept { return data.crend(); }

    auto crbegin() const noexcept { return data.crbegin(); }
    auto crend() const noexcept { return data.crend(); }


    bool empty() const noexcept {
        return data.empty();
    }

    size_t size() const noexcept {
        return data.size();
    }

    size_t max_size() const noexcept {
        return data.max_size();
    }

    void reserve(size_t new_cap) {
        data.reserve(new_cap);
    }

    size_t capacity() const noexcept {
        return data.capacity();
    }

    void shrink_to_fit() {
        data.shrink_to_fit();
    }


    void clear() noexcept {
        data.clear();
    }

    typename std::vector<Tp>::iterator insert(typename std::vector<Tp>::const_iterator pos, const Tp& value) {
        return data.insert(pos, value);
    }
    typename std::vector<Tp>::iterator insert(typename std::vector<Tp>::const_iterator pos, Tp&& value) {
        return data.insert(pos, std::move(value));
    }
    typename std::vector<Tp>::iterator insert(typename std::vector<Tp>::const_iterator pos, size_t count, const Tp& value) {
        return data.insert(pos, count, value);
    }
    template <typename InputIt>
    typename std::vector<Tp>::iterator insert(typename std::vector<Tp>::const_iterator pos, InputIt first, InputIt last) {
        return data.insert(pos, first, last);
    }
    typename std::vector<Tp>::iterator insert(typename std::vector<Tp>::const_iterator pos, std::initializer_list<Tp> init) {
        return data.insert(pos, init);
    }

    typename std::vector<Tp>::iterator erase(typename std::vector<Tp>::const_iterator pos) {
        return data.erase(pos);
    }
    typename std::vector<Tp>::iterator erase(typename std::vector<Tp>::const_iterator first, typename std::vector<Tp>::const_iterator last) {
        return data.erase(first, last);
    }

    void push_back(const Tp& value) {
        data.push_back(value);
    }
    void push_back(Tp&& value) {
        data.push_back(std::move(value));
    }

    Tp pop_back() {
        if (empty()) {
            throw std::out_of_range("vec::pop_back(): pop from empty vec");
        }
        Tp value = std::move(data.back());
        data.pop_back();
        return value;
    }

    void resize(size_t count) {
        data.resize(count);
    }
    void resize(size_t count, const Tp& value) {
        data.resize(count, value);
    }

    void swap(vec& other) noexcept {
        data.swap(other.data);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        data.emplace_back(std::forward<Args>(args)...);
    }

    template <typename... Args>
    typename std::vector<Tp>::iterator emplace(typename std::vector<Tp>::const_iterator pos, Args&&... args) {
        return data.emplace(pos, std::forward<Args>(args)...);
    }


    vec& operator+=(const vec& rhs) {
        if (size() != rhs.size()) {
            throw std::runtime_error("vec::operator+=: sizes do not match for element-wise addition.");
        }
        for (size_t i = 0; i < size(); ++i) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    vec& operator+=(const Tp& scalar) {
        for (size_t i = 0; i < size(); ++i) {
            data[i] += scalar;
        }
        return *this;
    }

    vec& operator*=(const Tp& scalar) {
        for (size_t i = 0; i < size(); ++i) {
            data[i] *= scalar;
        }
        return *this;
    }


    friend bool operator==(const vec& lhs, const vec& rhs) {
        return lhs.data == rhs.data;
    }
    friend bool operator!=(const vec& lhs, const vec& rhs) {
        return !(lhs == rhs);
    }

    friend vec operator+(const vec& lhs, const vec& rhs) {
        vec result = lhs;
        result += rhs;
        return result;
    }

    friend vec operator+(const vec& lhs, const Tp& scalar) {
        vec result = lhs;
        result += scalar;
        return result;
    }
    friend vec operator+(const Tp& scalar, const vec& rhs) {
        vec result = rhs;
        result += scalar;
        return result;
    }

    friend vec operator*(const vec& lhs, const Tp& scalar) {
        vec result = lhs;
        result *= scalar;
        return result;
    }
    friend vec operator*(const Tp& scalar, const vec& rhs) {
        vec result = rhs;
        result *= scalar;
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const vec& v) {
        for (size_t i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i < v.size() - 1) {
                os << " ";
            }
        }
        return os;
    }

private:
    std::vector<Tp> data;

};
