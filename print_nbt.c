/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#include "print_nbt.h"

#include "colors.h"

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident);

struct NBT_Window* newNBTWindow(nbt_node* node) {
  struct NBT_Window* output = malloc(sizeof(struct NBT_Window));
  output->nbt = node;
  output->last_line = 0;
  output->items = malloc(sizeof(ITEM*));
  nbt_fill_window(output, node, 0);
  output->items[output->last_line+1] = NULL;
  output->menu = new_menu(output->items);
  set_menu_format(output->menu, LINES - 1, 0);
  post_menu(output->menu);
  return output;
};

int printNBTtoBuffer(char* buf, size_t len, nbt_node* node, char* prefix) {
  switch (node->type) {
  case TAG_BYTE:
    if (node->name)
      return snprintf(buf, len, "%s byte('%s') %d", prefix, node->name, node->payload.tag_byte);
    return snprintf(buf, len, "%s byte(NULL) %d", prefix, node->payload.tag_byte);
  case TAG_SHORT:
    if (node->name)
      return snprintf(buf, len, "%s short('%s') %d", prefix, node->name, node->payload.tag_short);
    return snprintf(buf, len, "%s short(NULL) %d", prefix, node->payload.tag_short);
  case TAG_INT:
    if (node->name)
      return snprintf(buf, len, "%s int('%s') %d", prefix, node->name, node->payload.tag_int);
    return snprintf(buf, len, "%s int(NULL) %d", prefix, node->payload.tag_int);
  case TAG_LONG:
    if (node->name)
      return snprintf(buf, len, "%s long('%s') %ld", prefix, node->name, node->payload.tag_long);
    return snprintf(buf, len, "%s long(NULL) %ld", prefix, node->payload.tag_long);
  case TAG_FLOAT:
    if (node->name)
      return snprintf(buf, len, "%s float('%s') %f", prefix, node->name, node->payload.tag_float);
    return snprintf(buf, len, "%s float(NULL) %f", prefix, node->payload.tag_float);
  case TAG_DOUBLE:
    if (node->name)
      return snprintf(buf, len, "%s double('%s') %f", prefix, node->name, node->payload.tag_double);
    return snprintf(buf, len, "%s double(NULL) %f", prefix, node->payload.tag_double);
  case TAG_BYTE_ARRAY:
  case TAG_INT_ARRAY: //TODO actually implement these types..
  case TAG_INVALID:
    if (node->name)
      return snprintf(buf, len, "%s Unsupported tag '%s'", prefix, node->name);
    return snprintf(buf, len, "%s Unsupported tag NULL", prefix);
  case TAG_STRING:
    if (node->name)
      return snprintf(buf, len, "%s string('%s') %s", prefix, node->name, node->payload.tag_string);
    return snprintf(buf, len, "%s string(NULL) %s", prefix, node->payload.tag_string);
  case TAG_LIST:
    if (node->name)
      return snprintf(buf, len, "%s list('%s')", prefix, node->name);
    return snprintf(buf, len, "%s list(NULL)", prefix);
  case TAG_COMPOUND:
    if (node->name)
      return snprintf(buf, len, "%s compound('%s')", prefix, node->name);
    return snprintf(buf, len, "%s compound(NULL)", prefix);
  }
};

ITEM* NBTNodeToItem(nbt_node* node, char* prefix) {
  char buf[BUFSIZ];
  if (printNBTtoBuffer(buf, sizeof(buf), node, prefix)) {
    char* str = malloc(strlen(buf) + 1);
    ITEM* output = new_item(strcpy(str, buf), "");
    set_item_userptr(output, node);
    return output;
  }
  return NULL;
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