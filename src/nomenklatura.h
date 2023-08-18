/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 30.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef NOMENKLATURA_LIST_H
#define NOMENKLATURA_LIST_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/nomenklatura.h"
#include "askstring.h"
#include "helpers.h"
#include <newt.h>

static void 
nomenklatura_on_destroy(
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

static void *
get_nomenklatura(void *data, void *parent, nomenklatura_t *n){
	newtComponent tree = (newtComponent)data;
	
	char title[256];
	NOMENKLATURA(title, n);
	if (parent == NULL){
		newtCheckboxTreeAddItem(tree, title, n, 0, 0, NEWT_ARG_LAST);
	} else {
		newtCheckboxTreeAddItem(tree, title, n, 0, 0, NEWT_ARG_APPEND, NEWT_ARG_LAST);
	}
	return n;
}

static int
nomenklatura_refresh(
		prozubi_t *p, 
		const char **search,
		int cols, int rows,
		void *userdata,
		void callback(
			void *userdata,
			const char * title,
			const char *kod)
		)
{
	const void **result;
	int i, key, num, ret=-1;

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
	newtFormAddHotKey(form, 'r');
	newtFormAddHotKey(form, 's');
	newtFormAddHotKey(form, '/');
	
	// create new
	tree =
				newtCheckboxTree(0, 0, rows, 
	NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
		newtFormAddComponent(form, tree);
	newtCheckboxTreeSetWidth(tree, cols);
	newtComponentAddDestroyCallback(tree, nomenklatura_on_destroy, NULL);

	// fill list with nomenklatura
	char *predicate = NULL;
	if (*search){
		predicate = (char *)malloc(BUFSIZ);
		if (!predicate){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		sprintf(predicate, 
				"WHERE kod LIKE '%%%s%%' "
				"OR name LIKE '%%%s%%' "
				"OR headName LIKE '%%%s%%' "
				, *search
				, *search
				, *search
				);
	}
	prozubi_nomenklatura_foreach(p, predicate, tree, get_nomenklatura);
	if (predicate)
		free(predicate);

	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
		
		if (key == NEWT_KEY_F3 || key == 's' || key == '/'){
			// make search
			*search = ask_string("Поиск: наименование, код");
			break;
		}
		if (key == NEWT_KEY_ESCAPE || key == 'r'){
			// refresh
			*search = NULL;
			break;
		}
		
		if (key == 'q' || key == NEWT_KEY_F10){
			// exit without save
			ret = 0;
			break;
		}
		
		if (key == NEWT_KEY_F7 || key == 'a'){
			// add items
			result = newtCheckboxTreeGetSelection(tree, &num);
			for (i = 0; i < num; ++i) {
				nomenklatura_t *n = (nomenklatura_t*)result[i];	
				if (callback)
					callback(userdata, n->name, n->kod);
			}
			ret = 1;
			break;
		}
	} while (key != NEWT_KEY_F10);

	newtFormDestroy(form);
	return ret;
}
static void
nomenklatura_new(
		prozubi_t *p, 
		const char *search,
		int cols, int rows,
		void *userdata,
		void callback(
			void *userdata,
			const char * title,
			const char *kod)
		)
{
	cols -= 10;
	rows -= 6;

	newtCenteredWindow(cols, rows, "Номенклатура");
	while(nomenklatura_refresh(p, &search, cols, rows, userdata, callback) == -1){
		//do cycle
	}
		
	newtPopWindow();
}
#endif /* ifndef NOMENKLATURA_H */
