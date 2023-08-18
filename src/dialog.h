/**
 * File              : dialog.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DIALOG_H
#define DIALOG_H
#include <string.h>
#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int wcwidth(wchar_t c);

static int
dialog(
		const char *title,
		const char *msg, 
		const char *buttons[], int count)
{
	int ret = -1;
	int i, key, x=0, y=0, cols=0, max=0, rows = 6;
	struct newtExitStruct toexit;

	// count cols
	for (i = 0; i < count; ++i) {
		int len = 0;
		char *p = (char *)buttons[i];
		wchar_t c;
		mbstate_t ps;
		while(*p){
			int ret = mbrtowc(&c, p, MB_CUR_MAX, &ps);
			len += wcwidth(c);
			p += ret;
		}
		if (len > max)
			max = len;
	}
	cols = max * count + 2 + count;

	newtComponent form, text;
	newtComponent array[count];

	newtCenteredWindow(cols, rows, title);
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

	text = newtTextboxReflowed(1, 1, (char *)msg, cols-2, rows-4, 0, 0);
	newtFormAddComponents(form, text, NULL);
	
	for (i = 0; i < count; ++i) {
		newtComponent b = newtCompactButton(x, rows-1, buttons[i]);
		array[i] = b;
		newtFormAddComponent(form, b);
		x += max + 1;
	}

	newtFormRun(form, &toexit);
	key = toexit.u.key;
	if (key == NEWT_KEY_F10 ||
			key == NEWT_KEY_ESCAPE ||
			key == 'q')
	{
		// ret = -1;
	} else {
		newtComponent ans = newtFormGetCurrent(form);
		for (i = 0; i < count; ++i){
			if (ans == array[i]){
				ret = i;
				break;
			}	
		}
	}
	
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}

#endif /* ifndef DIALOG_H */
