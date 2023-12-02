/**
 * File              : ini.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.02.2023
 * Last Modified Date: 02.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* Read ini config file */

#ifndef INI_PARSE_H__
#define INI_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_ARG_MAX_BYTES 128     // max size of key/value string

/*
 * ini_parse - read file and run callback for each key/value pair
 * return non-null on error to open file
 * @path - config file path
 * @user_data - pointer to data transfered through callback function
 * @callback - callback function - return non-null to stop function execution
 */
static int ini_parse(
		const char *path, void *user_data, 
		int (*callback)(
			void *user_data, 
			const char *section, 
			const char *key, 
			const char *value));

/*
 * ini_get - read file and return value for key or NULL on error
 * @path - config file path
 * @section - set NULL to search in all sections
 * @key
 */
static char *ini_get(
		const char *path, 
		const char *section, const char *key); 

/* IMPLIMATION */

#include <stdio.h>
#include <string.h>
int ini_parse(
		const char *path, void *user_data, 
		int (*callback)(
			void *user_data, 
			const char *section, 
			const char *key, 
			const char *value)) 
{
	char 
		sect [CONFIG_ARG_MAX_BYTES], // section string 
		key  [CONFIG_ARG_MAX_BYTES], // key string 
		value[CONFIG_ARG_MAX_BYTES]; // value string
	int 
		i,                           // iterator
		ch,                          // fgetc char
		klen=0,                      // key string len
		vlen=0,                      // value string len
		first=1,                     // boolean - if char is first in line
		match=0,                     // boolean - if found key/value pair
		need_new_line=0,             // boolean - skip to new line
		comment=0,                   // boolean - if line starts with '#'
		section=0,                 // boolean - if line starts with '['
		b_s=0,                       // boolean - if start '''
		bb_s=0;                      // boolean - if start '''

	*sect = *key = *value = 0;

	FILE *fp = fopen(path, "r");
	if (!fp)
		return -1;

	while((ch=fgetc(fp))!=EOF){
		if (need_new_line && ch != '\n')
			continue;

		if (ch=='\'' && !b_s){
			if (match)
				b_s = 1;
			else 
				need_new_line = 1; // skip this line	
			continue;
		}
		if (ch=='\'' && b_s){
			// close brackets and go to new line
			b_s = 0;
			need_new_line = 1;
			continue;
		}
		if (ch=='"' && !bb_s){
			if (match)
				bb_s = 1;
			else 
				need_new_line = 1; // skip this line	
			continue;
		}
		if (ch=='"' && bb_s){
			// close brackets and go to new line
			bb_s = 0;
			need_new_line = 1;
			continue;
		}

		if ((ch=='#' || ch==';') && !b_s && !bb_s){
			// this is comment - skip line
			need_new_line = 1;
			continue;
		}
		if (ch=='[' && first){
			section = 1;	
			continue;
		}
		if (ch==']' && section){
			// we have a section
			key[klen+1] = 0;
			for (i = 0; i < klen; ++i)
				sect[i] = key[i];

			// free key buffer
			for (i = 0; i < klen; ++i)
				key[i] = 0;
			klen = 0;

			//skip to new line
			need_new_line = 1;
			section = 0;
			continue;
		}
		if (ch=='=') {
			key[klen+1] = 0;
			match = 1;
			continue;
		}
		if (ch == ' ' && !b_s && !bb_s){
			// skip spaces
			continue;
		}
		if (ch=='\n'){
			// do if new line
			need_new_line = 0;
			if (match) {
				// callback key/value pair
				value[vlen+1] = 0;
				if (callback)
					if (callback(user_data, sect, key, value))
						break;
			}
			// free key buffer
			for (i = 0; i < klen; ++i)
				key[i] = 0;
			klen = 0;
			// free value buffer
			for (i = 0; i < vlen; ++i)
				value[i] = 0;
			vlen = 0;
			// init values
			match = 0;
			comment = 0;
			first = 1;
			continue;
		}
		if (match)
			value[vlen++] = ch;
		else
			key[klen++] = ch;
		first = 0;
		if (klen > CONFIG_ARG_MAX_BYTES || 
				vlen > CONFIG_ARG_MAX_BYTES) 
			break;
	}
	return 0;
}

struct _ini_get_d {
	const char *s;
	const char *k;
	char *v;
};

static int _ini_get_cb(
		void *_d, const char *s, 
		const char *k, const char *v)
{
	struct _ini_get_d *d = 
		(struct _ini_get_d *)_d;

	if (d->s) {
		if (strcmp(d->s, s)==0 &&
				strcmp(d->k, k)==0)
		{
			d->v = strdup(v);
			return 1;
		}
	}	else {
		if (strcmp(d->k, k)==0)
		{
			d->v = strdup(v);
			return 1;
		}
	}

	return 0;
}

char *ini_get(
		const char *path, 
		const char *section, const char *key)
{
	struct _ini_get_d d = 
			{section, key, NULL};
	ini_parse(path, &d, _ini_get_cb);
	return d.v;
}	

#ifdef __cplusplus
}
#endif

#endif // INI_PARSE_H__
