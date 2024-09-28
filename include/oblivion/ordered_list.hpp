#ifndef OBLIVION_ORDERED_LIST
#define OBLIVION_ORDERED_LIST

#include <optional>
#include <ostream>
#include <vector>

namespace oblivion {

class OrderedList {
private:
  std::vector<std::optional<int>> v;
  size_t block_size;
  unsigned int tree_height;
  size_t nitems;

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

  void fix_density(const Block &blk, double new_density);

  std::optional<size_t> binary_search(const int &x, const size_t &begin,
                                      const size_t &end) const;
  std::optional<size_t> suc_idx(const int &x, const size_t &begin,
                                const size_t &end) const;

  void table_doubling();
  void table_halving();

  // Expand block size by one.
  void expand_blocks();
  // Shrik the block size by one.
  void shrink_blocks();

  // Distribute the elements of an block in the first sz positions of its
  // interval.
  void distribute(const Block &blk, const size_t &sz);
  void push_way(const size_t &i, const Block &blk);

public:
  OrderedList();
  size_t size() const;
  void include(int x); // TODO!
  void erase(int x);   // TODO!
  std::optional<int> successor(const int &x) const;

  friend std::ostream &operator<<(std::ostream &os, const OrderedList &obj);

  // Iterator
  class Iterator {
  private:
    std::vector<std::optional<int>>::const_iterator current;
    std::vector<std::optional<int>>::const_iterator end;

    void advance();

  public:
    Iterator(std::vector<std::optional<int>>::const_iterator start,
             std::vector<std::optional<int>>::const_iterator end);

    int operator*() const;
    Iterator &operator++();
    bool operator!=(const Iterator &other) const;
  };

  Iterator begin() const;
  Iterator end() const;
};

} // namespace oblivion

#endif
