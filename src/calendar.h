/**
 * File              : calendar.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 22.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdio.h>
#include <newt.h>
#include <time.h>

/* return char index of utf8 string n'th multibite char */
static int
uchar_index(char *str, int n)
{
	int i = 0, k;
	for (k = 0; k < n; ++k) {
		unsigned char c = str[i];
		if      (c >= 252) /* 6-bytes */
			i+=6;
		else if (c >= 248) /* 5-bytes */
			i+=5;
		else if (c >= 240) /* 4-bytes */
			i+=4;
		else if (c >= 224) /* 3-bytes */
			i+=3;
		else if (c >= 192) /* 2-bytes */
			i+=2;
		else               /* 1-byte  */
			i++;
	}
	return i;
}

enum cal_s{
	s_day,     // selection of days
	s_month,   // selection of month
	s_year,    // selection of year
};

struct calendar {
	struct tm *tm;
	int startofweek; // 0 - for sunday, 1 - for monday, ...
	enum cal_s sel;
};


static const char * calendar_dnames[] = 
{
	"  ",
	" 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10",
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
	"31"
};

#define calendar_iterate(it, tm) \
	it = *tm;\
	it.tm_mday = 1;\
	mktime(&it);\
	for (; it.tm_mon == tm->tm_mon; it.tm_mday++, mktime(&it))\

static void 
calendar_on_destroy(
		newtComponent l, void *data)
{
	// redraw labels before destroy
	newtLabelSetColors(l, NEWT_COLORSET_LABEL);
	newtLabelSetText(l, "  ");
}

static int calendar_draw(struct calendar *cal)
{

	struct newtExitStruct toexit;
	int i, key, x, y, w=20, h=8, ret=0;
	
	newtComponent form = newtForm(NULL, 0, NEWT_FLAG_HIDDEN);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_LEFT);
	newtFormAddHotKey(form, '-');
	newtFormAddHotKey(form, NEWT_KEY_RIGHT);
	newtFormAddHotKey(form, '+');
	newtFormAddHotKey(form, NEWT_KEY_UP);
	newtFormAddHotKey(form, NEWT_KEY_DOWN);
	newtFormAddHotKey(form, NEWT_KEY_TAB);
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	newtFormAddHotKey(form, 'q');

	// set date
	struct tm *tm = cal->tm;
	tm->tm_hour = 0;
	tm->tm_min = 0;
	mktime(tm);
	
	// print month name
	char mname[20];
	strftime(mname, 20, "%B", tm);
	{
		newtComponent l =
				newtLabel(0, 0, mname);
		newtComponentAddDestroyCallback(l, calendar_on_destroy, NULL);
		if (cal->sel == s_month)
			newtLabelSetColors(l, NEWT_COLORSET_BUTTON);
		newtFormAddComponent(form, l);
	}
	
	// print year
	char year[5];
	sprintf(year, "%d", tm->tm_year + 1900);
	{
		newtComponent l =
				newtLabel(w-4, 0, year);
		if (cal->sel == s_year)
			newtLabelSetColors(l, NEWT_COLORSET_BUTTON);
		newtComponentAddDestroyCallback(l, calendar_on_destroy, NULL);
		newtFormAddComponent(form, l);
	}
		
	// print week names
	struct tm tw = *tm;
	for (i = 0; i < 7; i++){
		tw.tm_wday = cal->startofweek + i;
		if (tw.tm_wday > 6)
			tw.tm_wday = 0;
		char wname[5];
		strftime(wname, 5, "%a", &tw);
		// terminate string after 2 char
		int index = uchar_index(wname, 2);
		wname[index] = 0;

		newtFormAddComponent(form,
				newtLabel(i*3, 1, wname));
	}

	// fill with dates
	y = 2; 
	struct tm tp;
	calendar_iterate(tp, tm){
		if (tp.tm_mday == 1){
			i = tp.tm_wday - cal->startofweek;
			if (i < 0)
				i += 7;
		}
		x = i*3;
		{
			newtComponent l =
					newtLabel(x, y, calendar_dnames[tp.tm_mday]);
			if (cal->sel == s_day && tp.tm_mday == tm->tm_mday)
				newtLabelSetColors(l, NEWT_COLORSET_BUTTON);
			newtComponentAddDestroyCallback(l, calendar_on_destroy, NULL);
			newtFormAddComponent(form, l);
		}
		if (++i == 7){
			i = 0;
			y++;	
		}
	}
	
	newtFormRun(form, &toexit);
	key = toexit.u.key;
	
	switch (key) {
		case NEWT_KEY_ESCAPE: case NEWT_KEY_F10: case 'q':
			{
				// exit without save
				ret=-1;
				break;
			}
		case NEWT_KEY_RETURN:
			{
				// exit with save
				ret=1;
				break;
			}

		case NEWT_KEY_TAB:
			{
				cal->sel++;
				if (cal->sel > s_year)
					cal->sel = s_day; 
				break;
			}
		case NEWT_KEY_RIGHT: case '+':
			{
				if (cal->sel == s_day)
					cal->tm->tm_mday++;
				else if (cal->sel == s_month)
					cal->tm->tm_mon++;
				else if (cal->sel == s_year)
					cal->tm->tm_year++;
				mktime(cal->tm);
				break;
			}

		case NEWT_KEY_LEFT: case '-':
			{
				if (cal->sel == s_day)
					cal->tm->tm_mday--;
				else if (cal->sel == s_month)
					cal->tm->tm_mon--;
				else if (cal->sel == s_year)
					cal->tm->tm_year--;
				mktime(cal->tm);
				break;
			}
		
		case NEWT_KEY_DOWN:
			{
				if (cal->sel == s_day)
					cal->tm->tm_mday += 7;
				else if (cal->sel == s_month)
					cal->tm->tm_mon++;
				else if (cal->sel == s_year)
					cal->tm->tm_year++;
				mktime(cal->tm);
				break;
			}

		case NEWT_KEY_UP:
			{
				if (cal->sel == s_day)
					cal->tm->tm_mday -= 7;
				else if (cal->sel == s_month)
					cal->tm->tm_mon--;
				else if (cal->sel == s_year)
					cal->tm->tm_year--;
				mktime(cal->tm);
				break;
			}
	}	

	newtFormDestroy(form);
	return ret;
}

static time_t
calendar_new(time_t date)
{
	struct calendar cal;
	cal.sel = s_day;
	cal.startofweek = 1;
	time_t t = time(NULL);
	cal.tm = localtime(&t);

	newtCenteredWindow(20, 8, "CAL");
	
	int ret; 
	while ((ret=calendar_draw(&cal)) == 0){
		// do redraw calendar
	}
		
	newtPopWindow();
	if (ret == 1)
		return mktime(cal.tm);
	else
		return date;
}

#endif /* ifndef CALENDAR_H */
