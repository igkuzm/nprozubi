/**
 * File              : planlecheniyaedit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PLANLECHENIYA_EDIT_H
#define PLANLECHENIYA_EDIT_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/planlecheniya.h"
#include "helpers.h"
#include <newt.h>

static int pl_number_filter(
		newtComponent entry, void * data, int c,
			       int p)
{
	if (c == NEWT_KEY_LEFT    ||
			c == NEWT_KEY_RIGHT   ||
			c == NEWT_KEY_DELETE  ||
			c == NEWT_KEY_BKSPC   ||
			c == NEWT_KEY_HOME    ||
			c == NEWT_KEY_ENTER   ||
			c == NEWT_KEY_UP      ||
			c == NEWT_KEY_DOWN    ||
			c == NEWT_KEY_END)
		return c;

	if (c >= '0' && c<= '9')
		return c;

	return 0;
}

#define PL_EDIT_TYPES\
	PL_EDIT_TYPE_ENTRY(title, "Наименование:")\
	PL_EDIT_TYPE_ENTRY(kod,   "Код:"      )\
	PL_EDIT_TYPE_NUMBE(price, "Цена:"     )\
	PL_EDIT_TYPE_NUMBE(count, "Количество:")\

struct pl_edit_t {
	struct case_t *c;
	struct planlecheniya_t *t;
#define PL_EDIT_TYPE_ENTRY(name, title) newtComponent m_##name;
#define PL_EDIT_TYPE_NUMBE(name, title) newtComponent m_##name;
	PL_EDIT_TYPES
#undef PL_EDIT_TYPE_ENTRY 
#undef PL_EDIT_TYPE_NUMBE 
};

static bool
planlecheniya_edit_new(
	prozubi_t *p,
	struct case_t *c,
	struct planlecheniya_t *t,
	int cols, int rows
		)
{
	bool ret = false;
	cols -= 10; rows -= 6;
	int i, key, y=0;
	newtComponent form, ans=NULL;
	struct newtExitStruct toexit;
	
	struct pl_edit_t l = {c, t};
	
	newtCenteredWindow(cols, rows-10, t->title);
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');

#define PL_EDIT_TYPE_ENTRY(name, title)\
	const char * v_##name = NULL;\
	{\
		newtComponent lab = newtLabel(1, y, title);\
		newtFormAddComponent(form, lab);\
		y++;\
		l.m_##name = \
				newtEntry(1, y, t->name, cols-1,\
				&v_##name, 0);\
		newtFormAddComponent(form, l.m_##name);\
		y+=2;\
	}

#define PL_EDIT_TYPE_NUMBE(name, title)\
	const char * v_##name = NULL;\
	{\
		newtComponent lab = newtLabel(1, y, title);\
		newtFormAddComponent(form, lab);\
		y++;\
		if (strcmp(#name, "count") == 0)\
		l.m_##name = \
				newtEntry(1, y, t->name, cols-1,\
				&v_##name, NEWT_ENTRY_RETURNEXIT);\
		else\
		l.m_##name = \
				newtEntry(1, y, t->name, cols-1,\
				&v_##name, 0);\
		newtEntrySetFilter(l.m_##name, pl_number_filter, NULL);\
		newtFormAddComponent(form, l.m_##name);\
		y+=2;\
	}

	PL_EDIT_TYPES
#undef PL_EDIT_TYPE_ENTRY 
#undef PL_EDIT_TYPE_NUMBE 

// main loop
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
		
		if (ans == l.m_count || key == NEWT_KEY_F7){
			// save data
#define PL_EDIT_TYPE_ENTRY(name, title)\
			if (v_##name)\
				prozubi_planlecheniya_set_item_##name(p, c->planlecheniya, t->stageIndex, t->itemIndex, v_##name);

#define PL_EDIT_TYPE_NUMBE(name, title)\
			if (v_##name)\
				prozubi_planlecheniya_set_item_##name(p, c->planlecheniya, t->stageIndex, t->itemIndex, atoi(v_##name));
	PL_EDIT_TYPES
#undef PL_EDIT_TYPE_ENTRY 
#undef PL_EDIT_TYPE_NUMBE 
			ret = true;
			break;
		}
		
		if (key == NEWT_KEY_ESCAPE){
			// exit without save
			break;
		}

	} while (key != NEWT_KEY_F10);
	
	newtFormDestroy(form);
	newtPopWindow();

	return ret;
}

#endif /* ifndef PLANLECHENIYA_EDIT_H */
