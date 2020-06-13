#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct _ExpandableArray ExpandableArray;

ExpandableArray *createExpandableArray(size_t item_size);
void freeExpandableArray(ExpandableArray *array);
void *getValueInExpandableArray(ExpandableArray *array, size_t index);
bool setValueInExpandableArray(ExpandableArray *array, size_t index, void const *value);
size_t getCapacityOfExpandableArray(ExpandableArray *array);
