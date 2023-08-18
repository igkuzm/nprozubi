/**
 * File              : fileselect.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 28.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef FILE_SELECT_H
#define FILE_SELECT_H

#include "../prozubilib/prozubilib.h"
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <newt.h>
#include <dirent.h>

char *getcwd(char *buffer, int maxlen);

static const char * fext(const char *filename) {
		const char *dot = strrchr(filename, '.');
		if (!dot || dot == filename)
			return "";
		return dot + 1;
}

static int 
file_select_filter(const struct dirent *d){
	// no names start with dot
	if (d->d_name[0] == '.'){
		if (d->d_name[1] == '.')
			return 1;
		else
			return 0;
	}
	if (d->d_type == DT_REG){
		// allow only *.png, *.tiff, *jpeg
		const char *ext = fext(d->d_name);
		if (strcmp(ext, "png") == 0) return 1;
		if (strcmp(ext, "PNG") == 0) return 1;
		if (strcmp(ext, "jpg") == 0) return 1;
		if (strcmp(ext, "JPG") == 0) return 1;
		if (strcmp(ext, "jpeg") == 0) return 1;
		if (strcmp(ext, "tif") == 0) return 1;
		if (strcmp(ext, "tiff") == 0) return 1;
		if (strcmp(ext, "TIFF") == 0) return 1;
		if (strcmp(ext, "TIF") == 0) return 1;
		if (strcmp(ext, "gif") == 0) return 1;
		if (strcmp(ext, "GIF") == 0) return 1;
		if (strcmp(ext, "bmp") == 0) return 1;
		if (strcmp(ext, "BMP") == 0) return 1;
		return 0;
	}
	return 1;
}

static void 
file_select_on_destroy(
		newtComponent tree, void *data)
{
	//int count = newtListboxItemCount(list);
	//int i;
	//for (i = 0; i < count; ++i) {
		//struct price_t *price;
		//newtListboxGetEntry(list, i, NULL, (void **)&price);	
		//prozubi_prices_free(price);
	//}
}

static int
file_select_refresh(
		prozubi_t *p, 
		char **path,
		int cols, int rows)
{
	int i, key, ret=0;
	
	newtComponent form, list, ans=NULL;
	struct newtExitStruct toexit;
	
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, NEWT_KEY_RETURN);
	
	// create new
	list =
				newtListbox(0, 0, rows, NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
	newtFormAddComponent(form, list);
	newtListboxSetWidth(list, cols);
	newtComponentAddDestroyCallback(list, file_select_on_destroy, NULL);

	struct dirent **files;
	int count = scandir (*path, &(files), 
			file_select_filter, alphasort);
	for (i = 0; i < count; ++i) {
		struct dirent *d = files[i];
		char name[256];
		if (d->d_type == DT_DIR)
			sprintf(name, "[%s]", d->d_name);
		else if (d->d_type == DT_REG)
			sprintf(name, "*%s", d->d_name);
		else if (d->d_type == DT_LNK)
			sprintf(name, "~%s", d->d_name);
		else 
			sprintf(name, "%s", d->d_name);
		newtListboxAppendEntry(list, name, d);
	}
	
	newtFormRun(form, &toexit);
	key  = -1;
	if (toexit.reason == NEWT_EXIT_COMPONENT)
		ans = toexit.u.co;
	else
		key = toexit.u.key;

	if (key == NEWT_KEY_ESCAPE
			|| key == NEWT_KEY_F10
			|| key == 'q'
			)
	{
		// exit without save
		ret = -1;
	}
	if (key == NEWT_KEY_RETURN){
		// add path
		struct dirent *d = 
			(struct dirent *)newtListboxGetCurrent(list);
		if(d){
			if (d->d_type == DT_REG){
				ret = 1;
			}
			strcat(*path, "/");
			strcat(*path, d->d_name);
		}
	}

	newtFormDestroy(form);
	return ret;
}

static char *
file_select_new(
		prozubi_t *p, 
		struct case_t *c, 
		const char *path,
		int cols, int rows)
{
	int i, key, num;
	cols -= 10;
	rows -= 6;

	char *filepath = (char *)malloc(BUFSIZ);
	if (!filepath){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	if (path)
		strcpy(filepath, path);
	else 
		getcwd(filepath, BUFSIZ);
	
	newtCenteredWindow(cols, rows, filepath);

	int ret; 
	while ((ret=file_select_refresh(p, &filepath, cols, rows)) == 0){
		// do redraw
		newtPopWindow();
		newtCenteredWindow(cols, rows, filepath);
	}
		
	newtPopWindow();
	if (ret == 1){
		// add file to images
		return filepath; 
	}
	else{
		free(filepath);
		return NULL;
	}
}
#endif /* ifndef FILE_SELECT_H */
