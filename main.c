#include "nbt.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  if (argc == 1) {
    fprintf(stderr, "USAGE: %s [nbtfile]\n", argv[0]);
    return 1;
  }
  FILE* f = fopen(argv[1], "rb");
  nbt_node* root = nbt_parse_file(f);
  fclose(f);

  if(errno != NBT_OK) {
    fprintf(stderr, "Parsing error!\n");
    return 1;
  }
  char* str = nbt_dump_ascii(root);
  nbt_free(root);
  if (str == NULL)
    fprintf(stderr, "Printing error!");
  printf("%s", str);
  free(str);
  return 0;
};