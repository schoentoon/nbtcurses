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
  output->items = NULL;
  nbt_fill_window(output, node, 0);
  output->menu = new_menu(output->items);
  return output;
};

void printNBTWindow(struct NBT_Window* window) {
  menu_opts_off(window->menu, O_SHOWDESC);
  set_menu_format(window->menu, LINES - 1, 0);
  post_menu(window->menu);
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
      return snprintf(buf, len, "%s string('%s') \"%s\"", prefix, node->name, node->payload.tag_string);
    return snprintf(buf, len, "%s string(NULL) \"%s\"", prefix, node->payload.tag_string);
  case TAG_LIST:
    if (node->name)
      return snprintf(buf, len, "%s list('%s') [%zu items]", prefix, node->name, list_length(&node->payload.tag_list->entry));
    return snprintf(buf, len, "%s list(NULL) [%zu items]", prefix, list_length(&node->payload.tag_list->entry));
  case TAG_COMPOUND:
    if (node->name)
      return snprintf(buf, len, "%s compound('%s') [%zu items]", prefix, node->name, list_length(&node->payload.tag_compound->entry));
    return snprintf(buf, len, "%s compound(NULL) [%zu items]", prefix, list_length(&node->payload.tag_compound->entry));
  }
  return 0;
};

ITEM* NBTNodeToItem(nbt_node* node, char* prefix) {
  char buf[BUFSIZ];
  if (printNBTtoBuffer(buf, sizeof(buf), node, prefix)) {
    char* item_name = malloc(strlen(buf) + 1);
    ITEM* output = new_item(strcpy(item_name, buf), NULL);
    set_item_userptr(output, node);
    return output;
  }
  return NULL;
};

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident) {
  w->items = realloc(w->items, (++w->last_line + 1) * sizeof(ITEM*));
  char prefix[257];
  unsigned char c;
  for (c = 0; c < ident; c++) { /* Simply hacking a tree view together over here.. */
    prefix[c*2] = '|';
    prefix[(c*2)+1] = ' ';
  }
  prefix[--c*2] = '`';
  prefix[(c*2)+1] = '-';
  prefix[++c*2] = '\0';
  w->items[w->last_line - 1] = NBTNodeToItem(node, prefix);
  w->items[w->last_line] = NULL;
  switch (node->type) {
  case TAG_LIST: {
    struct list_head* pos;
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

nbt_node* nbt_get_string(nbt_node* node, char* get_string) {
  if (node == NULL)
    return NULL;
  else if (node->name == NULL || strcmp(node->name, "") == 0) {
    if (node->type == TAG_COMPOUND) {
      if (list_length(&node->payload.tag_compound->entry) == 1)
        return nbt_get_string(nbt_list_item(node, 0), get_string);
    } else if (node->type == TAG_LIST) {
      if (list_length(&node->payload.tag_list->entry) == 1)
        return nbt_get_string(nbt_list_item(node, 0), get_string);
    }
  } else if (*get_string) {
    char* g;
    for (g = get_string; *g; g++) {
      if (*g == '.') {
        *g = '\0';
        break;
      }
    }
    nbt_node* found = nbt_find_by_name(node, get_string);
    if (found)
      return nbt_get_string(found, ++g);
  } else
    return node;
  return NULL;
};