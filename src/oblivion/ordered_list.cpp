#include "oblivion/ordered_list.hpp"

using namespace oblivion;

// constructors
OrderedList::OrderedList()
    : v{std::vector<std::optional<int>>()}, block_size{0}, tree_height{0} {}

// block
void OrderedList::Block::extend_left() {
  size_t blk_size = end - begin;
  if (blk_size > begin) {
    begin = 0;
  } else {
    begin -= blk_size;
  }
}

void OrderedList::Block::extend_right(const size_t &max_index) {
  size_t blk_size = end - begin;
  end += blk_size;
  if (end > max_index) {
    end = max_index;
  }
}

// methods
size_t OrderedList::size() const { return v.size(); }

unsigned long OrderedList::count_elements(const Block &blk) const {
  unsigned long count = 0;
  for (size_t i = blk.begin; i < blk.end; ++i) {
    if (v[i].has_value()) {
      ++count;
    }
  }
  return count;
}

double OrderedList::upper_limit(const Block &blk) const {
  return 0.5 - 0.25 * static_cast<double>(blk.depth) /
                   static_cast<double>(tree_height);
}

double OrderedList::lower_limit(const Block &blk) const {
  return 0.75 + 0.25 * static_cast<double>(blk.depth) /
                    static_cast<double>(tree_height);
}

double OrderedList::density(const Block &blk) const {
  return static_cast<double>(count_elements(blk)) /
         static_cast<double>(blk.end - blk.begin);
}
