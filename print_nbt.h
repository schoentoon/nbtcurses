/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#ifndef _PRINT_NBT_H
#define _PRINT_NBT_H

#include "nbt.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <menu.h>

struct NBT_Window {
  MENU* menu;
  ITEM** items;
  nbt_node* nbt;
  unsigned short height, width;
  unsigned short last_line;
};

struct NBT_Window* newNBTWindow(nbt_node* node);

#endif //_PRINT_NBT_H