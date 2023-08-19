#ifndef LEVELDB_FILTER_POLICY_H_
#define LEVELDB_FILTER_POLICY_H_

#include <string>

#include "leveldb/export.h"

namespace leveldb {

class Slice;

class LEVELDB_EXPORT FilterPolicy {
 public:
  virtual ~FilterPolicy();

  virtual const char* Name() const = 0;

  virtual void CreateFilter(const Slice* keys, int n,
                            std::string* dst) const = 0;

  virtual bool KeyMayMatch(const Slice& key, const Slice& filter) const = 0;
};

LEVELDB_EXPORT const FilterPolicy* NewBloomFilterPolicy(int bits_per_key);
}  // namespace leveldb

#endif