#ifndef LEVELDB_DB_H_
#define LEVELDB_DB_H_

#include "leveldb/export.h"
#include "leveldb/iterator.h"
#include "leveldb/options.h"

namespace leveldb {

static const int kMajorVersion = 1;
static const int kMinorVersion = 23;

struct Options;
struct ReadOptions;
struct WriteOptions;
class WriteBatch;

class LEVELDB_EXPORT Snapshot {
 protected:
  virtual ~Snapshot();
};

struct LEVELDB_EXPORT Range {
  Range() = default;
  Range(const Slice& s, const Slice& l) : start(s), limit(l) {}
  Slice start;
  Slice limit;
};

class LEVELDB_EXPORT DB {
 public:
  static Status Open(const Options& options, const std::string& name,
                     DB** dbptr);
  DB() = default;
  DB(const DB&) = delete;
  DB& operator=(const DB&) = delete;
  virtual ~DB();

  virtual Status Put(const WriteOptions& options, const Slice& key,
                     const Slice& value) = 0;
  virtual Status Delete(const WriteOptions& options, const Slice& key) = 0;
  virtual Status Write(const WriteOptions& options, WriteBatch* updates) = 0;

  virtual Status Get(const ReadOptions& options, const Slice& key,
                     std::string* value) = 0;
  virtual Iterator* NewIterator(const ReadOptions& options) = 0;
  virtual const Snapshot* GetSnapshot() = 0;
  virtual void ReleaseSnapshot(const Snapshot* snapshot) = 0;
  virtual bool GetProperty(const Slice& property, std::string* value) = 0;
  virtual void GetApproximateSizes(const Range* range, int n,
                                   uint64_t* sizes) = 0;
  virtual void CompactRange(const Slice* begin, const Slice* end) = 0;
};

LEVELDB_EXPORT Status DestroyDB(const std::string& name,
                                const Options& options);

LEVELDB_EXPORT Status RepairDB(const std::string& dbname,
                               const Options& options);
}  // namespace leveldb
#endif