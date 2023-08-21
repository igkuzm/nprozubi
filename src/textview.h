/**
 * File              : textview.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.08.2023
 * Last Modified Date: 21.08.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef TEXTVIEW_H
#define TEXTVIEW_H
#include "asktoremove.h"
#include <string.h>
#include <newt.h>
#include <stdio.h>

static int
textview(
		const char *title,
		const char *msg)
{
	int i, cols=40, rows = 10, key, ret=0;
	struct newtExitStruct toexit;

	newtComponent form, text;
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_F8);
	newtFormAddHotKey(form, NEWT_KEY_F4);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'e');
	newtFormAddHotKey(form, 'd');


	text = newtTextboxReflowed(1, 1, (char *)msg, cols-2, rows, 0, 0);
	int lines = newtTextboxGetNumLines(text);
	
	if (rows < lines + 1)
		rows = lines + 1;
	newtCenteredWindow(cols, rows, title);

	newtFormAddComponents(form, text, NULL);
	
	newtFormRun(form, &toexit);
	key = toexit.u.key;
	if (key == NEWT_KEY_F10 ||
			key == NEWT_KEY_ESCAPE ||
			key == NEWT_KEY_RETURN ||
			key == 'q')
	{
		//do nothing;
	} else if (key == NEWT_KEY_F4 || key == 'e'){
		ret = 1;
	} else if (key == NEWT_KEY_F8 || key == 'd'){
		if (ask_to_remove((char *)title))
			ret = -1;
	}
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}

#endif /* ifndef TEXTVIEW_H */
