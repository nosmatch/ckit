/*
 *  Copyright 2016 mogujie.com
 *
 *  Created on: 2015年7月10日
 *      Author: fengzhu@mogujie.com
 */

#ifndef  FOUNDATION_WLKIT_COUNT_DOWN_H_
#define  FOUNDATION_WLKIT_COUNT_DOWN_H_

#include <foundation/wlkit_condition.h>
#include <coundation/wlkit_lock.h>
#include <foundation/baseclass.h>

namespace muduo
{

class CountDownLatch : public NonCopyAble
{
 public:

  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable MutexLock mutex_;
  Condition condition_;
  int count_;
};

}

#endif // FOUNDATION_WLKIT_COUNT_DOWN_H_
