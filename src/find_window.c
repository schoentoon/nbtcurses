/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "find_window.h"

#include <menu.h>
#include <ctype.h>
#include <strings.h>
#include <ncurses.h>

char matchStartString(char* searchterm, size_t searchlen, char* name);

int search_item(char* searchterm, int startfrom, struct NBT_Window* window);

void enable_find(struct NBT_Window* window) {
  move(LINES - 1, 0);
  clrtoeol();
  printw("Search: ");
  refresh();
  int ch = 0;
  char searchbuf[BUFSIZ];
  bzero(searchbuf, sizeof(searchbuf));
  const char* s = searchbuf; /* Begin pointer */
  char* c = searchbuf; /* Current location */
  const char* end = s + sizeof(searchbuf) - 1; /* Max location */
  int found;
  while((ch = getch()) != 27) {
    if (ch == KEY_BACKSPACE || isalnum(ch)) {
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
      found = search_item(searchbuf, 0, window);
      if (found != -1)
        set_current_item(window->menu, window->items[found]);
    } else if (KEY_F(3)) {
      found = search_item(searchbuf, found + 1, window);
      if (found != -1)
        set_current_item(window->menu, window->items[found]);
      else {
        move(LINES - 1, 0);
        clrtoeol();
        printw("Search: %s [No more results]", searchbuf);
      }
    }
    refresh();
  }
  refresh();
};

char matchStartString(char* searchterm, size_t searchlen, char* name) {
  if (name == NULL) {
    static const char* NULLSTR = "NULL";
    if (strcmp(searchterm, NULLSTR) == 0)
      return 1;
    return 0;
  }
  const size_t namelen = strlen(name);
  if (namelen == 0 && searchlen > 0)
    return 0;
  const size_t lowest = (namelen < searchlen) ? namelen : searchlen;
  size_t i;
  for (i = 0; i < lowest; i++) {
    if (name[i] != searchterm[i])
      return 0;
  }
  return 1;
};

int search_item(char* searchterm, int startfrom, struct NBT_Window* window) {
  const size_t searchlen = strlen(searchterm);
  int i = (startfrom > 0) ? startfrom : 0;
  ITEM* item;
  for (item = window->items[i]; item; item = window->items[++i]) {
    nbt_node* node = item_userptr(item);
    if (matchStartString(searchterm, searchlen, node->name) == 1)
      return i;
  }
  return -1;
};