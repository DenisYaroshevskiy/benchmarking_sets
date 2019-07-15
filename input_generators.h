#ifndef INPUT_GENERATORS
#define INPUT_GENERATORS

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace benchmarking_sets {

struct fake_url {
  std::string data;

  fake_url() = default;

  explicit fake_url(int seed) : data("https://" + std::to_string(seed) + ".com") {}

  template <typename H>
  friend H AbslHashValue(H h, const fake_url& x) {
    return H::combine(std::move(h), x.data);
  }

  friend bool operator==(const fake_url& x, const fake_url& y) {
    return x.data == y.data;
  }

  friend bool operator<(const fake_url& x, const fake_url& y) {
    return x.data < y.data;
  }

  friend bool operator!=(const fake_url& x, const fake_url& y) {
    return !(x == y);
  }

  friend bool operator>(const fake_url& x, const fake_url& y) { return y < x; }

  friend bool operator<=(const fake_url& x, const fake_url& y) {
    return !(y < x);
  }

  friend bool operator>=(const fake_url& x, const fake_url& y) {
    return !(x < y);
  }
};

}  //  namespace benchmarking_sets

namespace std {

template <>
class hash<benchmarking_sets::fake_url> {
 public:
  size_t operator()(const benchmarking_sets::fake_url& x) const {
    return std::hash<std::string>()(x.data);
  }
};

}  // namespace std

namespace benchmarking_sets {

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

template <typename>
struct to_type_generator;

template <>
struct to_type_generator<int> {
  int operator()(int seed) const { return seed; }
};

template <>
struct to_type_generator<fake_url> {
  fake_url operator()(int seed) const { return fake_url{seed}; }
};

template <typename T>
std::vector<T> generate_random_vector(int size) {
  static auto op = memoized_input<int, std::vector<T>>([](int size) {
    std::uniform_int_distribution<> dis(1, size * 100);
    to_type_generator<T> to_type;
    std::vector<T> v(static_cast<size_t>(size));
    std::generate(v.begin(), v.end(),
                  [&] { return to_type(dis(random_engine())); });

    return v;
  });

  return op(size);
}

template <typename T>
std::vector<T> generate_random_vector_unique_elements(int size) {
  static auto op = memoized_input<int, std::vector<T>>([](int size) {
    std::uniform_int_distribution<> dis(1, size * 100);
    to_type_generator<T> to_type;
    std::set<T> limit;
    std::vector<T> v;
    while (static_cast<int>(limit.size()) < size) {
      T elem = to_type(dis(random_engine()));
      if (limit.insert(elem).second) {
        v.push_back(elem);
      }
    }

    return v;
  });

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

template <typename Container>
class trash_cache {
  std::vector<Container> copies_;
  int i = 0;
  const std::vector<int>* accessPattern = &trash_cache_access_pattern();

  static constexpr size_t sufficient_tashing_size = 3000;

  const std::vector<int>& trash_cache_access_pattern() {
    static std::vector<int> res = [] {
      std::vector<int> res(sufficient_tashing_size);
      std::iota(res.begin(), res.end(), 0);
      std::shuffle(res.begin(), res.end(), random_engine());
      return res;
    }();

    return res;
  }

 public:
  trash_cache(const Container& c) : copies_(sufficient_tashing_size, c) {}

  const Container& active() {
    if (i > static_cast<int>(sufficient_tashing_size)) {
      i = 0;
    }
    return copies_[(*accessPattern)[i++]];
  }
};

}  // namespace benchmarking_sets

#endif  // INPUT_GENERATORS