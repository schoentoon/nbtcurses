/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "help_window.h"

#include <ncurses.h>

#define HEIGHT (4/* amount of lines */ + 2)
#define WIDTH (56/* longest line */ + 3)

void show_help_window() {
  WINDOW* w = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) /2);
  wattron(w, A_BOLD);
  mvwprintw(w, 1, 1, "Navigation:");
  wattroff(w, A_BOLD);
  mvwprintw(w, 2, 2, "Up, down, page up and page down can be used to navigate.");
  mvwprintw(w, 3, 2, "You can also use the mouse scroll wheel.");
  mvwprintw(w, 4, 1, "Press any key to get rid of this window.");
  box(w, 0, 0);
  wrefresh(w);
  delwin(w);
};