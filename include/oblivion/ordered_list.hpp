#ifndef OBLIVION_ORDERED_LIST
#define OBLIVION_ORDERED_LIST

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
    size_t size;
    unsigned int depth;

    Block parent() const;
    Block sibling() const;
  };

  unsigned long count_elements(const Block &blk) const;
  double upper_limit(const Block &blk) const;
  double lower_limit(const Block &blk) const;
  double density(const Block &blk) const;

  std::optional<size_t> binary_search(const int &x, const size_t &begin,
                                      const size_t &end) const;
  std::optional<size_t> suc_idx(const int &x, const size_t &begin,
                                const size_t &end) const;

  void table_doubling();
  void table_halving();
  void distribute(const Block &blk);

public:
  OrderedList();
  size_t size() const;
  // void include(int x);
  // void erase(int x);
  std::optional<int> successor(const int &x) const;

  // TODO:
  // - implement functions (expand and shrink) to adjust the blocks to the size
  // to O(lg(n)). Consider using block_size = floor(lg(n)) + 1.
  // - distribute the elements of o block when density is violated (inclusion
  // and remotion)
};
} // namespace oblivion

#endif
