/**
 * File              : switcher.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 21.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef SWITCHER_H
#define SWITCHER_H
#include <newt.h>
#include "main.h"

static struct{
	char *name;
	SWITCH  number;
}
switchwr_items[] = 
	{
		"Список пациентов    ", SW_PATIENTS,
		"Список сотрудников  ", SW_DOCTORS,
		"Список услуг (прайс)", SW_PRICES
	};

static void
switcher_new(
		newtComponent form,
		newtComponent *switcher,
		int cols, int rows)
{
	*switcher =
			newtListbox(0, 0, rows-1, NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
	newtListboxSetWidth(*switcher, 30);

	int i;
	for (i = 0; i < 3; ++i) {
		newtListboxAppendEntry(
				*switcher, 
				switchwr_items[i].name,
				&switchwr_items[i].number);			
	}
	newtFormAddComponent(form, *switcher);
}

#endif /* ifndef SWITCHER_H */
