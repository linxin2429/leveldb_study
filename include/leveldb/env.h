#ifndef LEVELDB_ENV_H_
#define LEVELDB_ENV_H_

#include <cstdarg>
#include <cstdint>
#include <string>
#include <vector>

#include "export.h"
#include "leveldb/export.h"
#include "leveldb/status.h"
namespace leveldb {

class SequentialFile;
class RandomAccessFile;
class WritableFile;
class Logger;
class FileLock;
class LEVELDB_EXPORT Env {
 public:
  Env();
  Env(const Env&) = delete;
  Env& operator=(const Env&) = delete;

  virtual ~Env();

  static Env* Default();

  virtual Status NewSequentialFile(const std::string& fname,
                                   SequentialFile** result) = 0;

  virtual Status NewRandomAccessFile(const std::string& fname,
                                     RandomAccessFile** result) = 0;

  virtual Status NewWritableFile(const std::string& fname,
                                 WritableFile** result) = 0;

  virtual Status NewAppendableFile(const std::string& fname,
                                   WritableFile** result) = 0;

  virtual bool FileExists(const std::string& fname) = 0;

  virtual Status GetChildren(const std::string& dir,
                             std::vector<std::string>* result) = 0;
  virtual Status RemoveFile(const std::string& fname) = 0;

  virtual Status CreateDir(const std::string& dirname) = 0;

  virtual Status RemoveDir(const std::string& dirname) = 0;

  virtual Status DeleteDir(const std::string& dirname) = 0;

  virtual Status GetFileSize(const std::string& fname, uint64_t* size) = 0;

  virtual Status RenameFile(const std::string& src,
                            const std::string& target) = 0;

  virtual Status LockFile(const std::string& fname, FileLock** lock) = 0;

  virtual Status UnlockFile(FileLock* lock) = 0;

  virtual void Schedule(void (*function)(void* args), void* arg) = 0;

  virtual void StartThread(void (*function)(void* arg), void* arg) = 0;

  virtual Status GetTestDirectory(std::string* path) = 0;

  virtual uint64_t NowMicros() = 0;

  virtual void SleepForMicroseconds(int micros) = 0;

  virtual Status NewLogger(const std::string& fname, Logger** result) = 0;
};

class LEVELDB_EXPORT SequentialFile {
 public:
  SequentialFile() = default;
  SequentialFile(const SequentialFile&) = delete;
  virtual ~SequentialFile();

  virtual Status Read(size_t n, Slice* result, char* scratch) = 0;
  virtual Status Skip(uint64_t n) = 0;
};

class LEVELDB_EXPORT RandomAccessFile {
 public:
  RandomAccessFile() = default;
  RandomAccessFile(const RandomAccessFile&) = delete;
  RandomAccessFile& operator=(const RandomAccessFile&) = delete;

  virtual ~RandomAccessFile();

  virtual Status Read(uint64_t offset, size_t n, Slice* result,
                      char* scratch) const = 0;
};

class LEVELDB_EXPORT WritableFile {
 public:
  WritableFile() = default;
  WritableFile(const WritableFile&) = delete;
  WritableFile& operator=(const WritableFile&) = delete;

  virtual ~WritableFile();

  virtual Status Append(const Slice& data) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;
};

class LEVELDB_EXPORT Logger {
 public:
  Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  virtual ~Logger();

  virtual void Logv(const char* format, std::va_list ap) = 0;
};

class LEVELDB_EXPORT FileLock {
 public:
  FileLock() = default;
  FileLock(const FileLock&) = delete;
  FileLock& operator=(const FileLock&) = delete;

  virtual ~FileLock();
};

void Log(Logger* info_log, const char* format, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((__format__(__printf__, 2, 3)))
#endif
    ;

LEVELDB_EXPORT Status WriteStringToFile(Env* env, const Slice& data,
                                        const std::string& fname);
LEVELDB_EXPORT Status ReadFileToString(Env* env, const std::string& fname,
                                       std::string* data);

class LEVELDB_EXPORT EnvWrapper : public Env {
 public:
  explicit EnvWrapper(Env* t) : target_(t) {}
  virtual ~EnvWrapper();
  Env* target() const { return target_; }

  Status NewSequentialFile(const std::string& f, SequentialFile** r) override {
    return target_->NewSequentialFile(f, r);
  }
  Status NewRandomAccessFile(const std::string& f,
                             RandomAccessFile** r) override {
    return target_->NewRandomAccessFile(f, r);
  }
  Status NewWritableFile(const std::string& f, WritableFile** r) override {
    return target_->NewWritableFile(f, r);
  }
  Status NewAppendableFile(const std::string& f, WritableFile** r) override {
    return target_->NewAppendableFile(f, r);
  }
  bool FileExists(const std::string& f) override {
    return target_->FileExists(f);
  }
  Status GetChildren(const std::string& dir,
                     std::vector<std::string>* r) override {
    return target_->GetChildren(dir, r);
  }
  Status RemoveFile(const std::string& f) override {
    return target_->RemoveFile(f);
  }
  Status CreateDir(const std::string& d) override {
    return target_->CreateDir(d);
  }
  Status RemoveDir(const std::string& d) override {
    return target_->RemoveDir(d);
  }
  Status GetFileSize(const std::string& f, uint64_t* s) override {
    return target_->GetFileSize(f, s);
  }
  Status RenameFile(const std::string& s, const std::string& t) override {
    return target_->RenameFile(s, t);
  }
  Status LockFile(const std::string& f, FileLock** l) override {
    return target_->LockFile(f, l);
  }
  Status UnlockFile(FileLock* l) override { return target_->UnlockFile(l); }
  void Schedule(void (*f)(void*), void* a) override {
    return target_->Schedule(f, a);
  }
  void StartThread(void (*f)(void*), void* a) override {
    return target_->StartThread(f, a);
  }
  Status GetTestDirectory(std::string* path) override {
    return target_->GetTestDirectory(path);
  }
  Status NewLogger(const std::string& fname, Logger** result) override {
    return target_->NewLogger(fname, result);
  }
  uint64_t NowMicros() override { return target_->NowMicros(); }
  void SleepForMicroseconds(int micros) override {
    target_->SleepForMicroseconds(micros);
  }

 private:
  Env* target_;
};
}  // namespace leveldb

#endif