/**
 * File              : config.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.02.2023
 * Last Modified Date: 05.11.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
/* Read config file */

#ifndef k_lib_config_h__
#define k_lib_config_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef logfile
#define logfile stderr
#endif
#ifndef ERR
#define ERR(...) ({fprintf(logfile, __VA_ARGS__);})
#endif	

#define CONFIG_ARG_MAX_BYTES 128     // max size of key/value value

/*
 * read_config_file - read file and run callback for each key/value pair
 * @fp - stream FILE pointer 
 * @user_data - pointer to data transfered through callback function
 * @callback - callback function - return non zero to stop function execution
 */
static void read_config_file(
		FILE *fp, void *user_data, 
		int (*callback)(
			void *user_data, const char *key, const char *value)) 
{
	char 
		key[CONFIG_ARG_MAX_BYTES],   // key string 
		value[CONFIG_ARG_MAX_BYTES]; // value string
	int 
		i,                           // iterator
		ch,                          // fgetc char
		klen=0,                      // key string len
		vlen=0,                      // value string len
		first=1,                     // boolean - if char is first in line
		match=0,                     // boolean - if found key/value pair
		comment=0;                   // boolean - if line starts with '#'

	while((ch=fgetc(fp))!=EOF){
		if (ch=='#' && first)
			comment = 1;	
		if (ch=='=') {
			key[klen+1] = 0;
			match = 1;
			continue;
		}
		if (ch=='\n'){
			if (match & !comment) {
				value[vlen+1] = 0;
				if (callback)
					if (callback(user_data, key, value))
						break;
			}
			for (i = 0; i < klen; ++i)
				key[i] = 0;
			klen = 0;
			for (i = 0; i < vlen; ++i)
				value[i] = 0;
			vlen = 0;
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
		{
			ERR("read_config_file: error: arg is to long, "
			"more then %d len", CONFIG_ARG_MAX_BYTES);	
			break;
		}
	}
}

/*
 * write_config_file - write key/value pair to file (append)
 * @fp - stream FILE pointer 
 * @key - null-terminated key string
 * @value - null-terminated value string
 */
static void write_config_file(
		FILE *fp, const char *key, const char *value) 
{

	int i=0;                         // iterator
	while (i < CONFIG_ARG_MAX_BYTES && key[i])
		fputc(key[i++], fp);	
	
	fputc('=', fp);	
	
	i=0;
	while (i < CONFIG_ARG_MAX_BYTES && value[i])
		fputc(value[i++], fp);	
	
	fputc('\n', fp);	
}


#ifdef __cplusplus
}
#endif

#endif // k_lib_config_h__
