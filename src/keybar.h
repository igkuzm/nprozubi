/**
 * File              : keybar.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 25.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef KEYBAR_H
#define KEYBAR_H

#include <newt.h>
#include <stdio.h>
#include <string.h>

#include "colors.h"

#define KEYBAR_KEYS\
	KEYBAR_KEY(1)\
	KEYBAR_KEY(2)\
	KEYBAR_KEY(3)\
	KEYBAR_KEY(4)\
	KEYBAR_KEY(5)\
	KEYBAR_KEY(6)\
	KEYBAR_KEY(7)\
	KEYBAR_KEY(8)\
	KEYBAR_KEY(9)\
	KEYBAR_KEY(10)

static newtComponent  f_1,  f_2,  f_3,  f_4,  f_5,  f_6,  f_7,  f_8,  f_9,  f_10;

static const char * keybarTitles[] = 
{
	"Print",
	"Menu",
	"Find",
	"Edit",
	"Copy",
	"Move",
	"Add/Save",
	"Delete",
	"PullDn",
	"Quit",
};

static const char * keybarLables[] = 
{
	" 1",
	" 2",
	" 3",
	" 4",
	" 5",
	" 6",
	" 7",
	" 8",
	" 9",
	"10",
};

static void keybar_new(newtComponent form, int cols, int rows)
{
	int i, x=0, lwidth=2, fwidth;
	fwidth = (cols - (lwidth * 10))/10;

	//for (i = 0; i < 32; ++i) {
		//char text[2];
		//sprintf(text, "%d", i);
		//newtComponent l = newtLabel(y, rows-1, text);\
		//newtLabelSetColors(l, NEWT_COLORSET_CUSTOM(i));\
		//newtFormAddComponent(form, l);
		//y+=2;
	//}

	//return;
	
#define KEYBAR_KEY(n)\
	{\
		newtComponent\
		l = newtLabel(x, rows-1, keybarLables[n-1]);\
		newtLabelSetColors(l, NEWT_COLORSET_CUSTOM(WHITEONBLACK));\
		newtFormAddComponent(form, l);\
		x+=lwidth;\
		int len = strlen(keybarTitles[n-1]);\
		char text[fwidth + 2];\
		text[fwidth] = 0;\
		snprintf(text, fwidth, "%s", keybarTitles[n-1]);\
		for (i = len; i < fwidth; ++i)\
			text[i] = ' ';\
		f_##n  = newtLabel(x, rows-1, text);\
		newtLabelSetColors(f_##n, NEWT_COLORSET_CUSTOM(BLACKONCYAN));\
		newtFormAddComponent(form, f_##n);\
		x+=fwidth;\
	}
KEYBAR_KEYS
#undef KEYBAR_KEY
}

#endif /* ifndef KEYBAR_H */
