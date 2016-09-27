// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_RING_BUFFER_H_
#define SHINOBU_MODULE_CLIPBRD_RING_BUFFER_H_

#include <list>

template <class T>
class RingBuffer {
 public:
  RingBuffer() : RingBuffer(SIZE_MAX) {}
  explicit RingBuffer(size_t limit) : limit_(limit) {}

  RingBuffer(RingBuffer&& other) noexcept  // NOLINT(build/c++11)
      : limit_(other.limit_),
        list_(std::move(other.list_)) {}

  void push(T&& value) {  // NOLINT(build/c++11)
    list_.push_front(std::move(value));

    for (auto size = list_.size(); size > limit_; --size)
      list_.pop_back();
  }

  void pop() {
    if (!list_.empty())
      list_.pop_front();
  }

  void rotate(SSIZE_T count) {
    if (list_.empty())
      return;

    count %= list_.size();

    if (count > 0) {
      auto begin = list_.begin();
      auto end = begin;
      for (SSIZE_T i = 0; i < count; ++i)
        ++end;

      std::list<T> erased;
      erased.splice(erased.begin(), list_, begin, end);
      list_.splice(list_.end(), erased);
    } else if (count < 0) {
      auto end = list_.end();
      auto begin = end;
      for (auto i = count; i < 0; ++i)
        --begin;

      std::list<T> erased;
      erased.splice(erased.begin(), list_, begin, end);
      list_.splice(list_.begin(), erased);
    }
  }

  auto& front() {
    return list_.front();
  }

  auto& front() const {
    return list_.front();
  }

  bool empty() const {
    return list_.empty();
  }

  size_t size() const {
    return list_.size();
  }

  size_t limit() const {
    return limit_;
  }

  void set_limit(size_t new_limit) {
    limit_ = new_limit;

    for (auto size = list_.size(); size > limit_; --size)
      list_.pop_back();
  }

  auto begin() {
    return list_.begin();
  }

  auto end() {
    return list_.end();
  }

  auto begin() const {
    return list_.begin();
  }

  auto end() const {
    return list_.end();
  }

 private:
  size_t limit_;
  std::list<T> list_;

  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
};

#endif  // SHINOBU_MODULE_CLIPBRD_RING_BUFFER_H_
