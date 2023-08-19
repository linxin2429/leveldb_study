#ifndef LEVELDB_PORT_STDCXX_H_
#define LEVELDB_PORT_STDCXX_H_

#if defined(LEVELDB_HAS_PORT_CONFIG_H)

#if LEVELDB_HAS_PORT_CONFIG_H
#include "port/port_config.h"
#endif

#elif defined(__has_include)

#if __has_include("port/port_config.h")
#include "port/port_config.h"
#endif

#endif

#if HAVE_CRC32C
#include <crc32c/crc32c.h>
#endif  // HAVE_CRC32C
#if HAVE_SNAPPY
#include <snappy.h>
#endif  // HAVE_SNAPPY
#if HAVE_ZSTD
#define ZSTD_STATIC_LINKING_ONLY  // For ZSTD_compressionParameters.
#include <zstd.h>
#endif  // HAVE_ZSTD

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <string>
#include "port/thread_annotations.h"

namespace leveldb {
namespace port {
class CondVar;

class LOCKABLE Mutex {
 public:
  Mutex() = default;
  ~Mutex() = default;
  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  void Lock() EXCLUSIVE_LOCK_FUNCTION() { mu__.lock(); }
  void Unlock() UNLOCK_FUNCTION() { mu__.unlock(); }
  void AssertHeld() ASSERT_EXCLUSIVE_LOCK() {}

 private:
  friend class CondVar;
  std::mutex mu__;
};

class CondVar {
 public:
  explicit CondVar(Mutex* mu) : mu__(mu) { assert(mu != nullptr); }
  ~CondVar() = default;
  CondVar(const CondVar&) = delete;
  CondVar& operator=(const CondVar&) = delete;
  void Wait() {
    std::unique_lock<std::mutex> lock(mu__->mu__, std::adopt_lock);
    cv__.wait(lock);
    lock.release();
  }
  void Signal() { cv__.notify_one(); }
  void SignalAll() { cv__.notify_all(); }

 private:
  std::condition_variable cv__;
  Mutex* const mu__;
};

inline bool Snappy_Compress(const char* input, size_t length,
                            std::string* output) {
#if HAVE_SNAPPY
  output->resize(snappy::MaxCompressedLength(length));
  size_t outlen;
  snappy::RawCompress(input, length, &(*output)[0], &outlen);
  output->resize(outlen);
  return true;
#else
  (void)input;
  (void)length;
  (void)output;
#endif
  return false;
}

inline bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                         size_t* result) {
#if HAVE_SNAPPY
  return snappy::GetUncompressedLength(input, length, result);
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)result;
  return false;
#endif  // HAVE_SNAPPY
}

inline bool Snappy_Uncompress(const char* input, size_t length, char* output) {
#if HAVE_SNAPPY
  return snappy::RawUncompress(input, length, output);
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)output;
  return false;
#endif  // HAVE_SNAPPY
}

inline bool Zstd_Compress(int level, const char* input, size_t length,
                          std::string* output) {
#if HAVE_ZSTD
  // Get the MaxCompressedLength.
  size_t outlen = ZSTD_compressBound(length);
  if (ZSTD_isError(outlen)) {
    return false;
  }
  output->resize(outlen);
  ZSTD_CCtx* ctx = ZSTD_createCCtx();
  ZSTD_compressionParameters parameters =
      ZSTD_getCParams(level, std::max(length, size_t{1}), /*dictSize=*/0);
  ZSTD_CCtx_setCParams(ctx, parameters);
  outlen = ZSTD_compress2(ctx, &(*output)[0], output->size(), input, length);
  ZSTD_freeCCtx(ctx);
  if (ZSTD_isError(outlen)) {
    return false;
  }
  output->resize(outlen);
  return true;
#else
  // Silence compiler warnings about unused arguments.
  (void)level;
  (void)input;
  (void)length;
  (void)output;
  return false;
#endif  // HAVE_ZSTD
}

inline bool Zstd_GetUncompressedLength(const char* input, size_t length,
                                       size_t* result) {
#if HAVE_ZSTD
  size_t size = ZSTD_getFrameContentSize(input, length);
  if (size == 0) return false;
  *result = size;
  return true;
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)result;
  return false;
#endif  // HAVE_ZSTD
}

inline bool Zstd_Uncompress(const char* input, size_t length, char* output) {
#if HAVE_ZSTD
  size_t outlen;
  if (!Zstd_GetUncompressedLength(input, length, &outlen)) {
    return false;
  }
  ZSTD_DCtx* ctx = ZSTD_createDCtx();
  outlen = ZSTD_decompressDCtx(ctx, output, outlen, input, length);
  ZSTD_freeDCtx(ctx);
  if (ZSTD_isError(outlen)) {
    return false;
  }
  return true;
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)output;
  return false;
#endif  // HAVE_ZSTD
}

inline bool GetHeapProfile(void (*func)(void*, const char*, int), void* arg) {
  // Silence compiler warnings about unused arguments.
  (void)func;
  (void)arg;
  return false;
}

inline uint32_t AcceleratedCRC32C(uint32_t crc, const char* buf, size_t size) {
#if HAVE_CRC32C
  return ::crc32c::Extend(crc, reinterpret_cast<const uint8_t*>(buf), size);
#else
  // Silence compiler warnings about unused arguments.
  (void)crc;
  (void)buf;
  (void)size;
  return 0;
#endif  // HAVE_CRC32C
}

}  // namespace port
}  // namespace leveldb

#endif