// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_PTR_CIRCULAR_BUFFER_H_
#define SHINOBU_MODULE_CLIPBRD_PTR_CIRCULAR_BUFFER_H_

#include <boost/ptr_container/ptr_circular_buffer.hpp>

template <class T>
class ptr_circular_buffer : public boost::ptr_circular_buffer<T> {
 public:
  void Push(value_type entry) {
    push_front(entry);
  }

  void Pop() {
    if (!empty())
      pop_front();
  }

  void Rotate(int distance) {
    if (size() < 2)
      return;

    auto offset = distance % static_cast<int>(size());
    if (0 < distance)
      rotate(begin() + offset);
    else if (distance < 0)
      rotate(end() + offset);
  }
};

#endif  // SHINOBU_MODULE_CLIPBRD_PTR_CIRCULAR_BUFFER_H_
