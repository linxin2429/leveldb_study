#ifndef LEVELDB_HASH_H_
#define LEVELDB_HASH_H_

#include <cstddef>
#include <cstdint>
namespace leveldb {
uint32_t Hash(const char* data, size_t n, uint32_t seed);
}

#endif