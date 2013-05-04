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
  output->height = height;
  output->width = width;
  output->nbt = node;
  output->last_line = 0;
  output->items = malloc(sizeof(ITEM*));
  nbt_fill_window(output, node, 0);
  output->menu = new_menu(output->items);
  set_menu_format(output->menu, height, 0);
  post_menu(output->menu);
  wrefresh(output->window);
  return output;
};

ITEM* NBTNodeToItem(nbt_node* node, char* prefix) {
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
  char* str = malloc(strlen(buf) + 1);
  ITEM* output = new_item(strcpy(str, buf), "");
  set_item_userptr(output, node);
  return output;
};

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident) {
  if (!node)
    return 0;
  w->items = realloc(w->items, (w->last_line + 1) * sizeof(ITEM*));
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
  w->items[w->last_line++] = NBTNodeToItem(node, prefix);
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