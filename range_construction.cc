#include "common.h"

namespace benchmarking_sets {
namespace {

template <typename Contaier>
void range_construction(benchmark::State& state) {
  std::vector<value_type> v = generate_random_vector<value_type>(state.range(0));

  for(auto _ : state)
    benchmark::DoNotOptimize(Contaier(v.begin(), v.end()));
}

BENCHMARK_TEMPLATE(range_construction, Boost)->Apply(container_sizes);

}  // namespace
}  // namespace benchmarking_sets
