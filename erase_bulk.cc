#include "common.h"

namespace benchmarking_sets {
namespace {

template <typename Container, typename Op>
void erase_remove_if(Container& c, Op op) {
  c.erase(
    std::remove_if(c.begin(), c.end(), op),
    c.end()
  );
}

template <typename T, typename Op>
void erase_if(boost::container::flat_set<T>& fs, Op op) {
  erase_remove_if(fs, op);
}

template <typename T, typename Op>
void erase_if(absl::flat_hash_set<T>& fs, Op op) {
  for (auto f = fs.begin(); f != fs.end(); ++f) {
    if (op(*f)) {
      fs.erase(f);
    }
  }
}

template <typename T, typename Op>
void erase_if(std::set<T>& s, Op op) {
  for (auto f = s.begin(); f != s.end();) {
    if (op(*f)) {
      f = s.erase(f);
    } else {
      ++f;
    }
  }
}

template <typename T, typename Op>
void erase_if(std::unordered_set<T>& s, Op op) {
  for (auto f = s.begin(); f != s.end();) {
    if (op(*f)) {
      f = s.erase(f);
    } else {
      ++f;
    }
  }
}

template <typename Container>
void erase_bulk(benchmark::State& state) {
  int range_size = state.range(0);
  std::vector<value_type> v = generate_random_vector_unique_elements<value_type>(range_size);
  Container c(v.begin(), v.end());

  for(auto _ : state) {
    auto copy = c;
    /// erase_if(copy, [](int x) { return x & 3; });
    benchmark::DoNotOptimize(copy);
  }
}

BENCHMARK_TEMPLATE(erase_bulk, Boost)->Apply(container_sizes);

}  // namespace
}  // namespace benchmarking_sets