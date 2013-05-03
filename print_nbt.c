#include "print_nbt.h"

#define SAFE_NAME(string) (string ? string : "NULL")

int nbt_wprint(WINDOW* window, const nbt_node* tree, unsigned char ident) {
  if (!tree)
    return 0;
  unsigned char c;
  for (c = 0; c < ident; c++)
    wprintw(window, " ");
  switch (tree->type) {
  case TAG_BYTE:
    wprintw(window, "byte('%s') %d\n", SAFE_NAME(tree->name), tree->payload.tag_byte);
    break;
  case TAG_SHORT:
    wprintw(window, "short('%s') %d\n", SAFE_NAME(tree->name), tree->payload.tag_short);
    break;
  case TAG_INT:
    wprintw(window, "int('%s') %d\n", SAFE_NAME(tree->name), tree->payload.tag_int);
    break;
  case TAG_LONG:
    wprintw(window, "long('%s') %d\n", SAFE_NAME(tree->name), tree->payload.tag_long);
    break;
  case TAG_FLOAT:
    wprintw(window, "float('%s') %f\n", SAFE_NAME(tree->name), tree->payload.tag_float);
    break;
  case TAG_DOUBLE:
    wprintw(window, "double('%s') %f\n", SAFE_NAME(tree->name), tree->payload.tag_double);
    break;
  case TAG_BYTE_ARRAY:
  case TAG_INT_ARRAY:
  case TAG_INVALID:
    wprintw(window, "Unsupported tag '%s'\n", SAFE_NAME(tree->name));
    break;
  case TAG_STRING:
    wprintw(window, "string('%s') %s\n", SAFE_NAME(tree->name), tree->payload.tag_string);
    break;
  case TAG_LIST: {
    wprintw(window, "list('%s')\n", SAFE_NAME(tree->name));
    const struct list_head* pos;
    list_for_each(pos, &tree->payload.tag_list->entry) {
      const struct nbt_list* entry = list_entry(pos, const struct nbt_list, entry);
      nbt_wprint(window, entry->data, ident + 1);
    }
    break;
  }
  case TAG_COMPOUND: {
    wprintw(window, "compound('%s')\n", SAFE_NAME(tree->name));
    const struct list_head* pos;
    list_for_each(pos, &tree->payload.tag_compound->entry) {
      const struct nbt_list* entry = list_entry(pos, const struct nbt_list, entry);
      nbt_wprint(window, entry->data, ident + 1);
    }
    break;
  }
  };
  wrefresh(window);
  return 1;
};