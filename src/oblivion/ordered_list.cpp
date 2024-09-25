#include "oblivion/ordered_list.hpp"
#include <math.h>

using namespace oblivion;

// constructors
OrderedList::OrderedList()
    : v{std::vector<std::optional<int>>()}, block_size{0}, tree_height{0} {}

// block
OrderedList::Block OrderedList::Block::parent() const {
  size_t blk_idx = begin / size;
  if (blk_idx % 2 == 0) {
    // current block is a left child
    return OrderedList::Block{begin, size * 2, depth + 1};
  } else {
    // current node is a right child
    return OrderedList::Block{begin - size, size * 2, depth + 1};
  }
}

OrderedList::Block OrderedList::Block::sibling() const {
  size_t blk_idx = begin / size;
  if (blk_idx % 2 == 0) {
    // current block is a left child
    return OrderedList::Block{begin + size, size, depth};
  } else {
    // current node is a right child
    return OrderedList::Block{begin - size, size, depth};
  }
}

// methods
size_t OrderedList::size() const { return v.size(); }

unsigned long OrderedList::count_elements(const Block &blk) const {
  unsigned long count = 0;
  for (size_t i = blk.begin; i < blk.begin + blk.size; ++i) {
    if (v[i].has_value()) {
      ++count;
    }
  }
  return count;
}

double OrderedList::upper_limit(const Block &blk) const {
  return 0.75 + 0.25 * static_cast<double>(blk.depth) /
                    static_cast<double>(tree_height);
}

double OrderedList::lower_limit(const Block &blk) const {
  return 0.5 - 0.25 * static_cast<double>(blk.depth) /
                   static_cast<double>(tree_height);
}

double OrderedList::density(const Block &blk) const {
  return static_cast<double>(count_elements(blk)) /
         static_cast<double>(blk.size);
}

std::optional<size_t> OrderedList::binary_search(const int &x,
                                                 const size_t &begin,
                                                 const size_t &end) const {
  size_t middle = (end - begin) / 2;
  std::optional<int> y = v[middle];
  size_t y_idx = middle;

  // from middle -> end
  if (!y.has_value()) {
    for (size_t i = middle + 1; i < end; ++i) {
      if (v[i].has_value()) {
        y = v[i];
        y_idx = i;
        break;
      }
    }
  }

  // from middle -> begin
  if (!y.has_value()) {
    for (size_t i = middle - 1; i >= begin; --i) {
      if (v[i].has_value()) {
        y = v[i];
        y_idx = i;
        break;
      }
    }
  }

  if (!y.has_value()) {
    return {};
  } else if (x == y.value()) {
    return y_idx;
  } else if (x > y.value()) {
    return binary_search(x, y_idx + 1, end);
  } else {
    return binary_search(x, begin, y_idx);
  }
}

std::optional<size_t> OrderedList::suc_idx(const int &x, const size_t &begin,
                                           const size_t &end) const {
  size_t middle = (end - begin) / 2;

  std::optional<int> fst_left{};
  size_t fst_left_idx = end; // start with a 'out of range' index

  std::optional<int> fst_right{};
  size_t fst_right_idx = end; // start with a 'out of range' index

  // scan from middle + 1 -> end to find fst_right
  for (size_t i = middle + 1; i < end; ++i) {
    if (v[i].has_value()) {
      fst_right = v[i];
      fst_right_idx = i;
      break;
    }
  }

  // scan from middle -> begin to find fst_left
  for (size_t i = middle; i >= begin; --i) {
    if (v[i].has_value()) {
      fst_left = v[i];
      fst_left_idx = i;
      break;
    }
  }

  if (fst_left.has_value() && fst_right.has_value()) {
    if (x >= fst_right.value()) {
      return suc_idx(x, fst_right_idx, end);
    } else if (x < fst_left.value()) {
      return suc_idx(x, begin, fst_left_idx + 1);
    } else {
      // fst_left <= x < fst_right
      return fst_right_idx;
    }
  } else if (fst_left.has_value() && x < fst_left.value()) {
    return fst_left_idx;
  } else if (fst_right.has_value() && x < fst_right.value()) {
    return fst_right_idx;
  } else {
    return {};
  }
}

std::optional<int> OrderedList::successor(const int &x) const {
  std::optional<size_t> suc_i = suc_idx(x, 0, this->size());
  if (suc_i.has_value()) {
    return v[suc_i.value()].value();
  } else {
    return {};
  }
}
