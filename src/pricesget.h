/**
 * File              : pricesget.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 31.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PRICES_GET_H
#define PRICES_GET_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/nomenklatura.h"
#include "helpers.h"
#include "askstring.h"
#include <newt.h>

static void 
prices_get_on_destroy(
		newtComponent tree, void *data)
{
	//int count = newtListboxItemCount(list);
	//int i;
	//for (i = 0; i < count; ++i) {
		//struct price_t *price;
		//newtListboxGetEntry(list, i, NULL, (void **)&price);	
		//prozubi_prices_free(price);
	//}
}

static int 
get_get_prices(void *data, struct price_t *price){
	newtComponent tree = (newtComponent)data;
	char title[128];
	PRICE1(title, price);
	newtCheckboxTreeAddItem(tree, title, price, 0, 0, NEWT_ARG_LAST);
	return 0;
}

static int 
prices_get_refresh(
		prozubi_t *p, 
		char **predicate,
		int cols, int rows,
		void *userdata,
		void callback(
			void *userdata,
			struct price_t *price)
		)
{
	const void **result;
	int i, key, num, ret = -1;

	newtComponent form, tree, ans=NULL;
	struct newtExitStruct toexit;
	
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F3);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');
	newtFormAddHotKey(form, 's');
	newtFormAddHotKey(form, '/');
	newtFormAddHotKey(form, 'r');
	
	// create new
	tree =
				newtCheckboxTree(0, 0, rows, 
	NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
		newtFormAddComponent(form, tree);
	newtCheckboxTreeSetWidth(tree, cols);
	newtComponentAddDestroyCallback(tree, prices_get_on_destroy, NULL);

	// fill list with nomenklatura
	prozubi_price_foreach(p, *predicate, tree, get_get_prices);

	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
	
		if (key == NEWT_KEY_ESCAPE || key == 'r'){
			// refresh
			if (*predicate)
				free(*predicate);
			*predicate = NULL;
			break;
		}
		if (key == NEWT_KEY_F3 || key == 's' || key == '/'){
			// search
			char *search = 
					ask_string("Поиск: наименование, код");
			if (search){
				*predicate = (char *)malloc(BUFSIZ);
				if (!*predicate){
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				sprintf(*predicate, 
						"WHERE ZTITLE LIKE '%%%s%%' "
						"OR ZKOD LIKE '%%%s%%' "
						, search
						, search
						);
				free(search);
			}
			break;
		}
		if (key == NEWT_KEY_F10 || key == 'q'){
			// exit without save
			ret = 0;
			break;
		}
		if (key == NEWT_KEY_F7 || key == 'a'){
			// add items
			result = newtCheckboxTreeGetSelection(tree, &num);
			for (i = 0; i < num; ++i) {
				struct price_t *price = (struct price_t *)result[i];	
				if (callback)
					callback(userdata, price);
			}
			ret = 1;
			break;
		}
	} while (key != NEWT_KEY_F10);

	newtFormDestroy(form);
	return ret;
}


static void
prices_get_new(
		prozubi_t *p, 
		const char *search,
		int cols, int rows,
		void *userdata,
		void callback(
			void *userdata,
			struct price_t *price)
		)
{
	cols -= 10;
	rows -= 6;

	newtCenteredWindow(cols, rows, "Список услуг (прайс)");
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

	//do  cycle
	while (prices_get_refresh(p, &predicate, cols, rows, userdata, callback) == -1){
		//
	}
	if (predicate)
		free(predicate);
	newtPopWindow();
}
#endif /* ifndef PRICES_GET_H */
