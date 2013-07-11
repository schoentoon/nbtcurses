/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "help_window.h"

#include <string.h>
#include <ncurses.h>

static const char* help[] =
  { "Navigation:"
  , " Up, down, page up and page down can be used to navigate."
  , " You can also use the mouse scroll wheel."
  , "Editing:"
  , " To edit the current selected item simply press 'e'"
  , "Searching:"
  , " To search press 'f' and just start typing, it'll search on keys only at this moment."
  , NULL };

static const attr_t line_attrs[] =
  { A_BOLD
  , 0
  , 0
  , A_BOLD
  , 0
  , A_BOLD
  , 0 };

static inline size_t amount_of_lines() {
  size_t i = 0;
  while (help[i])
    i++;
  return i;
};

static inline size_t longest_line() {
  size_t i = 0;
  const char* line = help[i];
  size_t longest = 0;
  while (line) {
    size_t len = strlen(line);
    if (len > longest)
      longest = len;
    line = help[++i];
  }
  return longest;
};

#define HEIGHT (amount_of_lines() + 2)
#define WIDTH (longest_line() + 3)

void show_help_window() {
  WINDOW* w = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) /2);
  size_t i = 0;
  while (help[i]) {
    wattron(w, line_attrs[i]);
    mvwprintw(w, i + 1, 1, help[i]);
    wattroff(w, line_attrs[i]);
    i++;
  };
  box(w, 0, 0);
  mvwprintw(w, HEIGHT-1, 1, "Press any key to get rid of this window.");
  wrefresh(w);
  delwin(w);
};