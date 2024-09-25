#ifndef OBLIVION_ORDERED_LIST
#define OBLIVION_ORDERED_LIST

#include <cstddef>
#include <optional>
#include <vector>

namespace oblivion {

class OrderedList {
private:
  std::vector<std::optional<int>> v;
  size_t block_size;
  unsigned int tree_height;

  struct Block {
    size_t begin;
    size_t end;
    unsigned int depth;

    void extend_left();
    void extend_right(const size_t &max_index);
  };

  unsigned long count_elements(const Block &blk) const;
  double upper_limit(const Block &blk) const;
  double lower_limit(const Block &blk) const;
  double density(const Block &blk) const;

public:
  OrderedList();
  size_t size() const;
  // void include(int x);
  // void remove(int x);
  // int successor(int x) const;
  //
  // TODO:
  // - double and shrink vector in meters of blocks (mantain blocks of size
  // O(lg(n)) where n elements can fit)
  // - binary search method (required to include, remove and successor)
};
} // namespace oblivion

#endif
