/**
 * File              : askindex.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef ASKINDEX_H
#define ASKINDEX_H
#include <newt.h>
#include <stdio.h>
#include <stdlib.h>

static int ask_index_filter(
		newtComponent entry, void * data, int c,
			       int p)
{
	if (c == NEWT_KEY_LEFT    ||
			c == NEWT_KEY_RIGHT   ||
			c == NEWT_KEY_DELETE  ||
			c == NEWT_KEY_BKSPC   ||
			c == NEWT_KEY_HOME    ||
			c == NEWT_KEY_ENTER   ||
			c == NEWT_KEY_UP      ||
			c == NEWT_KEY_DOWN    ||
			c == NEWT_KEY_END)
		return c;

	if (c >= '0' && c<= '9')
		return c;

	return 0;
}

static int
ask_index(char *str)
{
	int ret = -1;
	int i, key, y=0, cols=24, rows = 6;
	newtComponent form, text, entry, ok, cancel, ans=NULL;
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, "Введите номер");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);

	const char *value;

	text = newtTextboxReflowed(1, 1, str, cols-2, rows-4, 0, 0);
	entry = newtEntry(1, rows-3, "", cols-2, &value, NEWT_FLAG_RETURNEXIT);
	newtEntrySetFilter(entry, ask_index_filter, NULL);\
	cancel = newtCompactButton(1, rows-1, "Отмена");
	ok = newtCompactButton(cols-11, rows-1, "Ok");

	newtFormAddComponents(form, text, entry, cancel, ok, NULL);

	// main loop
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;

		if (ans == ok || ans == entry) {
			// do ask again
			ret = atoi(value);
			break;
		}

		if (ans == cancel){
			// exit no save
			break;
		}
			
		if (key == NEWT_KEY_ESCAPE || key == 'q'){
			// exit without save
			break;
		}

	} while (key != NEWT_KEY_F10);
	
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}


#endif /* ifndef ASKINDEX_H */
