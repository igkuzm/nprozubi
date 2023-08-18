/**
 * File              : askstring.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef ASKSTRING_H
#define ASKSTRING_H
#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
ask_string(char *msg)
{
	char *ret = NULL;
	int i, key, y=0, cols=24, rows = 7;
	newtComponent form, text, entry, ok, cancel, ans=NULL;
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, "Введите строку");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);

	const char *value;

	text = newtTextboxReflowed(1, 1, msg, cols-2, rows-4, 0, 0);
	entry = newtEntry(1, rows-3, "", cols-2, &value, NEWT_FLAG_RETURNEXIT);
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
			ret = strdup(value);
			break;
		}

		if (ans == cancel){
			// exit no save
			break;
		}
			
		if (key == NEWT_KEY_ESCAPE){
			// exit without save
			break;
		}

	} while (key != NEWT_KEY_F10);
	
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}


#endif /* ifndef ASKSTRING_H */
