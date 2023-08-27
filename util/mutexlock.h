#ifndef LEVELDB_MUTEXTLOCK_H_
#define LEVELDB_MUTEXTLOCK_H_

#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

class SCOPED_LOCKABLE MutexLock {
 public:
  explicit MutexLock(port::Mutex* mu) EXCLUSIVE_LOCK_FUNCTION(mu) : mu_(mu) {
    this->mu_->Lock();
  }
  ~MutexLock() UNLOCK_FUNCTION() { this->mu_->Unlock(); }
  MutexLock(const MutexLock&) = delete;
  MutexLock& operator=(const MutexLock&) = delete;

 private:
  port::Mutex* const mu_;
};
}  // namespace leveldb

#endif