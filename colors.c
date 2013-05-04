#include "colors.h"

#include <stdarg.h>

#include <ncurses.h>

void error(char* format, ...) {
  wattron(stdscr, IMPORTANT);
  move(LINES - 1, 0);
  clrtoeol();
  va_list arg;
  va_start(arg, format);
  printw(format, arg);
  va_end(arg);
  wattroff(stdscr, IMPORTANT);
  refresh();
};