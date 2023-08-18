/**
 * File              : main.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 24.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <newt.h>

typedef enum {
	SW_PATIENTS,
	SW_DOCTORS,
	SW_PRICES
} SWITCH;

void on_destroy(newtComponent list, void *data);

#endif /* ifndef MAIN_H */
