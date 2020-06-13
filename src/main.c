#include "expandable_array.h"
#include <stdio.h>

int main()
{
  ExpandableArray *array = createExpandableArray(1);
  array = NULL;
  printf("hello world\n");
  return 0;
}
