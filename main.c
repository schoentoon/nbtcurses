#include "nbt.h"
#include "print_nbt.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ncurses.h>

int main(int argc, char** argv) {
  if (argc == 1) {
    fprintf(stderr, "USAGE: %s [nbtfile]\n", argv[0]);
    return 1;
  }
  FILE* f = fopen(argv[1], "rb");
  nbt_node* root = nbt_parse_file(f);
  fclose(f);
  if (errno != NBT_OK) {
    fprintf(stderr, "Parsing error!\n");
    return 1;
  }
  initscr();
  scrollok(stdscr, TRUE);
  nbt_wprint(stdscr, root, 0);
  getch();
  endwin();
  return 0;
};