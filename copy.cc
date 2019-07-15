#include "common.h"

namespace benchmarking_sets {
namespace {

template <typename Contaier>
void copy_bench(benchmark::State& state) {
  std::vector<value_type> v = generate_random_vector_unique_elements<value_type>(state.range(0));
  Contaier c(v.begin(), v.end());

  for(auto _ : state)
    benchmark::DoNotOptimize(Contaier(c));
}

BENCHMARK_TEMPLATE(copy_bench, Boost)->Apply(container_sizes);

}  // namespace
}  // namespace benchmarking_sets
