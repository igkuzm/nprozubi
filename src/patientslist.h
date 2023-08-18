/**
 * File              : patientslist.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PATIENTS_LIST_H
#define PATIENTS_LIST_H

#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include "../prozubilib/prozubilib.h"
#include "helpers.h"
#include "main.h"

static int 
get_patients(void *data, struct passport_t *patient){
	newtComponent list = (newtComponent)data;
	int count = newtListboxItemCount(list);
	char title[256];
	PATIENT(title, count, patient);
	newtListboxAppendEntry(list, title, patient);
	return 0;
}

static void
patients_list_new(
		prozubi_t *p, 
		newtComponent form,
		newtComponent *list,
		SWITCH *selected,
		const char *search,
		int cols, int rows)
{

	if (*list){
		// free list
		on_destroy(*list, selected);
		newtListboxClear(*list);
	} else {
		// create new
		*list =
				newtListbox(30, 0, rows-1, NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
		newtFormAddComponent(form, *list);
	} 
	newtListboxSetWidth(*list, cols-30);
	newtComponentAddDestroyCallback(*list, on_destroy, selected);

	// fill list with patients
	char *predicate = NULL;
	if (search){
		predicate = (char *)malloc(BUFSIZ);
		if (!predicate){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		sprintf(predicate, 
				"WHERE ZFAMILIYA LIKE '%%%s%%' "
				"OR ZIMIA LIKE '%%%s%%' "
				"OR ZOTCHESTVO LIKE '%%%s%%' "
				"OR ZTEL LIKE '%%%s%%' "
				"OR ZCOMMENT LIKE '%%%s%%' "
				"OR ZEMAIL LIKE '%%%s%%' "
				, search
				, search
				, search
				, search
				, search
				, search
				);
	}
	prozubi_passport_foreach(p, predicate, *list, get_patients);
	if (predicate)
		free(predicate);
}

#endif /* ifndef PATIENTS_LIST_H */
