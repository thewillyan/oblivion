#include "oblivion/ordered_list.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace oblivion {

// constructors
OrderedList::OrderedList()
    : v{std::vector<std::optional<int>>()}, block_size{0}, tree_height{0},
      nitems{0} {}

// block
OrderedList::Block OrderedList::Block::parent() const {
  size_t blk_idx = begin / size;
  if (blk_idx % 2 == 0) {
    // current block is a left child
    return OrderedList::Block{begin, size * 2, depth - 1};
  } else {
    // current node is a right child
    return OrderedList::Block{begin - size, size * 2, depth - 1};
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
size_t OrderedList::size() const { return nitems; }

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
  if (tree_height == blk.depth) {
    return 1.0;
  }
  return 0.75 + 0.25 * static_cast<double>(blk.depth) /
                    static_cast<double>(tree_height);
}

double OrderedList::lower_limit(const Block &blk) const {
  if (tree_height == blk.depth) {
    return 0.25;
  }
  return 0.5 - 0.25 * static_cast<double>(blk.depth) /
                   static_cast<double>(tree_height);
}

double OrderedList::density(const Block &blk) const {
  return static_cast<double>(count_elements(blk)) /
         static_cast<double>(blk.size);
}

void OrderedList::fix_density(const OrderedList::Block &blk,
                              double new_density) {

  OrderedList::Block curr_blk = blk;
  if (new_density > upper_limit(blk)) {
    // find a block where the upper limit is not violated
    while (new_density > upper_limit(curr_blk) && curr_blk.depth != 0) {
      const OrderedList::Block sibling = curr_blk.sibling();
      double sibling_density = density(sibling);
      new_density += sibling_density;
      curr_blk = curr_blk.parent();
    }

    if (new_density > upper_limit(curr_blk)) {
      // root has violated the upper limit
      table_doubling();
      const OrderedList::Block root = {0, v.size(), 0};
      distribute(root, root.size);
    } else {
      // succesfully found a block where the upper limit is valid
      distribute(curr_blk, curr_blk.size);
    }
  } else if (new_density < lower_limit(blk)) {
    // find a block where the lower limit is not violated
    while (new_density < lower_limit(curr_blk) && curr_blk.depth != 0) {
      const OrderedList::Block sibling = curr_blk.sibling();
      double sibling_density = density(sibling);
      new_density += sibling_density;
      curr_blk = curr_blk.parent();
    }

    if (new_density < lower_limit(curr_blk)) {
      // root has violated the lower limit
      distribute(curr_blk, curr_blk.size / 2);
      table_halving();
    } else {
      // succesfully found a block where the lower limit is valid
      distribute(curr_blk, curr_blk.size);
    }
  }
}

std::optional<size_t> OrderedList::binary_search(const int &x,
                                                 const size_t &begin,
                                                 const size_t &end) const {
  size_t middle = (end + begin) / 2;
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
    for (long int i = static_cast<long int>(middle) - 1;
         i >= static_cast<long int>(begin); --i) {
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
  if (begin >= end || end > v.size()) {
    return std::nullopt; // Invalid range
  }

  // Use binary search to find the first element greater than x
  auto it = std::upper_bound(
      v.begin() + begin, v.begin() + end, std::optional<int>(x),
      [](const std::optional<int> &a, const std::optional<int> &b) {
        return a.has_value() && (b.has_value() ? *a <= *b : true);
      });

  // Skip over any std::nullopt values and find the index
  while (it != v.begin() + end && !it->has_value()) {
    ++it;
  }

  // Return the index or std::nullopt if none exists
  return (it != v.begin() + end && it->has_value())
             ? std::optional<size_t>(std::distance(v.begin(), it))
             : std::nullopt;
}

std::optional<int> OrderedList::successor(const int &x) const {
  std::optional<size_t> suc_i = suc_idx(x, 0, this->size());
  if (suc_i.has_value()) {
    return v[suc_i.value()].value();
  } else {
    return {};
  }
}

void OrderedList::table_doubling() {
  if (v.size() == 0) {
    v.resize(1, std::optional<int>{});
  } else {
    v.resize(v.size() * 2, std::optional<int>{});
    ++tree_height;
  }
}

void OrderedList::table_halving() {
  if (v.size() == 0) {
    throw std::runtime_error("Error: can't shirk a vector with size 0.");
  } else {
    v.resize(v.size() / 2);
    if (tree_height > 0) {
      --tree_height;
    }
  }
}

void OrderedList::distribute(const Block &blk, const size_t &sz) {
  size_t n_elements = (blk.depth == 0) ? nitems : this->count_elements(blk);
  size_t step = (n_elements > 2) ? (sz - 1) / (n_elements - 1) : (sz - 1);

  std::vector<std::optional<int>> tmp_vector(v.size(), std::nullopt);
  size_t target_index = blk.begin;

  for (size_t k = blk.begin; k < blk.begin + blk.size && target_index < sz;
       ++k) {
    if (v[k].has_value()) {
      tmp_vector[target_index] = std::move(v[k]);
      target_index += step;
    }
  }

  for (size_t k = sz; k < v.size(); ++k) {
    tmp_vector[k] = std::move(v[k]);
  }

  v = std::move(tmp_vector);
}

void OrderedList::push_way(const size_t &i, const Block &blk) {
  if (i < blk.begin && i >= blk.begin + blk.size) {
    throw std::runtime_error("Error: can't push_way for a index that is not "
                             "inside the given block.");
  }

  std::optional<size_t> empty_slot;

  // i -> end
  for (size_t k = i; k < blk.begin + blk.size; ++k) {
    if (!v[k].has_value()) {
      empty_slot = k;
      break;
    }
  }

  if (!empty_slot.has_value()) {
    // i -> begin
    for (long int k = static_cast<long int>(i) - 1;
         k >= static_cast<long int>(blk.begin); --k) {
      if (!v[k].has_value()) {
        empty_slot = k;
      }
    }
  }

  // for now we assume that we find a empty_slot. That's true beacause the
  // desity of the block is okay before this call.
  if (*empty_slot > i) {
    for (size_t k = *empty_slot; k > i; --k) {
      std::swap(v[k], v[k - 1]);
    }
  } else if (*empty_slot < i) {
    for (size_t k = *empty_slot; k < i; ++k) {
      std::swap(v[k], v[k + 1]);
    }
  }
}

// Expand block size by one.
void OrderedList::expand_blocks() {
  if (block_size == 0) {
    v.resize(1, std::optional<int>{});
  } else {
    size_t nblocks = v.size() / block_size;
    size_t new_vec_size = nblocks * (block_size + 1);
    v.resize(new_vec_size, std::optional<int>{});
  }
  ++block_size;
}

// Shrik the block size by one.
void OrderedList::shrink_blocks() {
  if (block_size == 0) {
    throw std::runtime_error("Error: can't shrink a zero sized block.");
  }
  size_t nblocks = v.size() / block_size;
  size_t new_vec_size = nblocks * (block_size - 1);
  v.resize(new_vec_size);
  --block_size;
}

OrderedList::Iterator::Iterator(
    std::vector<std::optional<int>>::const_iterator start,
    std::vector<std::optional<int>>::const_iterator end)
    : current{start}, end{end} {
  advance();
}

void OrderedList::Iterator::advance() {
  while (current != end && !current->has_value()) {
    ++current;
  }
}

int OrderedList::Iterator::operator*() const {
  return current->value(); // safe since advance() has been called
}

OrderedList::Iterator &OrderedList::Iterator::operator++() {
  ++current;
  advance();
  return *this;
}

bool OrderedList::Iterator::operator!=(const Iterator &other) const {
  return current != other.current;
}

OrderedList::Iterator OrderedList::begin() const {
  return OrderedList::Iterator(v.begin(), v.end());
}

OrderedList::Iterator OrderedList::end() const {
  return OrderedList::Iterator(v.end(), v.end());
}

std::ostream &operator<<(std::ostream &os, const OrderedList &obj) {
  if (obj.size() == 0) {
    return os;
  }

  OrderedList::Iterator it = obj.begin();
  os << *it;
  ++it;
  for (; it != obj.end(); ++it) {
    os << ' ' << *it;
  }
  return os;
}

void OrderedList::include(int x) {
  // adjust the block size to keep it in O(lg(n)).
  size_t new_block_size = static_cast<size_t>(std::log2(nitems + 1)) + 1;
  // should i expand the block only when the element is difacto included?
  if (new_block_size > block_size) {
    expand_blocks();
    const OrderedList::Block root{0, v.size(), 0};
    distribute(root, root.size);
    // after here at least one aditional space is placed in the vector,
    // therefore the element gonna fit in it (table_doubling will not be
    // called).
  }

  // try insert
  std::optional<size_t> suc_index = suc_idx(x, 0, v.size());
  OrderedList::Block inc_block;
  size_t insert_pos;
  if (suc_index.has_value()) {
    size_t suc_blk_idx = *suc_index / block_size;
    OrderedList::Block suc_blk{suc_blk_idx * block_size, block_size,
                               tree_height};

    size_t block_items = count_elements(suc_blk);
    double new_density = static_cast<double>(block_items + 1) / suc_blk.size;

    if (new_density > upper_limit(suc_blk)) {
      fix_density(suc_blk, new_density);
      // redefine sucessor
      suc_index = suc_idx(x, 0, v.size());
      suc_blk_idx = *suc_index / block_size;
      suc_blk = {suc_blk_idx * block_size, block_size, tree_height};
    }
    inc_block = suc_blk;

    if (*suc_index != suc_blk.begin) {
      insert_pos = *suc_index - 1;
    } else {
      insert_pos = suc_blk.begin;
    }
  } else {
    // there is no succesor, therefore x is maximum, so we put x in the
    // end of the vector.
    size_t last_blk_idx = (v.size() / block_size) - 1;
    OrderedList::Block last_blk{last_blk_idx * block_size, block_size,
                                tree_height};

    size_t block_items = count_elements(last_blk);
    double new_density = static_cast<double>(block_items + 1) / last_blk.size;

    if (new_density > upper_limit(last_blk)) {
      fix_density(last_blk, new_density);
      last_blk_idx = (v.size() / block_size) - 1;
      last_blk = {last_blk_idx * block_size, block_size, tree_height};
    }
    inc_block = last_blk;
    insert_pos = v.size() - 1;
  }
  push_way(insert_pos, inc_block);
  v[insert_pos] = x;

  ++nitems; // if, AND ONLY IF, everything happens fine.
}

void OrderedList::erase(int x) {
  std::optional<size_t> x_idx = binary_search(x, 0, v.size());
  if (!x_idx.has_value()) {
    return;
  }

  // erase item
  v[*x_idx] = std::nullopt;

  // adjust the block size to keep it in O(lg(n)).
  size_t new_block_size = static_cast<size_t>(std::log2(nitems - 1)) + 1;
  if (new_block_size < block_size) {
    const OrderedList::Block root = {0, v.size(), 0};
    size_t n_blocks = v.size() / block_size;
    distribute(root, root.size - n_blocks);
    shrink_blocks();
  }

  size_t blk_idx = *x_idx / block_size;
  OrderedList::Block blk{blk_idx * block_size, block_size, tree_height};

  size_t block_items = count_elements(blk);
  double new_density = static_cast<double>(block_items - 1) / blk.size;

  if (new_density < lower_limit(blk)) {
    fix_density(blk, new_density);
  }
  --nitems;
}
} // namespace oblivion
