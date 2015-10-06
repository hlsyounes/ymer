// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// A vector class that takes ownership of its elements.

#ifndef UNIQUE_PTR_VECTOR_H_
#define UNIQUE_PTR_VECTOR_H_

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class UniquePtrVector {
 public:
  using size_type = typename std::vector<std::unique_ptr<T>>::size_type;

  class Iterator : public std::iterator<std::input_iterator_tag, T> {
   public:
    bool operator==(const Iterator& rhs) const { return i_ == rhs.i_; }
    bool operator!=(const Iterator& rhs) const { return i_ != rhs.i_; }
    T& operator*() const { return **i_; }
    Iterator& operator++() {
      ++i_;
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp(*this);
      ++i_;
      return tmp;
    }
    typename std::vector<std::unique_ptr<T>>::difference_type operator-(
        const Iterator& rhs) const {
      return i_ - rhs.i_;
    }

   private:
    explicit Iterator(
        typename std::vector<std::unique_ptr<T>>::const_iterator i)
        : i_(i) {}

    typename std::vector<std::unique_ptr<T>>::const_iterator i_;

    friend class UniquePtrVector;
  };

  UniquePtrVector() {}
  template <typename InputIterator,
            typename = typename std::enable_if<std::is_object<
                typename InputIterator::value_type>::value>::type>
  explicit UniquePtrVector(InputIterator first, InputIterator last) {
    for (InputIterator i = first; i != last; ++i) {
      elements_.push_back(std::move(*i));
    }
  }
  template <typename... Args>
  explicit UniquePtrVector(Args&&... args) {
    Init(std::forward<Args>(args)...);
  }

  size_type size() const { return elements_.size(); }
  bool empty() const { return elements_.empty(); }
  Iterator begin() const { return Iterator(elements_.begin()); }
  Iterator end() const { return Iterator(elements_.end()); }
  T& operator[](int i) const { return *elements_[i]; }
  T& back() const { return *elements_.back(); }
  void push_back(std::unique_ptr<T>&& element) {
    elements_.push_back(std::move(element));
  }
  void clear() { elements_.clear(); }

 private:
  void Init() {}
  template <typename... Args>
  void Init(std::unique_ptr<T>&& element, Args&&... args) {
    push_back(std::move(element));
    Init(std::forward<Args>(args)...);
  }

  std::vector<std::unique_ptr<T>> elements_;
};

#endif  // UNIQUE_PTR_VECTOR_H_
