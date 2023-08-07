#include "leveldb/status.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace leveldb {
const char* Status::CopyStatus(const char* state) {
  uint32_t size;
  std::memcpy(&size, state, sizeof(size));
  char* result = new char[size + 5];
  std::memcpy(result, state, size + 5);
  return result;
}

Status::Status(Code code, const Slice& msg, const Slice& msg2) {
  assert(code != kOk);
  const uint32_t len1 = msg.size();
  const uint32_t len2 = msg2.size();
  const uint32_t size = (len2 ? (len2 + 2) : 0) + len1;
  char* result = new char[size + 5];
  std::memcpy(result, &size, sizeof(size));
  result[4] = static_cast<char>(code);
  std::memcpy(result + 5, msg.data(), len1);
  if (len2) {
    result[5 + len1] = ':';
    result[6 + len1] = ' ';
    std::memcpy(result + 7 + len1, msg2.data(), len2);
  }
  status_ = result;
}

std::string Status::ToString() const {
  if (status_ == nullptr) {
    return "OK";
  } else {
    char tmp[20];
    const char* type;
    switch (code()) {
      case kOk:
        type = "OK";
        break;
      case kNotFound:
        type = "NotFound";
        break;
      case kCorruption:
        type = "Corruption";
        break;
      case kNotSupported:
        type = "NotSupported";
        break;
      case kInvalidArgument:
        type = "InvalidArgument";
        break;
      case kIOError:
        type = "IOError";
        break;
      default:
        std::snprintf(tmp, sizeof(tmp), "Unknown (%d)",
                      static_cast<int>(code()));
        type = tmp;
        break;
    }
    std::string result(type);
    uint32_t length;
    std::memcpy(&length, status_, sizeof(length));
    result.append(status_ + 5, length);
    return result;
  }
}
}  // namespace leveldb