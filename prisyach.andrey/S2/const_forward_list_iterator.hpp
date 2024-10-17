#ifndef CONST_FORWARD_LIST_ITERATOR_HPP
#define CONST_FORWARD_LIST_ITERATOR_HPP

#include <iterator>
#include <forward_list>
#include <cassert>
#include "node.hpp"

namespace prisyach
{
  template < typename T >
  class ForwardList;

  template < typename T >
  struct ForwardListIterator;

  template < typename T >
  struct ConstForwardListIterator: public std::iterator< std::forward_iterator_tag, T >
  {
    friend class ForwardList< T >;
    friend struct ForwardListIterator< T >;

  public:
    using this_t = ConstForwardListIterator< T >;
    ConstForwardListIterator():
      node_(nullptr)
    {}
    ConstForwardListIterator(Node< T > * node):
      node_(node)
    {}
    ~ConstForwardListIterator() = default;

    ConstForwardListIterator(const this_t &) = default;
    ConstForwardListIterator(const ForwardListIterator< T > & other):
      node_(other.node_)
    {}
    this_t & operator=(const this_t &) = default;
    this_t & operator++()
    {
      assert(node_ != nullptr);
      node_ = node_->next_;
      return *this;
    }
    this_t operator++(int)
    {
      assert(node_ != nullptr);
      this_t result(*this);
      ++(*this);
      return result;
    }
    const T & operator*() const
    {
      assert(node_ != nullptr);
      return node_->data_;
    }
    const T * operator->() const
    {
      assert(node_ != nullptr);
      return std::addressof(node_->data_);
    }

    bool operator==(const this_t & rhs) const
    {
      return node_ == rhs.node_;
    }
    bool operator!=(const this_t & rhs) const
    {
      return !(rhs == *this);
    }

  private:
    Node< T > * node_;
  };
}

#endif
