#ifndef _PRINT_NBT_H
#define _PRINT_NBT_H

#include "nbt.h"

#include <ncurses.h>

int nbt_wprint(WINDOW* window, const nbt_node* tree, unsigned char ident);

#endif //_PRINT_NBT_H