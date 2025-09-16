#pragma once
#include <atomic>
#include <cstddef>
#include <vector>

namespace mm {

template <typename T>
class SpscQueue {
 public:
  explicit SpscQueue(size_t capacity) : buf_(capacity + 1), cap_(capacity + 1) {}

  bool push(const T& v) {
    auto next = (head_ + 1) % cap_;
    if (next == tail_.load(std::memory_order_acquire)) return false;
    buf_[head_] = v;
    head_ = next;
    return true;
  }

  bool pop(T& out) {
    auto tail = tail_.load(std::memory_order_acquire);
    if (tail == head_) return false;
    out = buf_[tail];
    tail_.store((tail + 1) % cap_, std::memory_order_release);
    return true;
  }

  bool empty() const { return tail_.load(std::memory_order_acquire) == head_; }

 private:
  std::vector<T> buf_;
  size_t cap_;
  size_t head_{0};
  std::atomic<size_t> tail_{0};
};

} // namespace mm
