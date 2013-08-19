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
#include "find_window.h"

#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h>

static const struct option g_LongOpts[] = {
  { "help",     no_argument,       0, 'h' },
  { "print",    no_argument,       0, 'P' },
  { "get",      required_argument, 0, 'g' },
  { 0, 0, 0, 0 }
};

unsigned char print_tree = 0;

static void show_help_curses() {
  wattron(stdscr, A_REVERSE);
  move(LINES - 1, 0);
  clrtoeol();
  printw("Press 'h' to display help, or press 'q' to quit.");
  wattroff(stdscr, A_REVERSE);
  refresh();
};

int usage(char* program) {
  fprintf(stderr, "USAGE: %s [options] [nbtfile]\n", program);
  fprintf(stderr, "-P, --print\tPrint the tree instead of opening the editor.\n");
  fprintf(stderr, "-h, --help\tPrint this help\n");
  return 0;
};

int main(int argc, char** argv) {
  if (argc == 1)
    return usage(argv[0]);
  char* get_string = NULL;
  int arg, optindex;
  while ((arg = getopt_long(argc, argv, "hPg:", g_LongOpts, &optindex)) != -1) {
    switch (arg) {
    case 'P':
      print_tree = 1;
      break;
    case 'g':
      get_string = optarg;
      print_tree = 1;
      break;
    case 'h':
    default:
      return usage(argv[0]);
    }
  }
  const char* filename = argv[argc-1];
  nbt_node* root = nbt_parse_path(filename);
  if (errno != NBT_OK) {
    fprintf(stderr, "%s\n", nbt_error_to_string(errno));
    return 1;
  }
  if (get_string) {
    const size_t l = strlen(get_string) + 2;
    char* g = malloc(l);
    bzero(g, l);
    strcpy(g, get_string);
    root = nbt_get_string(root, g);
    if (root == NULL) {
      fprintf(stderr, "Failed to find '%s'\n", get_string);
      return 1;
    }
  }
  struct NBT_Window* nbt_window = newNBTWindow(root);
  if (print_tree) {
    int i;
    ITEM* item = nbt_window->items[0];
    for (i = 0; item; item = nbt_window->items[++i])
      printf("%s\n", item->name.str);
    return 0;
  }
  initscr();
  start_color();
  init_pair(IMPORTANT_COLOR_PAIR, COLOR_RED, 0);
  init_pair(NOTICE_COLOR_PAIR, COLOR_GREEN, 0);
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  printNBTWindow(nbt_window);
  int ch = KEY_UP;
  do {
    show_help_curses();
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
      case 'w':
      case 'W': {
        FILE* f = fopen(filename, "wb");
        if (f == NULL)
          error(strerror(errno));
        else {
          nbt_status status = nbt_dump_file(root, f, STRAT_GZIP);
          if (status == NBT_OK)
            notice("Succesfully saved modified nbt data to %s.", filename);
          else
            error("There was an error while saving %s.", nbt_error_to_string(status));
          fclose(f);
        }
        break;
      }
      case 'f':
      case 'F':
        enable_find(nbt_window);
        break;
    }
    show_help_curses();
  } while ((ch = getch()) != 'q' && ch != 'Q');
  endwin();
  return 0;
};