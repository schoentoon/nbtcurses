#ifndef _PRINT_NBT_H
#define _PRINT_NBT_H

#include "nbt.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <panel.h>

struct NBT_Window {
  WINDOW* window;
  PANEL* panel;
  nbt_node* nbt;
  unsigned short current_line;
  unsigned short height, width;
  char** buffer;
  unsigned short last_line;
};

struct NBT_Window* newNBTWindow(nbt_node* node, int height, int width, int starty, int startx);

void redrawNBTWindow(struct NBT_Window* nbt_window);

#endif //_PRINT_NBT_H