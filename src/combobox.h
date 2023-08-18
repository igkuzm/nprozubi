/**
 * File              : combobox.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 23.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef COMBOBOX_H
#define COMBOBOX_H
#include <string.h>
#include <newt.h>
#include <stdio.h>
#include "../prozubilib/cases.h"

static int
combobox_new(
			struct case_list_node *n)
{
	int i=0, ret=-1, key, y=0, cols=30, d, rows;
	// get rows - count array
	while (n->array[i])
		i++;
	rows = i + 1;

	newtComponent form, ans=NULL, rb[i];
	struct newtExitStruct toexit;

	newtCenteredWindow(cols, rows, n->title);
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, 'q');

	// create radio buttons
	char *value = (char *)prozubi_case_get(n->c, n->key); 
	if (!value) value = "";
	i = 0;
	newtComponent b = NULL;
	while (n->array[i]){
		char *str = n->array[i];
		d = 0;
		if (strcmp(str, value) == 0)
			d = 1;
		b = newtRadiobutton(1, i, str, d, b);
		newtFormAddComponent(form, b);
		rb[i++] = b;
	}

	// start form
	do{
		newtFormRun(form, &toexit);
		key = toexit.u.key;
		if (key == NEWT_KEY_ESCAPE || key == 'q')
			break;
		if (key == NEWT_KEY_RETURN){
			i=0;
			while (n->array[i]){
				if(newtRadioGetCurrent(rb[0]) == rb[i])
					ret = i;
				i++;
			}
			break;
		}
	} while (key != NEWT_KEY_F10);
	
	// destroy form
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}
#endif /* ifndef COMBOBOX_H */
