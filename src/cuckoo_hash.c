#include "cuckoo_hash.h"
#include "expandable_array.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
  size_t key;
  void *value;
} CuckooItem;

typedef struct _CuckooHash
{
  ExpandableArray *data;
  size_t capacity;
  size_t count;
  size_t offsets[2];
} _CuckooHash;

#define HASH_1_OFFSET_INDEX (size_t)0
#define HASH_2_OFFSET_INDEX (size_t)1

static bool setValueInCuckooHashInternal(CuckooHash *hash, CuckooItem *item);

static void createHashOffsets(CuckooHash *hash)
{
  const size_t half_capacity = hash->capacity / 2;
  hash->offsets[0] = rand() % half_capacity;
  hash->offsets[1] = rand() % half_capacity;
}

static size_t getIndex(CuckooHash *hash, size_t key, size_t offset_index)
{
  const size_t half_capacity = hash->capacity / 2;
  const size_t base = offset_index == 0 ? 0 : half_capacity;
  const size_t offset = hash->offsets[offset_index];
  return base + (((key % half_capacity) + offset) % half_capacity);
}

static void *getValueFromIndex(CuckooHash *hash, size_t key, size_t index)
{
  CuckooItem *const item = getValueInExpandableArray(hash->data, index);
  return item && item->key == key ? item->value : NULL;
}

static void reseed(CuckooHash *hash, size_t capacity_to_fix_until)
{
  createHashOffsets(hash);
  for (size_t i = 0; i < capacity_to_fix_until; ++i)
  {
    CuckooItem *item = getValueInExpandableArray(hash->data, i);
    if (item)
    {
      const size_t index1 = getIndex(hash, item->key, HASH_1_OFFSET_INDEX);
      const size_t index2 = getIndex(hash, item->key, HASH_2_OFFSET_INDEX);
      if (i != index1 && i != index2)
      {
        CuckooItem item_copy;
        memcpy(&item_copy, item, sizeof(CuckooItem));
        deleteValueInExpandableArray(hash->data, i);
        setValueInCuckooHashInternal(hash, &item_copy);
      }
    }
  }
}

static void doubleCapacity(CuckooHash *hash)
{
  if (hash->capacity != SIZE_MAX)
  {
    const size_t old_capacity = hash->capacity;
    hash->capacity = SIZE_MAX / 2 < hash->capacity ? SIZE_MAX : hash->capacity * 2;
    reseed(hash, old_capacity);
  }
}

static bool setValueInCuckooHashInternal(CuckooHash *hash, CuckooItem *item)
{
  if (hash->capacity != SIZE_MAX && hash->capacity / 2 <= hash->count)
  {
    doubleCapacity(hash);
  }

  while (true)
  {
    bool use_hash1 = true;
    for (size_t i = 0; i < 3 * log2(hash->count); ++i)
    {
      const size_t index = getIndex(hash, item->key, use_hash1 ? HASH_1_OFFSET_INDEX : HASH_2_OFFSET_INDEX);
      CuckooItem *existing_item = getValueInExpandableArray(hash->data, index);
      if (existing_item == NULL)
      {
        return setValueInExpandableArray(hash->data, index, item);
      }
      else
      {
        CuckooItem tmp;
        memcpy(&tmp, item, sizeof(CuckooItem));
        memcpy(item, existing_item, sizeof(CuckooItem));
        memcpy(existing_item, &tmp, sizeof(CuckooItem));
        use_hash1 = !use_hash1;
      }
    }
    reseed(hash, hash->capacity);
  }
  return false;
}

CuckooHash *createCuckooHash()
{
  CuckooHash *hash = malloc(sizeof(CuckooHash));
  ExpandableArray *array = createExpandableArray(sizeof(CuckooItem));
  if (hash && array)
  {
    hash->data = array;
    hash->capacity = 64;
    hash->count = 0;
    createHashOffsets(hash);
  }
  else
  {
    free(hash);
    free(array);
    hash = NULL;
  }
  return hash;
}

void *getValueFromCuckooHash(CuckooHash *hash, size_t key)
{
  const size_t first_index = getIndex(hash, key, HASH_1_OFFSET_INDEX);
  const size_t second_index = getIndex(hash, key, HASH_2_OFFSET_INDEX);
  void *value = getValueFromIndex(hash, key, first_index);
  return value ? value : getValueFromIndex(hash, key, second_index);
}

bool setValueInCuckooHash(CuckooHash *hash, size_t key, void *value)
{
  CuckooItem item = {.key = key, .value = value};
  return setValueInCuckooHashInternal(hash, &item);
}

void freeCuckooHash(CuckooHash *hash)
{
  free(hash);
}
