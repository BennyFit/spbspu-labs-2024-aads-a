#ifndef INPUT_OUTPUT_COLLECTION_HPP
#define INPUT_OUTPUT_COLLECTION_HPP

#include <string>
#include <iosfwd>
#include "red_black_tree.hpp"

namespace erohin
{
  using dictionary = RedBlackTree< std::string, size_t >;
  using collection = RedBlackTree< std::string, dictionary >;

  void inputDictionary(dictionary & dict, std::istream & input);
  void outputDictionary(const dictionary & dict, std::ostream & output);
  void inputCollection(collection & dict_context, std::istream & input);
  void outputCollection(const collection & dict_context, std::ostream & output);
}

#endif
