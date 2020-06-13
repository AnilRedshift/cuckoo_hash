#include "greatest.h"
#include "expandable_array.h"

TEST x_should_equal_1(void)
{
  ExpandableArray *array = createExpandableArray(1);
  int x = 0;
  ASSERT_EQ(1, x);
  PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[])
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(x_should_equal_1);
  GREATEST_MAIN_END();
}
