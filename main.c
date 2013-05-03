/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

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
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  struct NBT_Window* nbt_window = newNBTWindow(root, 40, 0, 0, 0);
  int ch;
  while ((ch = getch()) != 'q') {
    switch (ch) {
    case KEY_UP:
      if (nbt_window->current_line > 0)
        nbt_window->current_line--;
      break;
    case KEY_DOWN:
      if (nbt_window->current_line < nbt_window->last_line)
        nbt_window->current_line++;
      break;
    }
    redrawNBTWindow(nbt_window);
  }
  endwin();
  return 0;
};