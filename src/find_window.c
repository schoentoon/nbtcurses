/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "find_window.h"

#include <strings.h>
#include <ncurses.h>

void enable_find(nbt_node* node) {
  move(LINES - 1, 0);
  clrtoeol();
  printw("Search: ");
  refresh();
  int ch = 0;
  char searchbuf[BUFSIZ];
  bzero(searchbuf, sizeof(searchbuf));
  char* s = searchbuf; /* Begin pointer */
  char* c = s; /* Current location */
  char* end = s + sizeof(searchbuf) - 1; /* Max location */
  while((ch = getch()) != 27) {
    if (ch == KEY_BACKSPACE) {
      *c = '\0';
      if (c != s)
        c--;
    } else {
      if (c < end) {
        *c = ch;
        c++;
      }
    }
    move(LINES - 1, 0);
    clrtoeol();
    printw("Search: %s", searchbuf);
    refresh();
  }
  refresh();
};