#include "greatest.h"
#include "expandable_array.h"

extern size_t getLevelForIndexInExpandableArray(size_t index);
extern size_t getLevelOffsetForIndexInExpandedArray(size_t index, size_t level_index);

typedef struct
{
  ExpandableArray *array;
  ExpandableArray *array4;
} TestData;

TEST test_get_level_for_index(void *args)
{
  for (size_t i = 0; i < 18; ++i)
  {
    size_t expected = -1;
    switch (i)
    {
    case 0:
      expected = 0;
      break;
    case 1:
    case 2:
      expected = 1;
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      expected = 2;
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
      expected = 3;
      break;
    case 15:
    case 16:
    case 17:
      expected = 4;
      break;
    }

    ASSERT_EQ_FMT(expected, getLevelForIndexInExpandableArray(i), "%zu");
  }
  PASS();
}

TEST test_get_offsetfor_index(void *args)
{
  for (size_t i = 0; i < 18; ++i)
  {
    size_t expected = -1;
    switch (i)
    {
    case 0:
      expected = 0;
      break;
    case 1:
      expected = 0;
      break;
    case 2:
      expected = 1;
      break;
    case 3:
      expected = 0;
      break;
    case 4:
      expected = 1;
      break;
    case 5:
      expected = 2;
      break;
    case 6:
      expected = 3;
      break;

    case 7:
      expected = 0;
      break;
    case 8:
      expected = 1;
      break;
    case 9:
      expected = 2;
      break;
    case 10:
      expected = 3;
      break;
    case 11:
      expected = 4;
      break;
    case 12:
      expected = 5;
      break;
    case 13:
      expected = 6;
      break;
    case 14:
      expected = 7;
      break;
    case 15:
      expected = 0;
      break;
    case 16:
      expected = 1;
      break;
    case 17:
      expected = 2;
      break;
    }
    size_t level = getLevelForIndexInExpandableArray(i);
    size_t offset = getLevelOffsetForIndexInExpandedArray(i, level);

    char msg[25];
    sprintf(msg, "Failed for index %zu", i);

    ASSERT_EQ_FMTm(msg, expected, offset, "%zu");
  }
  PASS();
}

void test_setup(void *args)
{
  TestData *test_data = (TestData *)args;
  test_data->array = createExpandableArray(1);
  test_data->array4 = createExpandableArray(4);
}

void test_teardown(void *args)
{
  TestData *test_data = (TestData *)args;
  freeExpandableArray(test_data->array);
  freeExpandableArray(test_data->array4);
}

SUITE(ExpandableArraySuite)
{
  TestData test_data;

  GREATEST_SET_SETUP_CB(test_setup, &test_data);
  GREATEST_SET_TEARDOWN_CB(test_teardown, &test_data);
  RUN_TEST1(test_get_level_for_index, &test_data);
  RUN_TEST1(test_get_offsetfor_index, &test_data);
}

GREATEST_MAIN_DEFS();
int main(int argc, char *argv[])
{
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(ExpandableArraySuite);
  GREATEST_MAIN_END();
}
