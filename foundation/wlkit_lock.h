/*
 *  Copyright 2016 mogujie.com
 *
 *  Created on: 2015年7月10日
 *      Author: fengzhu@mogujie.com
 */

#ifndef  FOUNDATION_WLKIT_LOCK_H_
#define  FOUNDATION_WLKIT_LOCK_H_

#include <foundation/baseclass.h>

#include <pthread.h>

namespace wlkit {

/**
 * @brief
 * 锁接口类\n
 * 会有多种锁的实现，通过接口类可以统一控制
 */
class ILock : public NonCopyAble {
 public:
  /**
   * 加锁
   */
  virtual void Lock() = 0;

  /**
   * 尝试加锁（立即返回不等待）
   * @return	true为加锁成功  false为加锁不成功
   */
  virtual bool TryLock() = 0;

  /**
   * 解锁
   */
  virtual void UnLock() = 0;
};

/**
 * @brief
 * 		范围锁\n
 * 		实现自动在一个范围内加锁与解锁\n
 * 		@code
 * 		MutexLock m_cLock;
 * 		void foo()
 *      {
 *			ScopeLock cScopLock(m_cLock);
 *			//do something
 *		}
 *		@endcode
 */
class ScopeLock : public NonCopyAble {
public:
  explicit ScopeLock(ILock & cLock)
    : lock_(cLock) {
      lock_.Lock();
    }

  ~ScopeLock() {
    lock_.UnLock();
  }

private:
  ILock & lock_;
};

/**
 * @brief
 * 		自旋锁\n
 * 		自旋锁比较适用于锁使用者保持锁时间比较短的情况。
 * 		正是由于自旋锁使用者一般保持锁时间非常短，因此选择自旋而不是睡眠是非常必要的，自旋锁的效率远高于互斥锁。=
 *
 */
class SpinLock : public ILock {
public:
  /**
   * 自旋锁构造函数
   * @param isShareProcess 是否进程间共享（默认为false）如果进程间需要共享，那么锁必须放在共享内存中才有效果
   */
  explicit SpinLock(bool share_process = false);

  ~SpinLock();

  virtual void Lock();

  virtual bool TryLock();

  virtual void UnLock();

private:
  pthread_spinlock_t lock_;
};

/**
 * @brief
 * 		互斥锁\n
 * 		用于多线程之间同步
 */
class MutexLock : public ILock {
 public:
  /**
   * 互斥锁构造函数
   * @param isMutexRecursive 是否可线程递归加锁（默认为false）如果为TRUE，那么在同一线程里面，对一个锁可以上锁多次，解锁只需要一个操作即可
   * @param isShareProcess   是否进程间共享（默认为false）如果进程间需要共享，那么锁必须放在共享内存中才有效果
   */
  explicit MutexLock(bool mutex_recursive = false, bool share_process = false);

  ~MutexLock();

  virtual void Lock();

  virtual bool TryLock();

  virtual void UnLock();

  pthread_mutex_t* getPthreadMutex() {
    return &lock_;
  }

 private:
  pthread_mutex_t lock_;
};

/// 读写锁自动化操作辅助类
class RWLockHelper {
 public:
  // 锁状态
  enum __lock_state {
    kUnlock = 0,
    kRdlock = 1,
    kWrlock = 2
  };

 public:
  // 指定读写锁，以及锁执行的锁状态
  RWLockHelper(pthread_rwlock_t * rwlock, int state);

  ~RWLockHelper();

  // 获取当前锁状态
  int State() const {
    return state_;
  }
  int Error() const {
    return error_;
  }

 private:
  pthread_rwlock_t * rwlock_;
  int state_;  // 0:unlock, 1:rdlocked, 2:wrlocked
  int error_;  // 错误返回值
};

inline RWLockHelper::RWLockHelper(pthread_rwlock_t * rwlock, int state)
    : rwlock_(rwlock),
      state_(state),
      error_(0) {
  if (state_ == kRdlock)
    error_ = pthread_rwlock_rdlock(rwlock_);
  else if (state_ == kWrlock)
    error_ = pthread_rwlock_wrlock(rwlock_);

  return;
}

inline RWLockHelper::~RWLockHelper() {
  pthread_rwlock_unlock(rwlock_);

  state_ = kUnlock;
  error_ = 0;
  return;
}

}  // namespace wlkit

#endif   //  FOUNDATION_WLKIT_LOCK_H_
