#include <random>
#include <vector>

#include "input_generators.h"

#include "srt.h"
#include "absl/container/flat_hash_set.h"
#include <boost/container/flat_set.hpp>

#include "benchmark/benchmark.h"

namespace {

using value_type = int;

void problem_sizes(benchmark::internal::Benchmark* bench) {
  bench->Args({100});
  bench->Args({1000});
  bench->Args({10000});
  bench->Args({100000});
}

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

template <typename Container>
void do_benchmark(benchmark::State& state) {
  int range_size = state.range(0);
  std::vector<value_type> v = bencmarking_tools::generate_random_vector_unique_elements<int>(range_size);
  Container c(v.begin(), v.end());

  for(auto _ : state) {
    auto copy = c;
    erase_if(copy, [](int x) { return x & 3; });
    benchmark::DoNotOptimize(copy);
  }
}

}  // namespace

using Srt = srt::flat_set<value_type>;
using AbslHash = absl::flat_hash_set<value_type>;
using Boost = boost::container::flat_set<value_type>;

BENCHMARK_TEMPLATE(do_benchmark, Srt)->Apply(problem_sizes);
