/*
 *  Copyright 2016 mogujie.com
 *
 *  Created on: 2015年7月10日
 *      Author: fanshao@mogujie.com
 */


#ifndef  FOUNDATION_WLKIT_BOUNDED_BLOCKING_QUEUE_H_
#define  FOUNDATION_WLKIT_BOUNDED_BLOCKING_QUEUE_H_

#include <foundation/wkit_condition.h>
#include <foundation/wlkit_lock.h>
#include <foundation/wlkit_baseclass.h>
#include <assert.h>

namespace wlkit
{

template<typename T>
class BoundedBlockingQueue : public NonCopyAble
{
 public:
  explicit BoundedBlockingQueue(int maxSize)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      queue_(maxSize) {
  }

  void put(const T& x) {
    MutexLockGuard lock(mutex_);
    while (queue_.full()) {
      notFull_.wait();
    }
    assert(!queue_.full());
    queue_.push_back(x);
    notEmpty_.notify();
  }

  T take() {
    ScopeLock lock(mutex_);
    while (queue_.empty()) {
      notEmpty_.wait();
    }
    assert(!queue_.empty());
    T front(queue_.front());
    queue_.pop_front();
    notFull_.notify();
    return front;
  }

  bool empty() const {
    ScopeLock lock(mutex_);
    return queue_.empty();
  }

  bool full() const {
    ScopeLock lock(mutex_);
    return queue_.full();
  }

  size_t size() const {
    ScopeLock lock(mutex_);
    return queue_.size();
  }

  size_t capacity() const {
    ScopeLock lock(mutex_);
    return queue_.capacity();
  }

 private:
  mutable MutexLock          mutex_;
  Condition                  notEmpty_;
  Condition                  notFull_;
  boost::circular_buffer<T>  queue_;
};

}

#endif // FOUNDATION_WLKIT_BOUNDED_BLOCKING_QUEUE_H_
