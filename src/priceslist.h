/**
 * File              : priceslist.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PRICES_LIST_H
#define PRICES_LIST_H
#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "helpers.h"

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/prices.h"

static int 
get_prices(void *data, struct price_t *price){
	newtComponent list = (newtComponent)data;
	int count = newtListboxItemCount(list);
	char title[128];
	PRICE(title, count, price);
	newtListboxAppendEntry(list, title, price);
	return 0;
}

static void
prices_list_new(
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
				"WHERE ZTITLE LIKE '%%%s%%' "
				"OR ZKOD LIKE '%%%s%%' "
				, search
				, search
				);
	}
	prozubi_price_foreach(p, predicate, *list, get_prices);
	if (predicate)
		free(predicate);
}


#endif /* ifndef PRICES_LIST_H */
