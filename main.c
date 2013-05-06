/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "nbt.h"
#include "colors.h"
#include "print_nbt.h"
#include "nbt_editor.h"
#include "help_window.h"

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
  if (f == NULL) {
    fprintf(stderr, "Error '%s' while opening '%s'\n", strerror(errno), argv[1]);
    return 1;
  }
  nbt_node* root = nbt_parse_file(f);
  fclose(f);
  if (errno != NBT_OK) {
    fprintf(stderr, "Parsing error!\n");
    return 1;
  }
  initscr();
  start_color();
  init_pair(IMPORTANT_COLOR_PAIR, COLOR_RED, COLOR_BLACK);
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  struct NBT_Window* nbt_window = newNBTWindow(root);
  int ch = KEY_UP;
  do {
    refresh();
    wattron(stdscr, A_REVERSE);
    move(LINES - 1, 0);
    clrtoeol();
    printw("Press 'h' to display help, or press 'q' to quit.");
    wattroff(stdscr, A_REVERSE);
    switch (ch) {
      case 'h':
      case 'H':
        show_help_window();
        break;
      case KEY_DOWN:
        menu_driver(nbt_window->menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(nbt_window->menu, REQ_UP_ITEM);
        break;
      case KEY_NPAGE:
        menu_driver(nbt_window->menu, REQ_SCR_DPAGE);
        break;
      case KEY_PPAGE:
        menu_driver(nbt_window->menu, REQ_SCR_UPAGE);
        break;
      case 'e':
      case 'E': {
        ITEM* item = current_item(nbt_window->menu);
        nbt_node* node = (nbt_node*) item_userptr(item);
        show_edit_window(node, item);
        break;
      }
    }
  } while ((ch = getch()) != 'q' && ch != 'Q');
  endwin();
  return 0;
};