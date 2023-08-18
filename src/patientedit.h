/**
 * File              : patientedit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 28.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef PATIENT_EDIT
#define PATIENT_EDIT

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/passport.h"
#include "helpers.h"

#include <time.h>
#include <newt.h>

char * strptime(const char* s, const char* f, struct tm* tm);

#define PATIENT_EDIT_TYPES\
	PATIENT_EDIT_TYPE_ENTRY(familiya,    "Фамилия:",           PASSPORTFAMILIYA)\
	PATIENT_EDIT_TYPE_ENTRY(imia,        "Имя:",               PASSPOTIMIA)\
	PATIENT_EDIT_TYPE_ENTRY(otchestvo,   "Отчество:",          PASSPORTOTCHESTVO)\
	PATIENT_EDIT_TYPE_EDATE(dateofbirth, "Дата рождения:",     PASSPORTDATEOFBIRTH)\
	PATIENT_EDIT_TYPE_ETELE(tel,         "Телефон:",           PASSPORTTEL)\
	PATIENT_EDIT_TYPE_ENTRY(email,       "Email:",             PASSPORTEMAIL)\
	PATIENT_EDIT_TYPE_ENTRY(address,     "Адрес проживания:",  PASSPORTADDRESS)\
	PATIENT_EDIT_TYPE_ENTRY(document,    "Паспортные данные:", PASSPORTDOCUMENT)\
	PATIENT_EDIT_TYPE_ENTRY(comment,     "Коментарий:",        PASSPORTCOMMENT)

struct patinet_edit_t {
	prozubi_t *p;
	struct passport_t *patient;
#define PATIENT_EDIT_TYPE_ENTRY(name, title, key) newtComponent m_##name;
#define PATIENT_EDIT_TYPE_EDATE(name, title, key) newtComponent m_##name;
#define PATIENT_EDIT_TYPE_ETELE(name, title, key) newtComponent m_##name;
#define PATIENT_EDIT_TYPE_TEXTV(name, title, key) newtComponent m_##name;
	PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 
};

/* format input like date dd.mm.yyyy */
static int patient_date_filter(
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

	char *v = newtEntryGetValue(entry);
	if (v)
		if (strlen(v)>9)
			return 0;

	if (p == 2 || p == 5)
		return '.';
	
	if (p > 9)
		return 0;

	if (c >= '0' && c<= '9')
		return c;

	return 0;	
}

/* format input like telephone number  +7999123456 */
static int patient_tel_filter(
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

	char *v = newtEntryGetValue(entry);
	if (v)
		if (strlen(v)>11)
			return 0;
	
	if (p > 11)
		return 0;

	char text[BUFSIZ] = {0};
	if (p == 0){
		if (c == '+')
			text[0] = '+';
		else if (c == '8'){
			text[0] = '+';
			text[1] = '7';
		}
		else {
			text[0] = '+';
			if (c >= '0' && c<= '9')
				text[1] = c;
		}		
		if (strlen(text) > 0){
			newtEntrySet(entry, text, 1);
		}
	} else {
		if (c >= '0' && c<= '9')
			return c;
	} 
	return 0;	
}

static bool
patient_edit_new(
		prozubi_t *p,
		struct passport_t *patient,
		newtComponent list
		)
{
	bool ret = false;
	int i, key, y=0, cols=30, rows = 36;
	newtComponent form, ans=NULL;
	struct newtExitStruct toexit;
	
	struct patinet_edit_t t = {p, patient};
	
	char fio[64];
	FIO(fio, patient);
	
	newtCenteredWindow(cols, rows-10, fio);
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);

#define PATIENT_EDIT_TYPE_ENTRY(name, title, key)\
	const char * v_##name = NULL;\
	{\
		newtComponent l = newtLabel(1, y, title);\
		newtFormAddComponent(form, l);\
		y++;\
		if (strcmp(#name, "comment") == 0)\
		t.m_##name = \
				newtEntry(1, y, patient->name, cols-1,\
				&v_##name, NEWT_ENTRY_RETURNEXIT);\
		else\
		t.m_##name = \
				newtEntry(1, y, patient->name, cols-1,\
				&v_##name, 0);\
		newtFormAddComponent(form, t.m_##name);\
		y+=2;\
	}

#define PATIENT_EDIT_TYPE_EDATE(name, title, key)\
	const char * v_##name = NULL;\
	{\
		struct tm *tm = localtime(&patient->dateofbirth);\
		char date[11];\
		strftime(date, 11, "%d.%m.%Y", tm);\
		newtComponent l = newtLabel(1, y, title);\
		newtFormAddComponent(form, l);\
		y++;\
		t.m_##name = \
				newtEntry(1, y, date, cols-1,\
				&v_##name, 0);\
		newtFormAddComponent(form, t.m_##name);\
		newtEntrySetFilter(t.m_##name, patient_date_filter, NULL);\
		y+=2;\
	}

#define PATIENT_EDIT_TYPE_ETELE(name, title, key)\
	const char * v_##name = NULL;\
	{\
		newtComponent l = newtLabel(1, y, title);\
		newtFormAddComponent(form, l);\
		y++;\
		t.m_##name = \
				newtEntry(1, y, patient->name, cols-1,\
				&v_##name, 0);\
		newtFormAddComponent(form, t.m_##name);\
		newtEntrySetFilter(t.m_##name, patient_tel_filter, NULL);\
		y+=2;\
	}

#define PATIENT_EDIT_TYPE_TEXTV(name, title, key)
	PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 

// main loop
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
		
		if (ans == t.m_comment || key == NEWT_KEY_F7){
			// save data
#define PATIENT_EDIT_TYPE_ENTRY(name, title, key)\
			if (v_##name)\
				prozubi_passport_set_##key(t.p, patient, v_##name, true);	
#define PATIENT_EDIT_TYPE_EDATE(name, title, key)\
			if (v_##name)\
			{\
				struct tm tm;\
				strptime(v_##name, "%d.%m.%Y", &tm);\
				prozubi_passport_set_##key(t.p, patient, mktime(&tm), true);\
			}
#define PATIENT_EDIT_TYPE_ETELE(name, title, key)\
			if (v_##name)\
				prozubi_passport_set_##key(t.p, patient, v_##name, true);	
#define PATIENT_EDIT_TYPE_TEXTV(name, title, key)
	PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 
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

#endif /* ifndef PATIENT_EDIT */
