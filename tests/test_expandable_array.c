#include "greatest.h"
#include "expandable_array.h"

extern size_t getLevelIndexForIndexInExpandableArray(size_t index);
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

    ASSERT_EQ_FMT(expected, getLevelIndexForIndexInExpandableArray(i), "%zu");
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
    size_t level = getLevelIndexForIndexInExpandableArray(i);
    size_t offset = getLevelOffsetForIndexInExpandedArray(i, level);

    char msg[25];
    sprintf(msg, "Failed for index %zu", i);

    ASSERT_EQ_FMTm(msg, expected, offset, "%zu");
  }
  PASS();
}

TEST test_get_capacity(void *args)
{
  TestData *test_data = (TestData *)args;
  for (size_t i = 0; i < 18; ++i)
  {
    size_t expected = -1;
    switch (i)
    {
    case 0:
      expected = 1;
      break;
    case 1:
    case 2:
      expected = 3;
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      expected = 7;
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
      expected = 15;
      break;
    case 15:
    case 16:
    case 17:
      expected = 31;
      break;
    }
    char msg[25];
    sprintf(msg, "Failed for index %zu", i);

    char value = 'a';
    ASSERT(setValueInExpandableArray(test_data->array, i, &value));
    size_t actual = getCapacityOfExpandableArray(test_data->array);
    ASSERT_EQ_FMTm(msg, expected, actual, "%zu");
  }
  PASS();
}

TEST test_get_value_beyond_capacity(void *args)
{
  TestData *test_data = (TestData *)args;
  void *result = getValueInExpandableArray(test_data->array, 42);
  ASSERT_FALSE(result);
  PASS();
}

TEST test_set_and_get_first_value(void *args)
{
  TestData *test_data = (TestData *)args;
  char value = 'a';
  ASSERT(setValueInExpandableArray(test_data->array, 0, &value));
  char *result = getValueInExpandableArray(test_data->array, 0);
  ASSERT(result);
  ASSERT_EQ_FMT('a', *result, "%c");
  PASS();
}

TEST test_set_and_get_first_value_of_integers(void *args)
{
  TestData *test_data = (TestData *)args;
  int value = 42;
  ASSERT(setValueInExpandableArray(test_data->array4, 0, &value));
  int *result = getValueInExpandableArray(test_data->array4, 0);
  ASSERT(result);
  ASSERT_EQ_FMT(42, *result, "%d");
  PASS();
}

TEST test_set_and_get_at_first_index(void *args)
{
  TestData *test_data = (TestData *)args;
  int value = 42;
  ASSERT(setValueInExpandableArray(test_data->array4, 1, &value));
  int *result = getValueInExpandableArray(test_data->array4, 1);
  ASSERT(result);
  ASSERT_EQ_FMT(42, *result, "%d");
  PASS();
}

TEST test_set_and_get_at_tenth_index(void *args)
{
  TestData *test_data = (TestData *)args;
  int value = 42;
  ASSERT(setValueInExpandableArray(test_data->array4, 10, &value));
  int *result = getValueInExpandableArray(test_data->array4, 10);
  ASSERT(result);
  ASSERT_EQ_FMT(42, *result, "%d");
  PASS();
}

uint32_t MurmurOAAT32(const char *key)
{
  uint32_t h = 3323198485ul;
  for (; *key; ++key)
  {
    h ^= *key;
    h *= 0x5bd1e995;
    h ^= h >> 15;
  }
  return h;
}

TEST test_set_multiple_values(void *args)
{
  TestData *test_data = (TestData *)args;
  const size_t MAX_SIZE = 300;
  for (size_t i = 0; i < 50; ++i)
  {
    char buf[20];
    sprintf(buf, "my cool key for %zu", i);
    const uint32_t index = MurmurOAAT32(buf) % MAX_SIZE;
    ASSERT(setValueInExpandableArray(test_data->array4, index, &index));
  }

  for (size_t i = 0; i < 50; ++i)
  {
    char buf[20];
    sprintf(buf, "my cool key for %zu", i);
    const uint32_t index = MurmurOAAT32(buf) % MAX_SIZE;
    void *const result = getValueInExpandableArray(test_data->array4, index);
    ASSERT(result);
    uint32_t actual = *(uint32_t *)result;
    ASSERT_EQ_FMT(index, actual, "%u");
  }
  PASS();
}

TEST test_decrease_capacity_from_capacity_one(void *args)
{
  TestData *test_data = (TestData *)args;
  decreaseCapacityOfExpandableArray(test_data->array, 0);
  size_t actual = getCapacityOfExpandableArray(test_data->array);
  size_t expected = 1;
  ASSERT_EQ_FMT(expected, actual, "%zu");
  PASS();
}

TEST test_decrease_capacity_to_exact_level(void *args)
{
  TestData *test_data = (TestData *)args;
  char value = 'a';
  ASSERT(setValueInExpandableArray(test_data->array, 10, &value));
  decreaseCapacityOfExpandableArray(test_data->array, 3);
  size_t actual = getCapacityOfExpandableArray(test_data->array);
  size_t expected = 3;
  ASSERT_EQ_FMT(expected, actual, "%zu");
  PASS();
}

TEST test_no_decrease_when_not_below_previous_capacity(void *args)
{
  TestData *test_data = (TestData *)args;
  char value = 'a';
  ASSERT(setValueInExpandableArray(test_data->array, 10, &value));
  decreaseCapacityOfExpandableArray(test_data->array, 9);
  size_t actual = getCapacityOfExpandableArray(test_data->array);
  size_t expected = 15;
  ASSERT_EQ_FMT(expected, actual, "%zu");
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
  RUN_TEST1(test_get_capacity, &test_data);
  RUN_TEST1(test_get_value_beyond_capacity, &test_data);
  RUN_TEST1(test_set_and_get_first_value, &test_data);
  RUN_TEST1(test_set_and_get_first_value_of_integers, &test_data);
  RUN_TEST1(test_set_and_get_at_first_index, &test_data);
  RUN_TEST1(test_set_and_get_at_tenth_index, &test_data);
  RUN_TEST1(test_set_multiple_values, &test_data);
  RUN_TEST1(test_decrease_capacity_from_capacity_one, &test_data);
  RUN_TEST1(test_decrease_capacity_to_exact_level, &test_data);
  RUN_TEST1(test_no_decrease_when_not_below_previous_capacity, &test_data);
}

GREATEST_MAIN_DEFS();
int main(int argc, char *argv[])
{
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(ExpandableArraySuite);
  GREATEST_MAIN_END();
}
