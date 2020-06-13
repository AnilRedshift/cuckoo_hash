#include "expandable_array.h"
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct
{
  void *buf;
} ExpandableArrayLevel;

typedef struct _ExpandableArray
{
  ExpandableArrayLevel **levels;
  size_t num_levels;
  size_t item_size;
} _ExpandableArray;

static ExpandableArrayLevel *createLevel(size_t item_size, size_t capacity)
{
  ExpandableArrayLevel *level = malloc(offsetof(ExpandableArrayLevel, buf) + item_size * capacity);
  if (level)
  {
    memset(&level->buf, 0, item_size * capacity);
  }
  return level;
}

ExpandableArray *createExpandableArray(size_t item_size)
{
  _ExpandableArray *expandableArray = malloc(sizeof(_ExpandableArray));
  ExpandableArrayLevel *firstLevel = createLevel(item_size, 1);
  ExpandableArrayLevel **levels = malloc(sizeof(_ExpandableArray *));
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

// This isn't marked as static so we can unit test it
size_t getLevelForIndexInExpandableArray(size_t index)
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
