#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct _CuckooHash CuckooHash;

CuckooHash *createCuckooHash();
void freeCuckooHash(CuckooHash *hash);
void *getValueFromCuckooHash(CuckooHash *hash, size_t key);
bool setValueInCuckooHash(CuckooHash *hash, size_t key, void *value);
