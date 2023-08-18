/**
 * File              : priceedit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 24.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PRICE_EDIT
#define PRICE_EDIT

#include "../prozubilib/prozubilib.h"
#include "helpers.h"
#include <newt.h>

static int price_number_filter(
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


#define PRICE_EDIT_TYPES\
	PRICE_EDIT_TYPE_ENTRY(title, "Наименование:",  PRICETITLE)\
	PRICE_EDIT_TYPE_ENTRY(kod,   "Код:",           PRICEKOD)\
	PRICE_EDIT_TYPE_PRICE(price, "Цена:",          PRICEPRICE)\
	PRICE_EDIT_TYPE_ENTRY(about, "Дополнительно:", PRICEABOUT)\

struct price_edit_t {
	prozubi_t *p;
	struct price_t *price;
#define PRICE_EDIT_TYPE_ENTRY(name, title, key) newtComponent m_##name;
#define PRICE_EDIT_TYPE_PRICE(name, title, key) newtComponent m_##name;
	PRICE_EDIT_TYPES
#undef PRICE_EDIT_TYPE_ENTRY 
#undef PRICE_EDIT_TYPE_PRICE 
};

static bool
price_edit_new(
		prozubi_t *p,
		struct price_t *price,
		newtComponent list
		)
{
	bool ret = false;
	int i, key, y=0, cols=30, rows = 22;
	newtComponent form, ans=NULL;
	struct newtExitStruct toexit;
	
	struct price_edit_t t = {p, price};
	
	newtCenteredWindow(cols, rows-10, price->title);
	// create form
	form = newtForm(NULL, NULL, 0);
	/*newtFormSetBackground(form, 1);*/
	/*newtFormSetHeight(form, rows);*/
	/*newtFormSetWidth (form, cols);*/
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);

#define PRICE_EDIT_TYPE_ENTRY(name, title, key)\
	const char * v_##name = NULL;\
	{\
		newtComponent l = newtLabel(1, y, title);\
		newtFormAddComponent(form, l);\
		y++;\
		if (strcmp(#name, "about") == 0)\
		t.m_##name = \
				newtEntry(1, y, price->name, cols-1,\
				&v_##name, NEWT_ENTRY_RETURNEXIT);\
		else\
		t.m_##name = \
				newtEntry(1, y, price->name, cols-1,\
				&v_##name, 0);\
		newtFormAddComponent(form, t.m_##name);\
		y+=2;\
	}

#define PRICE_EDIT_TYPE_PRICE(name, title, key)\
	const char * v_##name = NULL;\
	{\
		newtComponent l = newtLabel(1, y, title);\
		newtFormAddComponent(form, l);\
		y++;\
		t.m_##name = \
				newtEntry(1, y, price->name, cols-1,\
				&v_##name, 0);\
		newtEntrySetFilter(t.m_##name, price_number_filter, NULL);\
		newtFormAddComponent(form, t.m_##name);\
		y+=2;\
	}

	PRICE_EDIT_TYPES
#undef PRICE_EDIT_TYPE_ENTRY 
#undef PRICE_EDIT_TYPE_PRICE 

// main loop
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
		
		if (ans == t.m_about || key == NEWT_KEY_F7){
			// save data
#define PRICE_EDIT_TYPE_ENTRY(name, title, key)\
			if (v_##name)\
				prozubi_prices_set_##key(t.p, price, v_##name, true);	
#define PRICE_EDIT_TYPE_PRICE(name, title, key)\
			if (v_##name)\
				prozubi_prices_set_##key(t.p, price, v_##name, true);	
	PRICE_EDIT_TYPES
#undef PRICE_EDIT_TYPE_ENTRY 
#undef PRICE_EDIT_TYPE_PRICE 
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

#endif /* ifndef PRICE_EDIT */
