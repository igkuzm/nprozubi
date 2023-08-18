/**
 * File              : asktoexit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef ASK_TO_EXIT
#define ASK_TO_EXIT
#include <newt.h>
#include <stdio.h>

static bool
ask_to_exit()
{
	bool ret = false;
	int i, key, y=0, cols=20, rows = 5;
	newtComponent form, text, ok, cancel, ans=NULL;
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, "Закрыть?");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

	text = newtLabel(1, 1, "Закрыть приложение?");
	cancel = newtCompactButton(0, rows-1, "Отмена");
	ok = newtCompactButton(cols-10, rows-1, "Закрыть");

	newtFormAddComponents(form, text, cancel, ok, NULL);

	newtFormRun(form, &toexit);
	ans = toexit.u.co;
	
	if (ans == ok)
		ret = true;

	newtFormDestroy(form);
	newtPopWindow();

	return ret;	
}
#endif /* ifndef ASK_TO_EXIT */
