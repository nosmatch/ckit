/*
 *  Created on: 2015年7月10日
 *      Author: fengzhu@mogujie.com
 */

#ifndef  FOUNDATION_WLKIT_BLOCKING_QUEUE_H_
#define  FOUNDATION_WLKIT_BLOCKING_QUEUE_H_

#include <foundation/wlkit_condition.h>
#include <foundation/wlkit_lock.h>
#include <foundation/wlkit_baseclass.h>

#include <deque>
#include <assert.h>

namespace wlkit
{

template<typename T>
class BlockingQueue : public NonCopyAble
{
 public:
  BlockingQueue()
    : mutex_(),
      notEmpty_(mutex_),
      queue_() {
  }

  void put(const T& x) {
    ScopeLock lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify(); 
  }
  
  T take() {
    MutexLockGuard lock(mutex_);
    while (queue_.empty()) {
      notEmpty_.wait();
    }
    assert(!queue_.empty());
    T front(queue_.front());
    queue_.pop_front();
    return front;
  }

  size_t size() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

 private:
  mutable MutexLock mutex_;
  Condition         notEmpty_;
  std::deque<T>     queue_;
};

}


#endif // FOUNDATION_WLKIT_BLOCKING_QUEUE_H_
