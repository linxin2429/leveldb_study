#ifndef LEVELDB_STATUS_H_
#define LEVELDB_STATUS_H_

#include <string>

#include "leveldb/export.h"
#include "leveldb/slice.h"

namespace leveldb {
class LEVELDB_EXPORT Status {
 public:
  /*success*/
  Status() noexcept : status_(nullptr) {}
  ~Status() { delete[] status_; }
  Status(const Status& rhs);
  Status& operator=(const Status& rhs);
  Status(Status&& rhs) noexcept : status_(rhs.status_) {
    rhs.status_ = nullptr;
  }
  Status& operator=(Status&& rhs) noexcept;

  static Status OK() { return Status(); }

  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotFound, msg, msg2);
  }

  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kCorruption, msg, msg2);
  }

  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotSupported, msg, msg2);
  }

  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kInvalidArgument, msg, msg2);
  }

  static Status IOError(const Slice& msg, const Slice& msg2) {
    return Status(kIOError, msg, msg2);
  }

  bool ok() const { return status_ == nullptr; }

  bool IsNotFound() const { return code() == kNotFound; }

  bool IsCorruption() const { return code() == kCorruption; }

  bool IsNotSupportedError() const { return code() == kNotSupported; }

  bool IsInvalidArgument() const { return code() == kInvalidArgument; }

  bool IsIOError() const { return code() == kIOError; }

  std::string ToString() const;

 private:
  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };

  Code code() const {
    return (status_ == nullptr) ? kOk : static_cast<Code>(status_[4]);
  }
  /*
      msg:error msg
      msg2:description
  */
  Status(Code code, const Slice& msg, const Slice& msg2);

  static const char* CopyStatus(const char* s);
  /*
      status_[0..3] = message length
      status_[4] = code
      status_[5..] = message
  */
  const char* status_;
};

inline Status::Status(const Status& rhs) {
  status_ = (rhs.status_ == nullptr) ? nullptr : CopyStatus(rhs.status_);
}

inline Status& Status::operator=(const Status& rhs) {
  if (this != &rhs) {
    delete[] status_;
    status_ = (rhs.status_ == nullptr) ? nullptr : CopyStatus(rhs.status_);
  }
  return *this;
}

inline Status& Status::operator=(Status&& rhs) noexcept {
  std::swap(status_, rhs.status_);
  return *this;
}
}  // namespace leveldb

#endif