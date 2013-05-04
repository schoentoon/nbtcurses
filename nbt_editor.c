/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "nbt_editor.h"

#include "colors.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <ncurses.h>

#define HEIGHT (LINES*0.5)
#define WIDTH (COLS*0.8)

void show_edit_window(nbt_node* node, ITEM* item) {
  if (!node || node->type == TAG_BYTE_ARRAY || node->type == TAG_COMPOUND || node->type == TAG_INT_ARRAY
    || node->type == TAG_INVALID || node->type == TAG_LIST) {
    error("This type is currently not supported.");
    return;
  }
  char tmpfilename[] = "/tmp/nbtcurses.XXXXXX";
  int fd = mkstemp(tmpfilename);
  if (fd < 0) {
    error("There was an error while creating a temporary file %s.", strerror(errno));
    return;
  }
  char keybuf[BUFSIZ];
  char valuebuf[BUFSIZ];
  if (node->name)
    snprintf(keybuf, sizeof(keybuf), "Key: '%s'", node->name);
  else
    snprintf(keybuf, sizeof(keybuf), "Key: NULL");
  switch (node->type) {
  case TAG_BYTE:
    snprintf(valuebuf, sizeof(valuebuf), "Byte: %d", node->payload.tag_byte);
    break;
  case TAG_SHORT:
    snprintf(valuebuf, sizeof(valuebuf), "Short: %d", node->payload.tag_short);
    break;
  case TAG_INT:
    snprintf(valuebuf, sizeof(valuebuf), "Int: %d", node->payload.tag_int);
    break;
  case TAG_LONG:
    snprintf(valuebuf, sizeof(valuebuf), "Long: %ld", node->payload.tag_long);
    break;
  case TAG_FLOAT:
    snprintf(valuebuf, sizeof(valuebuf), "Float: %f", node->payload.tag_float);
    break;
  case TAG_DOUBLE:
    snprintf(valuebuf, sizeof(valuebuf), "Double: %f", node->payload.tag_double);
    break;
  case TAG_STRING:
    snprintf(valuebuf, sizeof(valuebuf), "String: %s", node->payload.tag_string);
    break;
  default:
    break;
  }
  write(fd, keybuf, strlen(keybuf));
  write(fd, "\n", 1);
  write(fd, valuebuf, strlen(valuebuf));
  write(fd, "\n", 1);
  char commandbuf[BUFSIZ];
  snprintf(commandbuf, sizeof(commandbuf), "%s %s", (getenv("EDITOR") ? getenv("EDITOR") : "vi"), tmpfilename);
  endwin();
  system(commandbuf);
  reset_prog_mode();
  refresh();
  close(fd);
};