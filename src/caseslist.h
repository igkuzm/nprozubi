/**
 * File              : caseslist.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 30.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef CASES_LIST_H
#define CASES_LIST_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/cases.h"
#include "helpers.h"
#include "asktoremove.h"
#include "zubformula.h"
#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include "calendar.h"
#include "combobox.h"
#include "planlecheniya.h"
#include "imageslist.h"


struct cases_list_t {
	prozubi_t *p;
	newtComponent list;
	struct passport_t *patient; 
};

static void 
cases_on_destroy(
		newtComponent list, void *data)
{
	struct cases_list_t *t = 
			(struct cases_list_t *)data;
	int count = newtListboxItemCount(list);
	int i;
	for (i = 0; i < count; ++i) {
		struct case_list_node *n;
		newtListboxGetEntry(list, i, NULL, (void **)&n);	
		prozubi_case_list_node_free(t->p, n);
	}
}

static void *
get_casesi_list(
			void * data,
			void * parent,
			struct case_list_node *n)
{
	struct cases_list_t *t = 
			(struct cases_list_t *)data;
	
	char str[512];
	if (parent == NULL) {
		sprintf(str, "%s", n->title ? n->title : ""); 
	} else {
		sprintf(str, "--- %s", n->title ? n->title : "--- "); 
	}

	newtListboxAppendEntry(t->list, str, n);
	
	return n;
}

static int
get_cases(void *data, struct case_t *c){
	struct cases_list_t *t = 
			(struct cases_list_t *)data;
	
	prozubi_cases_list_foreach(
		t->p,
		c, 
		data, 
		get_casesi_list);

	return 0;
}

static void
cases_list_update(
		newtComponent list,	
		struct cases_list_t *t 
		)
{
	// free memory
	cases_on_destroy(list, t);

	// clear list
	newtListboxClear(list);

	// fill list with cases
	prozubi_cases_foreach(t->p, t->patient->id, NULL,  t, get_cases);
}

static void
cases_list_new(
		prozubi_t *p, 
		struct passport_t *patient, 
		int cols, int rows)
{
	int i, key;
	cols -= 10;
	rows -= 6;

	newtComponent form, list, ans=NULL;
	struct newtExitStruct toexit;
	
	char title[128];
	sprintf(title, "%s %s %s - Список визитов", 
			patient->familiya,
			patient->imia,
			patient->otchestvo);
	newtCenteredWindow(cols, rows, title);
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F8);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');
	newtFormAddHotKey(form, 'd');
	newtFormAddHotKey(form, 'e');

	// create new
	list =
			newtListbox(0, 0, rows,
			NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
	newtFormAddComponent(form, list);
	newtListboxSetWidth(list, cols);
	
	struct cases_list_t t = {p, list, patient}; 
	newtComponentAddDestroyCallback(list, cases_on_destroy, &t);

	// update data
	cases_list_update(list, &t);
	
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
	
		if (key == NEWT_KEY_ESCAPE || key == 'q'){
			// exit without save
			break;
		}
		if (key == NEWT_KEY_F7 || key == 'a'){
			// add item
			prozubi_case_new_for_patient(p, patient->id);
			cases_on_destroy(list, NULL);
			newtListboxClear(list);
			prozubi_cases_foreach(p, patient->id, NULL, &t, get_cases);
			continue;
		}
		if (key == NEWT_KEY_F8 || key == 'd'){
			struct case_list_node *n = 
					(struct case_list_node *)newtListboxGetCurrent(list);
			if(n){
				char text[512];
				sprintf(text, "Вы хотите удалить запись из базы: %s", n->c->name);
				if (ask_to_remove(text)){
					// remove item
					prozubi_case_remove(p, n->c);
					newtListboxDeleteEntry(list, n);
				}
			}
			continue;
		}
		if (key == NEWT_KEY_RETURN || key == 'e'){
			struct case_list_node *n = 
					(struct case_list_node *)newtListboxGetCurrent(list);
			if (!n)
				continue;
			switch (n->type) {
				case CASES_LIST_TYPE_TEXT:
					{
						int cur = newtListboxGetByKey(list, n);
						newtSuspend();
						remove(".tmp.txt");

						FILE *fp = fopen(".tmp.txt", "w+");
						if (!fp) 
							break;
						
						char *v = (char *)prozubi_case_get(n->c, n->key);
						if (n->key == CASEDIAGNOZIS)
							if (!v || strlen(v) < 2)
								v = prozubi_diagnosis_get(p, n->c);
						if (v)
							fwrite(v, strlen(v), 1, fp);
						fclose(fp);
						
						system("$EDITOR .tmp.txt");
						
						fp = fopen(".tmp.txt", "r");
						if (!fp) 
							break;
						fseek(fp, 0, SEEK_END);
						size_t size = ftell(fp);
						fseek(fp, 0, SEEK_SET);
						char *buf = (char *)malloc(size);
						if (!buf) break;
						fread(buf, size, 1, fp);
						fclose(fp);
						
						prozubi_case_set_text(n->key, p, n->c, buf);

						free(buf);
						
						newtResume();

						cases_list_update(list, &t);
						newtListboxSetCurrent(list, cur);
						break;
					}
				case CASES_LIST_TYPE_ZFORMULA:
					{
						int cur = newtListboxGetByKey(list, n);
						zformula_new(p, n->c);
						cases_list_update(list, &t);
						newtListboxSetCurrent(list, cur);
						break;
					}
				case CASES_LIST_TYPE_DATE:
					{
						time_t *value = (time_t *)prozubi_case_get(n->c, n->key);
						time_t  nvalue = calendar_new(*value);
						if (nvalue != *value){
							int cur = newtListboxGetByKey(list, n);
							prozubi_case_set_date(n->key, p, n->c, nvalue);
							cases_list_update(list, &t);
							newtListboxSetCurrent(list, cur);
						}
						break;
					}
				case CASES_LIST_TYPE_COMBOBOX:
					{
						int cur = newtListboxGetByKey(list, n);
						int i = combobox_new(n);
						if (i == -1)
							break;
						prozubi_case_set_text(n->key, p, n->c, n->array[i]);
						cases_list_update(list, &t);
						newtListboxSetCurrent(list, cur);

						break;
					}
				case CASES_LIST_TYPE_PLANLECHENIYA:
					{
						plan_lechniya_new(p, patient, n->c, cols, rows);
						break;
					}
				case CASES_LIST_TYPE_XRAY:
					{
						images_list_new(p, n->c, NULL, cols, rows);
						break;
					}

				default:
					break;
			}
			continue;;
		}

	} while (key != NEWT_KEY_F10);

	newtFormDestroy(form);
	newtPopWindow();
}
#endif /* ifndef CASES_H */
