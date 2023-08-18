/**
 * File              : asktoremove.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef ASKTOREMOVE_H
#define ASKTOREMOVE_H
#include <newt.h>
#include <stdio.h>

static bool
_ask_to_remove_confirm()
{
	bool ret = false;
	int i, key, y=0, cols=20, rows = 5;
	newtComponent form, text, ok, cancel, ans=NULL;
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, "удалить?");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

	text = newtLabel(1, 1, "Вы уверены?");
	cancel = newtCompactButton(0, rows-1, "Отмена");
	ok = newtCompactButton(cols-10, rows-1, "Удалить");

	newtFormAddComponents(form, text, cancel, ok, NULL);

	newtFormRun(form, &toexit);
	ans = toexit.u.co;
	
	if (ans == ok)
		ret = true;

	newtFormDestroy(form);
	newtPopWindow();

	return ret;	
}

static bool
ask_to_remove(char *str)
{
	bool ret = false;
	int i, key, y=0, cols=24, rows = 10;
	newtComponent form, text, ok, cancel, ans=NULL;
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, "удалить?");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

	text = newtTextboxReflowed(1, 1, str, cols-2, rows-2, 0, 0);
	cancel = newtCompactButton(1, rows-1, "Отмена");
	ok = newtCompactButton(cols-11, rows-1, "Удалить");

	newtFormAddComponents(form, text, cancel, ok, NULL);

	// main loop
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;

		if (ans == ok) {
			// do ask again
			ret = _ask_to_remove_confirm();
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


#endif /* ifndef ASKTOREMOVE_H */
