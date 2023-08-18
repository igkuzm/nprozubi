/**
 * File              : error.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef ERROR_H
#define ERROR_H
#include <newt.h>
#include <stdio.h>
#include <string.h>

static void error_callback(void *userdata, const char * error){
	int cols=24, rows = 10, key;
	struct newtExitStruct toexit;
	newtComponent form, text, ok;

	newtCenteredWindow(cols, rows, "Ошибка");
	
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

	text = newtTextboxReflowed(1, 1, (char *)error, cols-2, rows-2, 0, 0);
	ok = newtCompactButton(cols-11, rows-1, "Ok");

	newtFormAddComponents(form, text, ok, NULL);

	// main loop
	newtRunForm(form);
	
	newtFormDestroy(form);
	newtPopWindow();
}
static void log_callback(void *userdata, const char * msg){
	FILE *fp = fopen("prozubi.log", "a+");
	fwrite(msg, strlen(msg), 1, fp);
	fwrite("\n", 1, 1, fp);
	fclose(fp);
}
#endif /* ifndef ERROR_H */
