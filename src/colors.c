#include "colors.h"

#include <stdarg.h>

#include <ncurses.h>

void error(char* format, ...) {
  wattron(stdscr, IMPORTANT);
  move(LINES - 1, 0);
  clrtoeol();
  va_list arg;
  va_start(arg, format);
  vwprintw(stdscr, format, arg);
  va_end(arg);
  wattroff(stdscr, IMPORTANT);
  refresh();
};

void notice(char* format, ...) {
  wattron(stdscr, NOTICE);
  move(LINES - 1, 0);
  clrtoeol();
  va_list arg;
  va_start(arg, format);
  vwprintw(stdscr, format, arg);
  va_end(arg);
  wattroff(stdscr, NOTICE);
  refresh();
};