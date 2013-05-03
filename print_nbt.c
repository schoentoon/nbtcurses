#include "print_nbt.h"

#define SAFE_NAME(string) (string ? string : "NULL")

int nbt_fill_window(struct NBT_Window* w, nbt_node* node, unsigned char ident);

struct NBT_Window* newNBTWindow(nbt_node* node, int height, int width, int starty, int startx) {
  struct NBT_Window* output = malloc(sizeof(struct NBT_Window));
  output->window = newwin(height, width, starty, startx);
  scrollok(output->window, TRUE);
  output->panel = new_panel(output->window);
  output->height = height;
  output->width = width;
  output->nbt = node;
  output->current_line = 0;
  output->last_line = 0;
  output->buffer = malloc(sizeof(char*));
  nbt_fill_window(output, node, 0);
  redrawNBTWindow(output);
  update_panels();
  doupdate();
  return output;
};

char* NBTNodeToString(nbt_node* node) {
  char buf[BUFSIZ];
  switch (node->type) {
  case TAG_BYTE:
    snprintf(buf, sizeof(buf), "byte('%s') %d", SAFE_NAME(node->name), node->payload.tag_byte);
    break;
  case TAG_SHORT:
    snprintf(buf, sizeof(buf), "short('%s') %d", SAFE_NAME(node->name), node->payload.tag_short);
    break;
  case TAG_INT:
    snprintf(buf, sizeof(buf), "int('%s') %d", SAFE_NAME(node->name), node->payload.tag_int);
    break;
  case TAG_LONG:
    snprintf(buf, sizeof(buf), "long('%s') %ld", SAFE_NAME(node->name), node->payload.tag_long);
    break;
  case TAG_FLOAT:
    snprintf(buf, sizeof(buf), "float('%s') %f", SAFE_NAME(node->name), node->payload.tag_float);
    break;
  case TAG_DOUBLE:
    snprintf(buf, sizeof(buf), "double('%s') %f", SAFE_NAME(node->name), node->payload.tag_double);
    break;
  case TAG_BYTE_ARRAY:
  case TAG_INT_ARRAY:
  case TAG_INVALID:
    snprintf(buf, sizeof(buf), "Unsupported tag '%s'", SAFE_NAME(node->name));
    break;
  case TAG_STRING:
    snprintf(buf, sizeof(buf), "string('%s') %s", SAFE_NAME(node->name), node->payload.tag_string);
    break;
  case TAG_LIST:
    snprintf(buf, sizeof(buf), "list('%s')", SAFE_NAME(node->name));
    break;
  case TAG_COMPOUND:
    snprintf(buf, sizeof(buf), "compound('%s')", SAFE_NAME(node->name));
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
  w->buffer[w->last_line++] = NBTNodeToString(node);
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
  int i, j;
  for (i = nbt_window->current_line, j = 0; i < nbt_window->last_line && j < nbt_window->height; i++, j++) {
    fprintf(stderr, "%s\n", nbt_window->buffer[i]);
    mvwprintw(nbt_window->window, j, 0, "%s\n", nbt_window->buffer[i]);
  }
  wrefresh(nbt_window->window);
};