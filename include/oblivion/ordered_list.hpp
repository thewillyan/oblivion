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

  void expand_vector();
  void shrink_vector();

public:
  OrderedList();
  size_t size() const;
  // void include(int x);
  // void erase(int x);
  std::optional<int> successor(const int &x) const;

  // TODO:
  // - distribute the elements of o block when density is violated
  // - double and shrink vector in meters of blocks (mantain blocks of size
  // O(lg(n)) where n elements can fit)
  // - think if the way im recursevely dividing the blocks (maybe i should use
  // tree like operations where the bloks are nodes) (MAYBE NOT!!)
};
} // namespace oblivion

#endif
