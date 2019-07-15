#include "common.h"

namespace benchmarking_sets {
namespace {

template <typename Container>
void lookup_speed_out_of_cache(benchmark::State& state) {
  std::vector<value_type> v = generate_random_vector_unique_elements<value_type>(state.range(0));
  auto every10thPerecntile = every_10th_percentile_N(v.begin(), v.size());

  trash_cache<Container> containers {{v.begin(), v.end()}};

  for(auto _ : state) {
    for (auto e : every10thPerecntile) {
       benchmark::DoNotOptimize(containers.active().count(*e));
    }
  }
}

BENCHMARK_TEMPLATE(lookup_speed_out_of_cache, Boost)->Apply(container_sizes);

}  // namespace
}  // namespace benchmarking_sets
