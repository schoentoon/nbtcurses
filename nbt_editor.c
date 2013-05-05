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
#include "print_nbt.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

#include <ncurses.h>

#define HEIGHT (LINES*0.5)
#define WIDTH (COLS*0.8)

ssize_t write_help(int fd, nbt_type type);

int set_new_nbt_data(char* filename, nbt_node* node);

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
    snprintf(keybuf, sizeof(keybuf), "Key: %s\n", node->name);
  switch (node->type) {
  case TAG_BYTE:
    snprintf(valuebuf, sizeof(valuebuf), "Byte: %d\n", node->payload.tag_byte);
    break;
  case TAG_SHORT:
    snprintf(valuebuf, sizeof(valuebuf), "Short: %d\n", node->payload.tag_short);
    break;
  case TAG_INT:
    snprintf(valuebuf, sizeof(valuebuf), "Int: %d\n", node->payload.tag_int);
    break;
  case TAG_LONG:
    snprintf(valuebuf, sizeof(valuebuf), "Long: %ld\n", node->payload.tag_long);
    break;
  case TAG_FLOAT:
    snprintf(valuebuf, sizeof(valuebuf), "Float: %f\n", node->payload.tag_float);
    break;
  case TAG_DOUBLE:
    snprintf(valuebuf, sizeof(valuebuf), "Double: %f\n", node->payload.tag_double);
    break;
  case TAG_STRING:
    snprintf(valuebuf, sizeof(valuebuf), "String: %s\n", node->payload.tag_string);
    break;
  default:
    break;
  }
  write(fd, keybuf, strlen(keybuf));
  write(fd, valuebuf, strlen(valuebuf));
  write_help(fd, node->type);
  char commandbuf[BUFSIZ];
  snprintf(commandbuf, sizeof(commandbuf), "%s %s", (getenv("EDITOR") ? getenv("EDITOR") : "vi"), tmpfilename);
  endwin();
  system(commandbuf);
  reset_prog_mode();
  refresh();
  if (set_new_nbt_data(tmpfilename, node)) {
    char prefix[BUFSIZ];
    const char* org = item->name.str;
    if (sscanf(org, "%[|`- ]", prefix) == 1) {
      prefix[strlen(prefix)-1] = '\0';
      char buf[BUFSIZ];
      if (printNBTtoBuffer(buf, sizeof(buf), node, prefix)) {
        if (org)
          free((char*) org);
        item->name.length = strlen(buf);
        item->name.str = malloc(item->name.length + 1);
        strcpy((char*) item->name.str, buf);
      }
    }
  }
  close(fd);
};

const char* key_help = "# To set the key to NULL simply get the line out of there, if it was there to begin with.\n"
                       "# If you want to add one use the following syntax 'Key: keyname', omit the ''s\n";
const char* byte_help = "# Usually used for booleans. (1 for true, 0 for false)\n# Range from -128 to 127\n";
const char* short_help = "# Range from -32,768 to 32,767\n";
const char* int_help = "# Range from -2,147,483,648 to 2,147,483,647\n";
const char* long_help = "# Range from -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807\n";
const char* float_help = "# Range from 1.40129846432481707e-45 to 3.40282346638528860e+38\n";
const char* double_help = "# Range from 4.94065645841246544e-324d to 1.79769313486231570e+308d\n";
const char* string_help = "# All ASCII characters should be supported, the NBT format supports UTF-8 I am not sure how well nbtcurses handles this.\n";

ssize_t write_help(int fd, nbt_type type) {
  write(fd, key_help, strlen(key_help));
  switch (type) {
  case TAG_BYTE:
    return write(fd, byte_help, strlen(byte_help));
  case TAG_SHORT:
    return write(fd, short_help, strlen(short_help));
  case TAG_INT:
    return write(fd, int_help, strlen(int_help));
  case TAG_LONG:
    return write(fd, long_help, strlen(long_help));
  case TAG_FLOAT:
    return write(fd, float_help, strlen(float_help));
  case TAG_DOUBLE:
    return write(fd, double_help, strlen(double_help));
  case TAG_STRING:
    return write(fd, string_help, strlen(string_help));
  default:
    return 0;
  }
};

int set_new_nbt_data(char* filename, nbt_node* node) {
  FILE* f = fopen(filename, "r");
  char line[BUFSIZ];
  if (node->name)
    free(node->name);
  node->name = NULL;
  int number_int;
  long number_long;
  double number_double;
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '#')
      continue;
    char value[BUFSIZ];
    errno = 0;
    if (sscanf(line, "Key: %s", value) == 1) {
      node->name = malloc(strlen(value) + 1);
      node->name = strcpy(node->name, value);
    } else if (sscanf(line, "Byte: %d", &number_int) == 1) {
      if (number_int < -128 || number_int > 127) {
        error("Byte %d is out of range (-128 to 127)", number_int);
        fclose(f);
        return 0;
      } else
        node->payload.tag_byte = number_int;
    } else if (sscanf(line, "Short: %d", &number_int) == 1) {
      if (number_int < -32768 || number_int > 32767) {
        error("Short %d is out of range (-32,768 to 32,767)", number_int);
        fclose(f);
        return 0;
      } else
        node->payload.tag_short = number_int;
    } else if (sscanf(line, "Int: %d", &number_int) == 1) {
      if (number_int < -2147483648 || number_int > 2147483647) {
        error("Int %d is out of range (2,147,483,648 to 2,147,483,647)", number_int);
        fclose(f);
        return 0;
      } else
        node->payload.tag_int = number_int;
    } else if (sscanf(line, "Long: %ld", &number_long) == 1) {
      if (errno != 0) {
        error("Error %s", strerror(errno));
        fclose(f);
        return 0;
      } else
        node->payload.tag_long = number_long;
    } else if (sscanf(line, "Float: %lf", &number_double) == 1) {
      if (errno != 0) {
        error("Error %s", strerror(errno));
        fclose(0);
        return 0;
      } else
        node->payload.tag_float = number_double;
    } else if (sscanf(line, "Double: %lf", &number_double) == 1) {
      if (errno != 0) {
        error("Error %s", strerror(errno));
        fclose(0);
        return 0;
      } else
        node->payload.tag_double = number_double;
    } else if (sscanf(line, "String: %[^\n]", value) == 1) {
      if (node->payload.tag_string)
        free(node->payload.tag_string);
      node->payload.tag_string = malloc(strlen(value) + 1);
      node->payload.tag_string = strcpy(node->payload.tag_string, value);
    }
  };
  fclose(f);
  return 1;
};