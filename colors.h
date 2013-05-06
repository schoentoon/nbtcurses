/* * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Toon Schoenmakers <nighteyes1993@gmail.com>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If we meet some day, and you think this stuff is worth
 * it, you can buy us a beer in return.
 * -----------------------------------------------------------------------------
 */

#ifndef _COLORS_H
#define _COLORS_H

#define IMPORTANT_COLOR_PAIR 1
#define IMPORTANT A_REVERSE|A_BOLD|COLOR_PAIR(IMPORTANT_COLOR_PAIR)
#define NOTICE_COLOR_PAIR 2
#define NOTICE A_REVERSE|COLOR_PAIR(NOTICE_COLOR_PAIR)

void error(char* format, ...);

void notice(char* format, ...);

#endif //_COLORS_H