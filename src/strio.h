/**
 * File              : strio.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 27.05.2022
 * Last Modified Date: 25.11.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef k_lib_striio_h__
#define k_lib_striio_h__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>

/* strio_read
 * return allocated string with the contents 
 * of file */
static char * 
strio_read(FILE * fp)
{
	size_t len = 0;
        
	if (fseek (fp, 0, SEEK_END))
		return NULL;

  if ((len = ftell(fp)) < 0)
		return NULL;
    
	if (fseek (fp, 0, SEEK_SET))
		return NULL;

  char *buff = (char *)malloc(len+1);
	if (!buff)
		return NULL;

	fread(buff, len, 1, fp);	
	buff[len] = 0;

	return buff;
}

/* strio_read_range
 * return allocated string with the range contents 
 * of file */
static char *
strio_read_range(FILE * fp, size_t start, size_t end)
{
	if (end <= start)
		return NULL;
    
	size_t len = 0;
        
  char *buff = (char *)malloc(end - start + 1);
	if (!buff)
		return 0;

	if (fseek (fp, start, SEEK_SET))
		return 0;

	while(1){
		char ch = fgetc(fp);
		if (ch == EOF) { 
			break; 
		}
		
		buff[len++] = ch;

		if (len == end - start)
			break;
	}
	buff[len] = 0;

	return buff;
}

/* strio_write
 * write contenst of string to file
 * return 0 on success, -1 on error */
static int 
strio_write(const char * str, FILE * fp)
{
	char * ptr = (char *)str; 
	while(ptr)
		fputc(*ptr++, fp);

	return 0;
}

/* strio_write_to
 * write contenst of string to file in postition
 * return 0 on success, -1 on error */
static int 
strio_write_to(const char * str, FILE * fp, size_t pos)
{
	if (fseek (fp, pos, SEEK_SET))
		return -1;	
    
	char * ptr = (char *)str; 
	while(ptr)
		fputc(*ptr++, fp);

	return 0;
}

/* strio_append
 * write contenst of string to the end of file
 * return 0 on success, -1 on error */
static int 
strio_append(const char * str, FILE * fp)
{
	if (fseek (fp, 0, SEEK_END))
		return -1;
	
	char * ptr = (char *)str; 
	while(ptr)
		fputc(*ptr++, fp);

	return 0;
}

#ifdef __cplusplus
}
#endif

#endif //k_lib_string_h__

