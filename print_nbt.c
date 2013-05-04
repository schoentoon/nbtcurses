/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "print_nbt.h"

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident);

struct NBT_Window* newNBTWindow(nbt_node* node, int height, int width, int starty, int startx) {
  struct NBT_Window* output = malloc(sizeof(struct NBT_Window));
  output->window = newwin(height, width, starty, startx);
  scrollok(output->window, TRUE);
  output->panel = new_panel(output->window);
  output->height = height;
  output->width = width;
  output->nbt = node;
  output->current_line = 1;
  output->last_line = 0;
  output->buffer = malloc(sizeof(char*));
  nbt_fill_window(output, node, 0);
  redrawNBTWindow(output);
  update_panels();
  doupdate();
  return output;
};

char* NBTNodeToString(nbt_node* node, char* prefix) {
  char buf[BUFSIZ];
  switch (node->type) {
  case TAG_BYTE:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s byte('%s') %d", prefix, node->name, node->payload.tag_byte);
    else
      snprintf(buf, sizeof(buf), "%s byte(NULL) %d", prefix, node->payload.tag_byte);
    break;
  case TAG_SHORT:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s short('%s') %d", prefix, node->name, node->payload.tag_short);
    else
      snprintf(buf, sizeof(buf), "%s short(NULL) %d", prefix, node->payload.tag_short);
    break;
  case TAG_INT:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s int('%s') %d", prefix, node->name, node->payload.tag_int);
    else
      snprintf(buf, sizeof(buf), "%s int(NULL) %d", prefix, node->payload.tag_int);
    break;
  case TAG_LONG:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s long('%s') %ld", prefix, node->name, node->payload.tag_long);
    else
      snprintf(buf, sizeof(buf), "%s long(NULL) %ld", prefix, node->payload.tag_long);
    break;
  case TAG_FLOAT:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s float('%s') %f", prefix, node->name, node->payload.tag_float);
    else
      snprintf(buf, sizeof(buf), "%s float(NULL) %f", prefix, node->payload.tag_float);
    break;
  case TAG_DOUBLE:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s double('%s') %f", prefix, node->name, node->payload.tag_double);
    else
      snprintf(buf, sizeof(buf), "%s double(NULL) %f", prefix, node->payload.tag_double);
    break;
  case TAG_BYTE_ARRAY:
  case TAG_INT_ARRAY: //TODO actually implement these types..
  case TAG_INVALID:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s Unsupported tag '%s'", prefix, node->name);
    else
      snprintf(buf, sizeof(buf), "%s Unsupported tag NULL", prefix);
    break;
  case TAG_STRING:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s string('%s') %s", prefix, node->name, node->payload.tag_string);
    else
      snprintf(buf, sizeof(buf), "%s string(NULL) %s", prefix, node->payload.tag_string);
    break;
  case TAG_LIST:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s list('%s')", prefix, node->name);
    else
      snprintf(buf, sizeof(buf), "%s list(NULL)", prefix);
    break;
  case TAG_COMPOUND:
    if (node->name)
      snprintf(buf, sizeof(buf), "%s compound('%s')", prefix, node->name);
    else
      snprintf(buf, sizeof(buf), "%s compound(NULL)", prefix);
    break;
  }
  if (buf[0] == 0)
    return NULL;
  char* output = malloc(strlen(buf) + 1);
  return strcpy(output, buf);
};

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident) {
  if (!node)
    return 0;
  w->buffer = (char**) realloc(w->buffer, (w->last_line + 1) * sizeof(char*));
  char prefix[BUFSIZ];
  memset(&prefix, 0, sizeof(prefix));
  unsigned char c;
  for (c = 0; c < ident; c++) { /* Simply hacking a tree view together over here.. */
    prefix[c*2] = '|';
    prefix[(c*2)+1] = ' ';
  }
  c--;
  prefix[c*2] = '`';
  prefix[(c*2)+1] = '-';
  w->buffer[w->last_line++] = NBTNodeToString(node, prefix);
  switch (node->type) {
  case TAG_LIST: {
    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
      const struct nbt_list* entry = list_entry(pos, const struct nbt_list, entry);
      nbt_fill_window(w, entry->data, ident + 1);
    }
    break;
  }
  case TAG_COMPOUND: {
    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_compound->entry) {
      const struct nbt_list* entry = list_entry(pos, const struct nbt_list, entry);
      nbt_fill_window(w, entry->data, ident + 1);
    }
    break;
  }
  default:
    break;
  };
  return 1;
};

void redrawNBTWindow(struct NBT_Window* nbt_window) {
  wclear(nbt_window->window);
  int first_line = nbt_window->current_line;
  if ((nbt_window->height / 2) > first_line)
    first_line = 0;
  else
    first_line = nbt_window->current_line - (nbt_window->height / 2);
  int i, j;
  for (i = first_line, j = 0; i < nbt_window->last_line && j < nbt_window->height; i++, j++) {
    if (i == nbt_window->current_line) {
      wattron(nbt_window->window, A_REVERSE);
      mvwprintw(nbt_window->window, j, 0, "%s\n", nbt_window->buffer[i]);
      wattroff(nbt_window->window, A_REVERSE);
    } else
      mvwprintw(nbt_window->window, j, 0, "%s\n", nbt_window->buffer[i]);
  }
  wrefresh(nbt_window->window);
};