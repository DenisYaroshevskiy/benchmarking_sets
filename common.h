#ifndef BENCHMARKING_SETS_COMMON_H
#define BENCHMARKING_SETS_COMMON_H

#include "input_generators.h"

#include <boost/container/flat_set.hpp>
#include <set>
#include <unordered_set>
#include "absl/container/flat_hash_set.h"
#include "srt.h"

#include "benchmark/benchmark.h"

namespace benchmarking_sets {

using value_type = int;

inline void container_sizes(benchmark::internal::Benchmark* bench) {
  for (size_t size = 1 << 6; size < (1 << 18); size <<= 1) {
    // -15 to avoid powers of 2 which might have weird consequences.
    bench->Args({static_cast<int>(size) - 15});
  }
}

using Std = std::set<value_type>;
using StdUnordered = std::unordered_set<value_type>;
using Srt = srt::flat_set<value_type>;
using AbslHash = absl::flat_hash_set<value_type>;
using Boost = boost::container::flat_set<value_type>;

}  // namespace benchmarking_sets

#endif  // BENCHMARKING_SETS_COMMON_H