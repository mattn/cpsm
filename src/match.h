// cpsm - fuzzy path matcher
// Copyright (C) 2015 Jamie Liu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CPSM_MATCH_H_
#define CPSM_MATCH_H_

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "str_util.h"

namespace cpsm {

struct MatchBase {
  // Sum of path component indexes (counting from the right) for all item path
  // components containing at least one match, a lower value of which should
  // indicate higher confidence that the matches are correct.
  CharCount part_index_sum = 0;

  // The number of path components that must be traversed between the query and
  // item paths.
  CharCount path_distance = 0;

  // The number of consecutive unmatched characters at the end of the rightmost
  // path component of the item. Since it's easier to add characters at the end
  // of a query (vs. in the middle) to refine a match, lower values are weakly
  // preferred.
  CharCount unmatched_len = 0;

  // The number of consecutive matched characters at the beginning of the
  // "words" in rightmost path component of the item.
  CharCount word_prefix_len = 0;

  // True iff the first character of the query matched the first character of
  // the rightmost path component in the item.
  enum class PrefixMatch : std::uint8_t {
    // The first character of the query did not match the first character of
    // the rightmost path component in the item.
    NONE,
    // The first character of the query matched the first character of the
    // rightmost path component in the item, but the same is not true of all
    // remaining characters.
    PARTIAL,
    // The query matched all of the first characters of the rightmost path
    // component of the item.
    FULL,
  };
  PrefixMatch prefix_match = PrefixMatch::NONE;

  std::string debug_string() const {
    return str_cat("part_index_sum=", part_index_sum, ", path_distance=",
                   path_distance, ", unmatched_len=", unmatched_len,
                   ", word_prefix_len=", word_prefix_len, ", prefix_match=",
                   int(prefix_match));
  }
};

template <typename T>
struct Match : public MatchBase {
  T item;

  Match() = default;
  explicit Match(T item) : item(std::move(item)) {}
};

// Returns true if lhs is a *better* match than rhs (so that sorting in
// ascending order, as is default for std::sort, sorts in *descending* match
// quality).
template <typename T>
bool operator<(Match<T> const& lhs, Match<T> const& rhs) {
  if (lhs.prefix_match != rhs.prefix_match) {
    return lhs.prefix_match > rhs.prefix_match;
  }
  if (lhs.word_prefix_len != rhs.word_prefix_len) {
    return lhs.word_prefix_len > rhs.word_prefix_len;
  }
  if (lhs.part_index_sum != rhs.part_index_sum) {
    return lhs.part_index_sum < rhs.part_index_sum;
  }
  if (lhs.path_distance != rhs.path_distance) {
    return lhs.path_distance < rhs.path_distance;
  }
  if (lhs.unmatched_len != rhs.unmatched_len) {
    return lhs.unmatched_len < rhs.unmatched_len;
  }
  // Sort lexicographically on the item if all else fails.
  return lhs.item < rhs.item;
}

template <typename T>
void sort_limit(std::vector<T>& vec,
                typename std::vector<T>::size_type const limit = 0) {
  if (limit && limit < vec.size()) {
    std::partial_sort(vec.begin(), vec.begin() + limit, vec.end());
    vec.resize(limit);
  } else {
    std::sort(vec.begin(), vec.end());
  }
}

}  // namespace cpsm

#endif /* CPSM_MATCH_H_ */
