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

template <typename Contaier>
void range_construction(benchmark::State& state) {
  std::vector<value_type> v = bencmarking_tools::generate_random_vector_unique_elements<int>(state.range(0));
  auto every10thPerecntile = bencmarking_tools::every_10th_percentile_N(v.begin(), v.size());

  Contaier c(v.begin(), v.end());

  for(auto _ : state) {
    for (auto e : every10thPerecntile) {
       benchmark::DoNotOptimize(c.count(*e));
    }
  }
}

}  // namespace

using Srt = srt::flat_set<value_type>;
using AbslHash = absl::flat_hash_set<value_type>;
using Boost = boost::container::flat_set<value_type>;

BENCHMARK_TEMPLATE(range_construction, Srt)->Apply(problem_sizes);
