#ifndef TREE_HPP
#define TREE_HPP
#include <cstddef>
#include "treenode.hpp"
#include "treeiterator.hpp"

namespace piyavkin
{
  template< class Key, class T, class Compare = std::less< Key > >
  class Tree
  {
  public:
    using val_type = std::pair< const Key, T >;
    Tree():
      root_(nullptr),
      cmp_(Compare()),
      size_(0),
      before_min_(Key(), nullptr, nullptr, nullptr, T()),
      end_node_(Key(), nullptr, nullptr, nullptr, T())
    {}
    Tree(const Tree& rhs):
      Tree()
    {
      cmp_ = rhs.cmp_;
      TreeIterator< Key, T, Compare > it_right(rhs.root_);
      TreeIterator< Key, T, Compare > it_left(rhs.root_);
      while (it_left.node_ != std::addressof(rhs.before_min_) && it_right.node_ != std::addressof(rhs.end_node_))
      {
        insert(std::make_pair< Key, T >(std::move(it_right.node_->key_), std::move(it_right.node_->data_)));
        insert(std::make_pair< Key, T >(std::move(it_left.node_->key_), std::move(it_left.node_->data_)));
        --it_left;
        ++it_right;
      }
    }
    Tree< Key, T, Compare >& operator=(const Tree& rhs)
    {
      if (std::addressof(rhs) != this)
      {
        Tree< Key, T, Compare > temp(rhs);
        swap(temp);
      }
      return *this;
    }
    size_t size() const noexcept
    {
      return size_;
    }
    bool empty() const noexcept
    {
      return size_ == 0;
    }
    void swap(Tree< Key, T, Compare >& mp)
    {
      std::swap(root_, mp.root_);
      std::swap(cmp_, mp.cmp_);
      std::swap(size_, mp.size_);
      std::swap(before_min_, mp.before_min_);
      std::swap(end_node_, mp.end_node_);
    }
    std::pair< TreeIterator< Key, T, Compare >, bool > insert(const val_type& val)
    {
      detail::Node< Key, T >* node = new detail::Node< Key, T >(val.first, nullptr, nullptr, nullptr, val.second);
      detail::Node< Key, T >* curr_node = root_;
      detail::Node< Key, T >* parent_node = nullptr;
      while (curr_node != nullptr && (curr_node != std::addressof(end_node_) && curr_node != std::addressof(before_min_)))
      {
        if (cmp_(curr_node->key_, node->key_))
        {
          if (!curr_node->right_ || curr_node != std::addressof(end_node_))
          {
            parent_node = curr_node;
          }
          curr_node = curr_node->right_;
        }
        else
        {
          if (!curr_node->left_ || curr_node != std::addressof(end_node_))
          {
            parent_node = curr_node;
          }
          curr_node = curr_node->left_;
        }
      }
      if (!parent_node)
      {
        root_ = node;
        root_->left_ = std::addressof(before_min_);
        root_->right_ = std::addressof(end_node_);
        before_min_.parent_ = root_;
        end_node_.parent_ = root_;
      }
      else if (cmp_(parent_node->key_, node->key_))
      {
        if (parent_node->right_ == std::addressof(end_node_))
        {
          node->right_ = std::addressof(end_node_);
          end_node_.parent_ = node;
        }
        parent_node->right_ = node;
      }
      else if (cmp_(node->key_, parent_node->key_))
      {
        if (parent_node->left_ == std::addressof(before_min_))
        {
          node->left_ = std::addressof(before_min_);
          before_min_.parent_ = node;
        }
        parent_node->left_ = node;
      }
      else
      {
        delete node;
        return std::make_pair< TreeIterator< Key, T, Compare >, bool >(TreeIterator< Key, T, Compare >(parent_node), false);
      }
      node->parent_ = parent_node;
      ++size_;
      //splay();
      return std::make_pair< TreeIterator< Key, T, Compare >, bool >(TreeIterator< Key, T, Compare >(node), true);
    }
    TreeIterator< Key, T, Compare > begin() noexcept
    {
      return TreeIterator< Key, T, Compare >(before_min_.parent_);
    }
    TreeIterator< Key, T, Compare > end() noexcept
    {
      return TreeIterator< Key, T, Compare >(std::addressof(end_node_));
    }
    TreeIterator< Key, T, Compare > find(const Key& key)
    {
      detail::Node< Key, T >* curr_node = root_;
      while (curr_node != nullptr && (curr_node != std::addressof(end_node_) && curr_node != std::addressof(before_min_)))
      {
        if (cmp_(curr_node->key_, key))
        {
          curr_node = curr_node->right_;
        }
        else
        {
          if (!cmp_(key, curr_node->key_))
          {
            return TreeIterator< Key, T, Compare >(curr_node);
          }
          curr_node = curr_node->left_;
        }
      }
      return end();
    }
    T& operator[](const Key& key)
    {
      return (*((this->insert(std::make_pair(key, T()))).first));
    }
    T& at(const Key& key)
    {
      if (find(key) == end())
      {
        throw std::out_of_range("No data with this key");
      }
      return operator[](key);
    }
  private:
    detail::Node< Key, T >* root_;
    Compare cmp_;
    size_t size_;
    detail::Node< Key, T > before_min_;
    detail::Node< Key, T > end_node_;
  };
}
#endif