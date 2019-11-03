#pragma once

#include <iterator>
#include <string_view>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>

template <typename It>
class Range {
public:
  using ValueType = typename std::iterator_traits<It>::value_type;

  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

template <typename C>
auto AsRange(const C& container) {
  return Range{std::begin(container), std::end(container)};
}

template <typename It>
size_t ComputeUniqueItemsCount(Range<It> range) {
  return std::unordered_set<typename Range<It>::ValueType>{
      range.begin(), range.end()
  }.size();
}

template <typename K, typename V>
const V* GetValuePointer(const std::unordered_map<K, V>& map, const K& key) {
  if (auto it = map.find(key); it != end(map)) {
    return &it->second;
  } else {
    return nullptr;
  }
}

template <typename T>
bool operator==(const std::vector<T> &lhs, const std::vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (!(lhs[i] == rhs[i]))
      return false;
  }
  return true;
}

template <typename T>
bool operator==(const std::map<std::string, T> &lhs, const std::map<std::string, T> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  try {
    for (const auto &[lhs_key, lhs_value] : lhs) {
      if (!(rhs.at(lhs_key) == lhs_value))
        return false;
    }
  } catch (...) {
    return false;
  }
  return true;
}

