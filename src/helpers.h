/**
 * File              : helpers.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 02.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef HELPERS_H
#define HELPERS_H
#include <bits/types/FILE.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <newt.h>

#define PATIENT(title, i, c)\
	struct tm *dob = localtime(&c->dateofbirth);\
	char dateofbirth[11] = {0};\
	strftime(dateofbirth, 11, "%d.%m.%Y", dob);\
	snprintf(title, 255, "%d. %s %s %s %s %s",\
			i + 1,\
			c->familiya ? c->familiya : "",\
			c->imia ? c->imia : "",\
			c->otchestvo ? c->otchestvo : "",\
			dateofbirth,\
			c->tel ? c->tel : ""\
	);\

#define IMAGE(title, i, c)\
	struct tm *d = localtime(&c->date);\
	char date[17] = {0};\
	strftime(date, 17, "%d.%m.%Y %H:%M", d);\
	snprintf(title, 64, "%d. %s %s",\
			i + 1,\
			c->title ? c->title : "",\
			date\
	);\


#define FIO(fio, c)\
	snprintf(fio, 64, "%s %s %s",\
			c->familiya ? c->familiya : "",\
			c->imia ? c->imia : "",\
			c->otchestvo ? c->otchestvo : ""\
	);\

#define PRICE(text, i, c)\
	snprintf(text, 128, "%d. %s %s %s",\
			i + 1,\
			c->title ? c->title : "",\
			c->price ? c->price : "0",\
			c->kod ? c->kod : ""\
	);\

#define PRICE1(text, c)\
	snprintf(text, 128, "%s %s %s",\
			c->title ? c->title : "",\
			c->price ? c->price : "0",\
			c->kod ? c->kod : ""\
	);\


#define NOMENKLATURA(text, c)\
	snprintf(text, 256, "%s %s",\
			c->name ? c->name : "",\
			c->kod ? c->kod : ""\
	);\

static int 
newtListboxGetByKey(
		newtComponent list, void *data)
{
	int count = newtListboxItemCount(list);
	int i;
	for (i = 0; i < count; ++i) {
		void *item;
		newtListboxGetEntry(list, i, NULL, &item);	
		if (item == data)
			return i;
	}
	return -1;
}

#endif /* ifndef HELPERS_H */
