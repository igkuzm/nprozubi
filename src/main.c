/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.07.2023
 * Last Modified Date: 08.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include <newt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../prozubilib/prozubilib.h"
#include "../prozubilib/passport.h"

#include "askstring.h"
#include "caseslist.h"
#include "dialog.h"
#include "main.h"
#include "keybar.h"
#include "patientedit.h"
#include "colors.h"
#include "helpers.h"
#include "switcher.h"
#include "asktoremove.h"
#include "asktoexit.h"
#include "patientslist.h"
#include "priceslist.h"
#include "priceedit.h"
#include "nomenklatura.h"
#include "error.h"
#include "ini.h"

struct nomenklatura_add_t {
	prozubi_t *p;
	newtComponent list;
};

void nomenklatura_add_cb(
			void *userdata,
			const char *title,
			const char *kod)
{
	struct nomenklatura_add_t *t = userdata;
	struct price_t *c = 
		prozubi_price_new(
				t->p, 
				title, 
				"", 
				kod, 
				"10000", 
				title, 
				NULL);
	if (c){
		int count = newtListboxItemCount(t->list);
		//char title[256];
		//PRICE(title, count-1, c);
		//newtListboxAppendEntry(t->list, title, t->p);
		free(c);
	}
}

void
on_destroy(
		newtComponent list, void *data)
{
	SWITCH *selected = data;
	int count = newtListboxItemCount(list);
	int i;
	for (i = 0; i < count; ++i) {
		if (*selected == SW_PATIENTS){
			struct passport_t *patient;
			newtListboxGetEntry(list, i, NULL, (void **)&patient);	
			prozubi_passport_free(patient);
		} else if (*selected == SW_DOCTORS){
		} else if (*selected == SW_PRICES){
			struct price_t *price;
			newtListboxGetEntry(list, i, NULL, (void **)&price);	
			prozubi_prices_free(price);
		}
	}
}


int main(int argc, char *argv[])
{
	// try to load token
	char *token = ini_get(
		"nprozubi.ini", NULL, "TOKEN");

	SWITCH selected = SW_PATIENTS;
	int i, cols, rows, key, appclose = 0;
	newtComponent form, list=NULL, switcher, ans=NULL;
	struct newtExitStruct toexit;

	newtInit();
	newtCls();
	newtGetScreenSize(&cols, &rows);

	prozubi_t *p =
			prozubi_init(
					"prozubi.sqlite", token, 
					//NULL, error_callback, 
					NULL, error_callback, 
					NULL, log_callback);

	// create form
	form = newtForm(NULL, NULL, 0);
	/*newtFormSetBackground(form, 1);*/
	newtFormSetHeight(form, rows);
	newtFormSetWidth (form, cols);
	newtFormAddHotKey(form, NEWT_KEY_F1);
	newtFormAddHotKey(form, NEWT_KEY_F2);
	newtFormAddHotKey(form, NEWT_KEY_F3);
	newtFormAddHotKey(form, NEWT_KEY_F4);
	newtFormAddHotKey(form, NEWT_KEY_F5);
	newtFormAddHotKey(form, NEWT_KEY_F6);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F8);
	newtFormAddHotKey(form, NEWT_KEY_F9);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');
	newtFormAddHotKey(form, 'd');
	newtFormAddHotKey(form, 'e');
	newtFormAddHotKey(form, 'v');
	newtFormAddHotKey(form, 's');
	newtFormAddHotKey(form, '/');
	newtFormAddHotKey(form, 'r');
	
	// add widgets to form
	keybar_new(form, cols, rows);
	switcher_new(form, &switcher, cols, rows);
	patients_list_new(p, form, &list, &selected, NULL, cols, rows);

	// main loop
	do{
		newtFormSetCurrent(form, list);
		newtFormRun(form, &toexit);
		
		ans = newtFormGetCurrent(form);
		key = toexit.u.key;
		
		switch (key) {
			case NEWT_KEY_F7: case 'a':
				{
					if (selected == SW_PATIENTS){
						// add new patinet
						struct passport_t *c = 
							prozubi_passport_new(
								p,
							  "",	
								"Новый", 
								"пациент", 
								"", 
								"", 
								"", 
								"", 
								"", 
								"", 
								0, 
								NULL
							);
						if (c){
							if (patient_edit_new(p, c, list)){
								free(c);
								//int count = newtListboxItemCount(list);
								//char title[256];
								//PATIENT(title, count, c);
								//newtListboxAppendEntry(list, title, p);
								patients_list_new(p, form, &list, &selected, NULL, cols, rows);
							}
							else{
								free(c);
							}
						}
						break;
					}
					if (selected == SW_PRICES){
						// add new price from nomenklatura
						struct nomenklatura_add_t t = {p, list};
						nomenklatura_new(
								p, NULL, 
								cols, rows, 
								&t, nomenklatura_add_cb);
						prices_list_new(p, form, &list, &selected, NULL, cols, rows);
						break;
					}

					break;
				}
			case NEWT_KEY_F3: case 's': case '/':
				{
					if (selected == SW_PATIENTS){
						char *search = ask_string("Поиск: Фамилия, Имя, Отчество, телефон, email");
						if (search && strlen(search) > 1){
							patients_list_new(p, form, &list, &selected, search, cols, rows);
						}
						if (search)
							free(search);
					} else if (selected == SW_PRICES){
						char *search = ask_string("Поиск: Наименование, код");
						if (search && strlen(search) > 1){
							prices_list_new(p, form, &list, &selected, search, cols, rows);
						}
						if (search)
							free(search);
					}
					break;
				}
			case NEWT_KEY_F4: case 'e':
				{
					if (selected == SW_PATIENTS){
						// edit patient
						struct passport_t *c = 
							newtListboxGetCurrent(list);
						if (c){
							if (patient_edit_new(p, c, list)){
								int num = newtListboxGetByKey(list, c);
								//char title[256];
								//PATIENT(title, num, c);
								//newtListboxSetEntry(list, num, title);
								patients_list_new(p, form, &list, &selected, NULL, cols, rows);
								newtListboxSetCurrent(list, num);
							}
						}
						break;
					}
					if (selected == SW_PRICES){
						// edit price
						struct price_t *c = 
							newtListboxGetCurrent(list);
						if (c){
							if (price_edit_new(p, c, list)){
								int num = newtListboxGetByKey(list, c);
								//char title[256];
								//PRICE(title, num, c);
								//newtListboxSetEntry(list, num, title);
								prices_list_new(p, form, &list, &selected, NULL, cols, rows);
								newtListboxSetCurrent(list, num);
							}
						}
						break;
					}
					break;
				}
			case NEWT_KEY_F8: case 'd':
				{
					// remove
					if (selected == SW_PATIENTS){
						struct passport_t *c = 
							newtListboxGetCurrent(list);
						if (c){
							char text[256], fio[64];
							FIO(fio, c);
							sprintf(text, "Вы хотите удалить запись из базы: %s", fio);
							if (ask_to_remove(text)){
								prozubi_passport_remove(p, c);
								newtListboxDeleteEntry(list, c);
								free(c);
							}
						}
						break;
					}
					if (selected == SW_PRICES){
						struct price_t *c = 
							newtListboxGetCurrent(list);
						if (c){
							int num = newtListboxGetByKey(list, c);
							char text[512], title[256];
							PRICE(title, num + 1, c);
							sprintf(text, "Вы хотите удалить запись из базы: %s", title);
							if (ask_to_remove(text)){
								prozubi_price_remove(p, c);
								newtListboxDeleteEntry(list, c);
								free(c);
							}
						}
						break;
					}
					break;
				}
			case NEWT_KEY_RETURN: case 'v':
			{
				if (ans == list){
					if (selected == SW_PATIENTS){
						struct passport_t *c = 
								newtListboxGetCurrent(list);
						if (c){
							cases_list_new(p, c, cols, rows);
						}
					break;	
					}
				}
				if (ans == switcher){
					int *index = 
							newtListboxGetCurrent(switcher);
					if (*index == SW_PATIENTS){
						// patients list
						patients_list_new(p, form, &list, &selected, NULL, cols, rows);
						selected = SW_PATIENTS;
						break;
					}	
					if (*index == SW_DOCTORS){
						// doctors list
						break;
					} 
					if (*index == SW_PRICES){
						// price list
						prices_list_new(p, form, &list, &selected, NULL, cols, rows);
						selected = SW_PRICES;
						break;
					}
				}
				break;
			}
			case NEWT_KEY_ESCAPE: case 'r':
			{
				//refresh
				if (selected == SW_PATIENTS)
					patients_list_new(p, form, &list, &selected, NULL, cols, rows);
				else if (selected == SW_PRICES)	
					prices_list_new(p, form, &list, &selected, NULL, cols, rows);
				break;
			}
			case NEWT_KEY_F10: case 'q':
				{
					if (ask_to_exit())
						appclose = 1;
					break;
				}
		
			default:
				break;
		}
	} while (appclose != 1);
	
	newtFormDestroy(form);
	newtFinished();
	
	return 0;
}
