#include "expandable_array.h"
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct _ExpandableArray
{
  void **levels;
  size_t num_levels;
  size_t item_size;
} _ExpandableArray;

static void *createLevel(size_t item_size, size_t capacity)
{
  void *level = malloc(item_size * capacity);
  if (level)
  {
    memset(level, 0, item_size * capacity);
  }
  return level;
}

ExpandableArray *createExpandableArray(size_t item_size)
{
  _ExpandableArray *expandableArray = malloc(sizeof(_ExpandableArray));
  void *firstLevel = createLevel(item_size, 1);
  void **levels = malloc(sizeof(void *));
  if (expandableArray && firstLevel && levels)
  {
    expandableArray->levels = levels;
    expandableArray->levels[0] = firstLevel;
    expandableArray->num_levels = 1;
    expandableArray->item_size = item_size;
  }
  else
  {
    free(expandableArray);
    free(firstLevel);
    free(levels);
    expandableArray = NULL;
  }
  return expandableArray;
}

static bool ensureCapacity(ExpandableArray *array, int index)
{
  while (index >= getCapacityOfExpandableArray(array))
  {
    const size_t new_num_levels = array->num_levels + 1;
    const size_t next_level_capacity = 1 << array->num_levels;
    void *next_level = createLevel(array->item_size, next_level_capacity);
    if (!next_level)
    {
      return false;
    }
    void **new_levels = realloc(array->levels, sizeof(void *) * new_num_levels);
    if (!new_levels)
    {
      free(next_level);
      return false;
    }
    new_levels[new_num_levels - 1] = next_level;

    array->levels = new_levels;
    array->num_levels = new_num_levels;
  }
  return true;
}

// This isn't marked as static so we can unit test it
size_t getLevelIndexForIndexInExpandableArray(size_t index)
{
  return (size_t)log2(index + 1);
}

// This isn't marked as static so we can unit test it
size_t getLevelOffsetForIndexInExpandedArray(size_t index, size_t level_index)
{
  if (index == 0)
  {
    return 0;
  }
  // The previous levels could hold 2^n - 1 items
  size_t base = (1 << level_index) - 1;
  return index - base;
}

void freeExpandableArray(ExpandableArray *array)
{
  if (array)
  {
    for (size_t i = 0; i < array->num_levels; ++i)
    {
      free(array->levels[i]);
    }
    free(array->levels);
    free(array);
  }
}

void *getValueInExpandableArray(ExpandableArray *array, size_t index)
{
  void *value = NULL;
  size_t level_index = getLevelIndexForIndexInExpandableArray(index);
  if (level_index < array->num_levels)
  {
    void *level = array->levels[level_index];
    size_t offset = getLevelOffsetForIndexInExpandedArray(index, level_index);
    // This has potential portability risks, but suffices for now
    value = ((char *)level) + offset * array->item_size;
  }
  return value;
}

bool setValueInExpandableArray(ExpandableArray *array, size_t index, void const *value)
{
  if (!ensureCapacity(array, index))
  {
    return false;
  }
  void *dest = getValueInExpandableArray(array, index);
  if (dest)
  {
    memcpy(dest, value, array->item_size);
  }
  return true;
}

size_t getCapacityOfExpandableArray(ExpandableArray *array)
{
  return (1 << array->num_levels) - 1;
}

void decreaseCapacityOfExpandableArray(ExpandableArray *array, size_t desired_capacity)
{
  while (array->num_levels > 1)
  {
    size_t level_capacity = 1 << (array->num_levels - 1);
    size_t previous_capacity = getCapacityOfExpandableArray(array) - level_capacity;
    if (previous_capacity < desired_capacity)
    {
      break;
    }
    void *level = array->levels[array->num_levels - 1];
    free(level);
    array->levels[array->num_levels - 1] = NULL;
    array->num_levels--;
  }
}
