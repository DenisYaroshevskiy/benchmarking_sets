#ifndef INPUT_GENERATORS
#define INPUT_GENERATORS

#include <algorithm>
#include <map>
#include <random>
#include <vector>

namespace bencmarking_tools {

template <typename Key, typename Value, typename Factory>
auto memoized_input(Factory f) {
    return [cache = std::map<Key, Value>{}, f](const Key& k) mutable {
        if (auto it = cache.find(k); it != cache.end()) {
            return it->second;
        }
        auto& r = cache[k];
        r = f(k);
        return r;
    };
}

inline auto& random_engine() {
    static std::mt19937 g;
    return g;
}

template <typename T>
std::vector<T> generate_random_vector(int size) {
  static auto op = memoized_input<int, std::vector<T>> (
    [](int size) {
       std::uniform_int_distribution<T> dis(1, size * 100);

       std::vector<T> v(static_cast<size_t>(size));
       std::generate(v.begin(), v.end(), [&] { return dis(random_engine()); });

       return v;
    }
  );

  return op(size);
}

}  // namespace bencmarking_tools

#endif  // INPUT_GENERATORS