/**
 * File              : fileselect.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 08.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef FILE_SELECT_H
#define FILE_SELECT_H

#include "../prozubilib/prozubilib.h"
#include <string.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <newt.h>
#include <dirent.h>
#include "helpers.h"
#include <unistd.h>

static char * staticpath = NULL;

//char *getcwd(char *buffer, int maxlen);

static const char * file_select_fext(const char *filename) {
		const char *dot = strrchr(filename, '.');
		if (!dot || dot == filename)
			return "";
		return dot + 1;
}

static int lastpath(const char *filename) {
		const char *slash = strrchr(filename, '/');
		if (!slash || slash == filename)
			return 0;
		return slash - filename;
}

static bool is_dir(const char *path, const struct dirent *d){
	if (d->d_type == DT_DIR)
		return true;
	char filepath[BUFSIZ];
	snprintf(filepath, BUFSIZ-1, "%s/%s", path, d->d_name);
	DIR *dir;
	if ((dir = opendir(filepath))){
		closedir(dir);
		return true;
	}
	return false;
}

static int file_compar(const struct dirent **a, const struct dirent **b){
	// check if dir
	if (is_dir(staticpath, *a) && !is_dir(staticpath, *b))
		return -1;
	if (!is_dir(staticpath, *a) && is_dir(staticpath, *b))
		return 1;
		
	return strcoll((*a)->d_name, (*b)->d_name);
}

static int 
file_select_filter(const struct dirent *d){
	// no names start with dot
	if (d->d_name[0] == '.'){
		if (d->d_name[1] == '.'){
			if (strcmp(staticpath, "/"))
				return 1;
		}
		return 0;
	}
	if (d->d_type == DT_REG || d->d_type == DT_LNK){
		// allow only *.png, *.tiff, *jpeg
		const char *ext = file_select_fext(d->d_name);
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
		int selected,
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

	// set static path
	staticpath = *path;

	struct dirent **dirents;
	int count = scandir (*path, &(dirents), 
			file_select_filter, file_compar);
	int dl = 0, fl = 0;
	for (i = 0; i < count; ++i) {
		struct dirent *d = dirents[i];
		char name[256];
		if (d->d_type == DT_DIR)
			sprintf(name, "[%s]", d->d_name);
		else if (d->d_type == DT_REG)
			sprintf(name, "*%s", d->d_name);
		else if (d->d_type == DT_LNK)
			if (is_dir(*path, d))
				sprintf(name, "~[%s]", d->d_name);
			else
				sprintf(name, "~%s", d->d_name);
		else 
			sprintf(name, "%s", d->d_name);
		newtListboxAppendEntry(list, name, d);
	}

	// select
	newtListboxSetCurrent(list, selected);
	
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
				// exit with filepath
				strcat(*path, "/");
				strcat(*path, d->d_name);
				ret = -2;
			} else {
				if (strcmp(d->d_name, "..") == 0){
					// remove last path component
					int i = lastpath(*path);
					if (i==0) i = 1;
					path[0][i] = 0;
					ret = 0;
				} else {
					if (is_dir(*path, d)){
						strcat(*path, "/");
						strcat(*path, d->d_name);
						ret = 0;
					} else {
						ret = newtListboxGetByKey(list, d);
					}
				}
			}
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
	while ((ret=file_select_refresh(p, &filepath, ret, cols, rows)) >= 0){
		// do redraw
		newtPopWindow();
		newtCenteredWindow(cols, rows, filepath);
	}
		
	newtPopWindow();
	if (ret == -2){
		// add file to images
		return filepath; 
	}
	else{
		free(filepath);
		return NULL;
	}
}
#endif /* ifndef FILE_SELECT_H */
