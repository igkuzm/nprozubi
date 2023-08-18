/**
 * File              : zubformula.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 22.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef ZUBFORMULA_H
#define ZUBFORMULA_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/cases.h"
#include <newt.h>

#define TEETH\
	TOOTH(Z18, 1, 1)\
	TOOTH(Z17, 1, 2)\
	TOOTH(Z16, 1, 3)\
	TOOTH(Z15, 1, 4)\
	TOOTH(Z14, 1, 5)\
	TOOTH(Z13, 1, 6)\
	TOOTH(Z12, 1, 7)\
	TOOTH(Z11, 1, 8)\
	TOOTH(Z21, 1, 9)\
	TOOTH(Z22, 1, 10)\
	TOOTH(Z23, 1, 11)\
	TOOTH(Z24, 1, 12)\
	TOOTH(Z25, 1, 13)\
	TOOTH(Z26, 1, 14)\
	TOOTH(Z27, 1, 15)\
	TOOTH(Z28, 1, 16)\
	TOOTH(Z48, 3, 1)\
	TOOTH(Z47, 3, 2)\
	TOOTH(Z46, 3, 3)\
	TOOTH(Z45, 3, 4)\
	TOOTH(Z44, 3, 5)\
	TOOTH(Z43, 3, 6)\
	TOOTH(Z42, 3, 7)\
	TOOTH(Z41, 3, 8)\
	TOOTH(Z31, 3, 9)\
	TOOTH(Z32, 3, 10)\
	TOOTH(Z33, 3, 11)\
	TOOTH(Z34, 3, 12)\
	TOOTH(Z35, 3, 13)\
	TOOTH(Z36, 3, 14)\
	TOOTH(Z37, 3, 15)\
	TOOTH(Z38, 3, 16)\
	
static char * znames[] = 
{
	"18", "17", "16", "15", "14", "13", "12", "11",
	"21", "22", "23", "24", "25", "26", "27", "28",
	"48", "47", "46", "45", "44", "43", "42", "41",
	"31", "32", "33", "34", "35", "36", "37", "38",
};
static char * zvalues[] = 
{
	"P", "Pt", "0", "R", "К", "П", "C", "И", "", NULL 
};

static int zformula_filter(
		newtComponent entry, void * data, int ch,
			       int cursor)
{
	switch (ch) {
		case 'p':
			newtEntrySet(entry, zvalues[0], 0);
			return 0;
		case 't':
			newtEntrySet(entry, zvalues[1], 0);
			return 0;
		case '0':
			newtEntrySet(entry, zvalues[2], 0);
			return 0;
		case 'r':
			newtEntrySet(entry, zvalues[3], 0);
			return 0;
		case 'k':
			newtEntrySet(entry, zvalues[4], 0);
			return 0;
		case 'l':
			newtEntrySet(entry, zvalues[5], 0);
			return 0;
		case 'c':
			newtEntrySet(entry, zvalues[6], 0);
			return 0;
		case 'i':
			newtEntrySet(entry, zvalues[7], 0);
			return 0;
		case 'n':
			newtEntrySet(entry, zvalues[8], 0);
			return 0;
	}

	return 0;
}

static void
zformula_new(
		prozubi_t *p, 
		struct case_t *c) 
{
	bool ret = false;
	int i=0, key, y, cols=52, rows = 5;
	newtComponent form, ans=NULL;
	struct newtExitStruct toexit;
	
	newtCenteredWindow(cols, rows, "Зубная формула");
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, NEWT_KEY_UP);
	newtFormAddHotKey(form, NEWT_KEY_DOWN);
	newtFormAddHotKey(form, NEWT_KEY_TAB);
	newtFormAddHotKey(form, ' ');
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');

	#define TOOTH(n, row, col)\
	y = (row == 1)?0:4;\
	newtFormAddComponent(form, \
			newtLabel(col*3, y, znames[i++]));\
	char *v_##n = (char *)prozubi_case_get_##n(c);\
	if (!v_##n) v_##n=(char *)"";\
	newtComponent n = \
			newtEntry(col*3, row, v_##n, 2, NULL,\
			NEWT_FLAG_BORDER);\
	newtEntrySetFilter(n, zformula_filter, NULL);\
	newtFormAddComponent(form, n);\

	TEETH
	#undef TOOTH

	newtPushHelpLine(
			"p: \"P\", " "t: \"Pt\", " "0: \"0\", " "r: \"R\", " 
			"k: \"К\", " "l: \"П\", "  "c: \"C\", " "i: \"И\", " "n - norm");

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
		if (key == NEWT_KEY_RETURN){
			// exit with save
			#define TOOTH(n, row, col)\
			char *v_##n = newtEntryGetValue(n);\
			prozubi_case_set_##n(p, c, v_##n);
			TEETH
			#undef TOOTH
			break;
		}
		if (key == NEWT_KEY_UP){
			newtComponent cur =
					newtFormGetCurrent(form);
			int ncol, nrow;
			i=0;
			#define TOOTH(n, row, col)\
			if (n == cur){\
				ncol = col;\
				nrow = row;\
			}\
			i++;
			TEETH
			#undef TOOTH
			
			if (nrow == 3){
				i=0;
				#define TOOTH(n, row, col)\
				if (row == 1 && col == ncol){\
					newtFormSetCurrent(form, n);\
				}\
				i++;
				TEETH
				#undef TOOTH
			}
			continue;
		}
		if (key == NEWT_KEY_DOWN){
			newtComponent cur =
					newtFormGetCurrent(form);
			int ncol, nrow;
			i=0;
			#define TOOTH(n, row, col)\
			if (n == cur){\
				ncol = col;\
				nrow = row;\
			}\
			i++;
			TEETH
			#undef TOOTH
			
			if (nrow == 1){
				i=0;
				#define TOOTH(n, row, col)\
				if (row == 3 && col == ncol){\
					newtFormSetCurrent(form, n);\
				}\
				i++;
				TEETH
				#undef TOOTH
			}
			continue;
		}
		if (key == ' ' || key == NEWT_KEY_TAB){
			newtComponent cur =
					newtFormGetCurrent(form);
			char *v = newtEntryGetValue(cur);
			if (!v) v = "";
			i=0;
			while	(zvalues[i]){
				if (strcmp(v, zvalues[i++]) == 0)
					break;
			}
			v = zvalues[i];
			if (!v) v = zvalues[0];
			newtEntrySet(cur, v, 0);
			
			continue;
		}

	} while (key != NEWT_KEY_F10);
	
	newtFormDestroy(form);
	newtPopHelpLine();
	newtPopWindow();
}
#endif /* ifndef ZUBFORMULA_H */
