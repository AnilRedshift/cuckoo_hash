#pragma once
#include <stdlib.h>

typedef struct _ExpandableArray ExpandableArray;

ExpandableArray *createExpandableArray(size_t item_size);
void freeExpandableArray(ExpandableArray *array);
