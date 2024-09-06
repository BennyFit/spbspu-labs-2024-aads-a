#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include "freqList.hpp"

namespace isaychev
{
  using collection_t = BSTree< std::string, FreqList >;

  void make_freqlist(collection_t & col, std::istream & in);
  void delete_freqlist(collection_t & col, std::istream & in);
  void print(const collection_t & col, std::istream & in, std::ostream & out);
  void count(const collection_t & col, std::istream & in, std::ostream & out);
  void get_total(const collection_t & col, std::istream & in, std::ostream & out);
  void get_unique(const collection_t & col, std::istream & in, std::ostream & out);
  void print_extremes(const collection_t & col, const std::string & spec, std::istream & in, std::ostream & out);
  void merge(collection_t & col, std::istream & in);
  void get_names(const collection_t & col, std::ostream & out);
  void intersect(collection_t & col, std::istream & in);
}

#endif
