#ifndef INPUT_GENERATORS
#define INPUT_GENERATORS

#include <algorithm>
#include <map>
#include <random>
#include <set>
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

template <typename T>
std::vector<T> generate_random_vector_unique_elements(int size) {
  static auto op = memoized_input<int, std::vector<T>> (
    [](int size) {
       std::uniform_int_distribution<T> dis(1, size * 100);
       std::set<T> limit;
       std::vector<T> v;
       while (static_cast<int>(limit.size()) < size) {
         T elem = dis(random_engine());
         if (limit.insert(elem).second) {
           v.push_back(elem);
         }
       }

       return v;
    }
  );

  return op(size);
}

template <typename I, typename N>
std::array<I, 10> every_10th_percentile_N(I first, N n) {
  N step = n / 10;
  std::array<I, 10> res;

  for (size_t i = 0; i < 10; ++i) {
    res[i] = first;
    std::advance(first, step);
  }

  return res;
}

}  // namespace bencmarking_tools

#endif  // INPUT_GENERATORS