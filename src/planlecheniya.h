/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 24.07.2023
 * Last Modified Date: 02.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLAN_LECHENIYA_H
#define PLAN_LECHENIYA_H
#include <stdio.h>
#include <stdlib.h>
#include <newt.h>
#include "../prozubilib/planlecheniya.h"
#include "../prozubilib/cases.h"
#include "../prozubilib/prozubilib.h"
#include "../prozubilib/documents.h"
#include "asktoremove.h"
#include "askindex.h"
#include "pricesget.h"
#include "planlecheniyaedit.h"
#include "dialog.h"
#include "helpers.h"
#include "ini.h"


struct plt {
	prozubi_t *p;
	struct case_t *c;
	newtComponent tree;
	struct planlecheniya_t *array[200];
	int count;
	int selected;
};

static void 
plan_lecheniya_on_destroy(
		newtComponent tree, void *data)
{
	struct plt *l = (struct plt *)data;
	int i;
	for (i = 0; i < l->count; ++i) {
		//prozubi_planlecheniya_free(l->array[i]);
	}
}

void plan_lecheniya_add_price(void *userdata, struct price_t *price)
{
	struct plt *l = (struct plt *)userdata;
	struct planlecheniya_t *t = 
			(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(l->tree);
	if (!t)
		return;

	if (t->stageIndex == -1)
		return;
	
	prozubi_planlecheniya_add_item(l->p, l->c->planlecheniya, t->stageIndex, 
			price->title, price->kod, atoi(price->price), 1);	
}

static void * 
plan_lecheniya_update_callback(
			void * userdata, 
			void * parent, 
			struct planlecheniya_t *t)
{
	struct plt *l = (struct plt *)userdata;
	l->array[l->count++] = t;

	switch (t->type) {
		case PLANLECHENIYA_TYPE_STAGE:
			{
				newtCheckboxTreeAddItem(l->tree, t->title, t, NEWT_FLAG_SELECTED, t->stageIndex, NEWT_ARG_LAST);
				break;
			}
		case PLANLECHENIYA_TYPE_ITEM:
			{
				char str[512];
				sprintf(str, "%d. x%s - %s",
					t->itemIndex+1, t->count, t->title);
				newtCheckboxTreeAddItem(l->tree, str, t, 0, t->stageIndex, NEWT_ARG_APPEND, NEWT_ARG_LAST);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_PRICE:
			{
				char str[128];
				sprintf(str, "%s %s руб.", t->title, t->total);		
				newtCheckboxTreeAddItem(l->tree, str, t, 0, t->stageIndex, NEWT_ARG_APPEND, NEWT_ARG_LAST);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			{
				char str[128];
				sprintf(str, "%s %s", t->title, t->count);		
				newtCheckboxTreeAddItem(l->tree, str, t, 0, t->stageIndex, NEWT_ARG_APPEND, NEWT_ARG_LAST);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_PRICE:
			{
				char str[128];
				sprintf(str, "%s %s руб.", t->title, t->total);		
				newtCheckboxTreeAddItem(l->tree, str, t, 0, NEWT_ARG_LAST);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_DURATION:
			{
				char str[128];
				sprintf(str, "%s %s", t->title, t->count);		
				newtCheckboxTreeAddItem(l->tree, str, t, 0, NEWT_ARG_LAST);
				break;
			}
	}
	
	return t;
}

static int
plan_lecheniya_update(
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c,
		int selected,
		int cols, int rows)
{
	int i, key, ret=-1;
	newtComponent form, tree, ans=NULL;
	struct newtExitStruct toexit;
	
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F1);
	newtFormAddHotKey(form, NEWT_KEY_F4);
	newtFormAddHotKey(form, NEWT_KEY_F5);
	newtFormAddHotKey(form, NEWT_KEY_F6);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F8);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, '+');
	newtFormAddHotKey(form, '-');
	newtFormAddHotKey(form, '*');
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');
	newtFormAddHotKey(form, 'e');
	newtFormAddHotKey(form, 'd');
	newtFormAddHotKey(form, 'c');
	newtFormAddHotKey(form, 'm');
	newtFormAddHotKey(form, 'p');

	// create new
	tree =
			newtCheckboxTree(0, 0, rows, 
			NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|
			NEWT_FLAG_BORDER|
			NEWT_CHECKBOXTREE_UNSELECTABLE);
	
	newtFormAddComponent(form, tree);
	newtCheckboxTreeSetWidth(tree, cols);

	struct plt l = {p, c, tree};
	l.count = 0;
	newtComponentAddDestroyCallback(tree, plan_lecheniya_on_destroy, &l);

	// fill list
	prozubi_planlecheniya_foreach(p, c->planlecheniya, &l, plan_lecheniya_update_callback);

	// set selected
	newtCheckboxTreeSetCurrent(tree, l.array[selected]);

	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
	
		if (key == NEWT_KEY_F7 || key == 'a'){
			// add stage/item
			if (!c->planlecheniya){
				prozubi_planlecheniya_new(c);
				prozubi_planlecheniya_add_stage(p, c->planlecheniya);
				break;
			}
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_STAGE ||
					t->type == PLANLECHENIYA_TYPE_STAGE_PRICE ||
					t->type == PLANLECHENIYA_TYPE_STAGE_DURATION ||
					t->type == PLANLECHENIYA_TYPE_ITEM)
			{
				prices_get_new(p, NULL, cols, rows, &l,  plan_lecheniya_add_price);			
			} else {
				prozubi_planlecheniya_add_stage(p, c->planlecheniya);
			}
			break;
		}
		if (key == NEWT_KEY_F8 || key == 'd'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_STAGE)
				if (ask_to_remove(t->title))	
					prozubi_planlecheniya_remove_stage(p, c->planlecheniya, t->stageIndex);
			if (t->type == PLANLECHENIYA_TYPE_ITEM)
				if (ask_to_remove(t->title))	
					prozubi_planlecheniya_remove_item(p, c->planlecheniya, t->stageIndex, t->itemIndex);

			break;
		}
		if (key == NEWT_KEY_F5 || key == 'c'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_ITEM){
				int index = ask_index("Введите номер этапа");
				index--;
				prozubi_planlecheniya_add_item(p, c->planlecheniya, index, t->title, t->kod, atoi(t->price), atoi(t->count));
				break;
			}
		}
		if (key == NEWT_KEY_F6 || key == 'm'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_ITEM){
				int index = ask_index("Введите номер этапа");
				index--;
				if (index != t->stageIndex){
					prozubi_planlecheniya_add_item(p, c->planlecheniya, index, t->title, t->kod, atoi(t->price), atoi(t->count));
					prozubi_planlecheniya_remove_item(p, c->planlecheniya, t->stageIndex, t->itemIndex);
				}
				break;
			}
		}
		if (key == '+'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_ITEM){
				int count = atoi(t->count);
				count++;
				prozubi_planlecheniya_set_item_count(p, c->planlecheniya, t->stageIndex, t->itemIndex, count);
				break;
			}
			if (t->type == PLANLECHENIYA_TYPE_STAGE_DURATION){
				int count = atoi(t->count);
				count++;
				prozubi_planlecheniya_set_stage_duration(p, c->planlecheniya, t->stageIndex, count);
				break;
			}
		}
		if (key == '-'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_ITEM){
				int count = atoi(t->count);
				count--;
				if (count < 1) count = 1;
				prozubi_planlecheniya_set_item_count(p, c->planlecheniya, t->stageIndex, t->itemIndex, count);
				break;
			}
			if (t->type == PLANLECHENIYA_TYPE_STAGE_DURATION){
				int count = atoi(t->count);
				count--;
				if (count < 1) count = 1;
				prozubi_planlecheniya_set_stage_duration(p, c->planlecheniya, t->stageIndex, count);
				break;
			}
		}
		if (key == NEWT_KEY_F4 || key == 'e'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			if (t->type == PLANLECHENIYA_TYPE_ITEM){
				planlecheniya_edit_new(p, c, t, cols, rows);
				break;
			}
		}
		if (key == NEWT_KEY_F1 || key == 'p'){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (!t)
				break;
			const char *rtf = documents_get_plan_lecheniya(
					"Templates/planLecheniyaTemplate.rtf", 
					p, patient, c);

			int sel = -1;
			while (sel != 0 && sel != 1){
				const char *buttons[] = {"отмена", "печать", "просмотр", "email"};
				sel = dialog(
						"Печать плана", 
						"Распечатать план лечения или открыть для просмотра?", 
						buttons, 4);
				if (sel == 1){
					newtSuspend();
					char *command = ini_get(
							"nprozubi.ini", NULL, "PRINTCMD");
					if (!command)
						command = strdup("soffice out.rtf");
					system(command);
					free(command);
					//system("unoconv -f pdf out.rtf");
					//system("lp out.pdf");
					newtResume();
				} else if (sel == 2){
					newtSuspend();
					char *command = ini_get(
							"nprozubi.ini", NULL, "OPENCMD");
					if (!command)
						command = strdup("soffice out.rtf");
					system(command);
					free(command);
					//system("unoconv -f pdf out.rtf");
					//system("fbpdf out.pdf");
					newtResume();
				} else if (sel == 3){
					newtSuspend();
					char *command = ini_get(
							"nprozubi.ini", NULL, "MAILCMD");
					if (!command)
						command = strdup("mutt -a out.rtf");
					system(command);
					free(command);
					//system("unoconv -f pdf out.rtf");
					//system("mutt -a out.pdf");
					newtResume();
				}
			}
			break;
		}
		if (key == NEWT_KEY_ESCAPE || key == NEWT_KEY_F10 || key == 'q'){
			// exit without save
			ret=-2;
			break;
		}

	} while (1);
	
	if (ret == -1){
		// set selected
		selected = 0;
		if (l.count){
			struct planlecheniya_t *t = 
					(struct planlecheniya_t *)newtCheckboxTreeGetCurrent(tree);
			if (t){
				for (i = 0; i < l.count; ++i) {
					if (t == l.array[i])
						selected = i;	
				}
			}
		}
			
		ret = selected;
	}
	newtFormDestroy(form);
	return ret;
}

static void
plan_lechniya_new(
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c,
		int cols, int rows)
{
	cols -= 10;
	rows -= 6;

	newtCenteredWindow(cols, rows, "План лечения");
	
	// main loop
	int selected = 0;
	while ((selected = plan_lecheniya_update(p, patient, c, selected, cols, rows)) > -1){
	}
	
	prozubi_case_set_CASEPLANLECHENIYA(p, c, c->planlecheniya, 0);
	newtPopWindow();
}

#endif /* ifndef PLAN_LECHENIYA_H */
