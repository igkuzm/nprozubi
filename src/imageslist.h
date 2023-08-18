/**
 * File              : imageslist.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.07.2023
 * Last Modified Date: 29.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef IMAGES_LIST_H
#define IMAGES_LIST_H

#include "../prozubilib/prozubilib.h"
#include "../prozubilib/images.h"
#include "asktoremove.h"
#include "helpers.h"
#include "fileselect.h"
#include <newt.h>

static void 
images_list_on_destroy(
		newtComponent list, void *data)
{
	int count = newtListboxItemCount(list);
	int i;
	for (i = 0; i < count; ++i) {
		struct image_t *image;
		newtListboxGetEntry(list, i, NULL, (void **)&image);	
		prozubi_image_free(image);
	}
}

static int 
get_images(void *data, struct image_t *image){
	newtComponent list = (newtComponent)data;
	int count = newtListboxItemCount(list);
	char title[128];
	IMAGE(title, count, image);
	newtListboxAppendEntry(list, title, image);
	return 0;
}

static void
images_list_new(
		prozubi_t *p, 
		struct case_t *c, 
		const char *search,
		int cols, int rows)
{
	const void **result;
	int i, key, num;
	cols -= 10;
	rows -= 6;

	newtComponent form, list, ans=NULL;
	struct newtExitStruct toexit;
	
	newtCenteredWindow(cols, rows, "Список изображений (снимков)");
	// create form
	form = newtForm(NULL, NULL, 0);
	newtFormAddHotKey(form, NEWT_KEY_F7);
	newtFormAddHotKey(form, NEWT_KEY_F8);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormAddHotKey(form, NEWT_KEY_ESCAPE);
	newtFormAddHotKey(form, NEWT_KEY_ENTER);
	newtFormAddHotKey(form, 'q');
	newtFormAddHotKey(form, 'a');
	newtFormAddHotKey(form, 'd');
	newtFormAddHotKey(form, 'v');
	
	// create new
	list =
				newtListbox(0, 0, rows, NEWT_FLAG_RETURNEXIT|NEWT_FLAG_SCROLL|NEWT_FLAG_BORDER);
	newtFormAddComponent(form, list);
	newtListboxSetWidth(list, cols);
	newtComponentAddDestroyCallback(list, images_list_on_destroy, NULL);

	// fill list with patients
	char *predicate = NULL;
	if (search){
		predicate = (char *)malloc(BUFSIZ);
		if (!predicate){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		sprintf(predicate, 
				"AND ZTITLE LIKE %%%s%% "
				, search
				);
	}
	prozubi_image_foreach(p, c->id, predicate, list, get_images);
	
	do{
		newtFormRun(form, &toexit);
		key  = -1;
		if (toexit.reason == NEWT_EXIT_COMPONENT)
			ans = toexit.u.co;
		else
			key = toexit.u.key;
	
		if (key == NEWT_KEY_ESCAPE || key == 'q'){
			// exit without save
			break;
		}
		if (key == NEWT_KEY_ENTER || key == 'v'){
			if (newtListboxItemCount(list)>0){
				struct image_t *img = 
					(struct image_t *)newtListboxGetCurrent(list);
				if (img){
					remove("out.jpg");
					FILE *out = fopen("out.jpg", "w+");
					if (out){
						fwrite(img->data, img->len_data, 1, out);
						newtSuspend();
						system("fbi out.jpg");	
						newtResume();
					}
				}
			}
			continue;
		}
		if (key == NEWT_KEY_F8 || key == 'd'){
			if (newtListboxItemCount(list)>0){
				struct image_t *img = 
					(struct image_t *)newtListboxGetCurrent(list);
				if (img){
					if (ask_to_remove(img->title)){
						prozubi_image_remove(p, img);
						images_list_on_destroy(list, NULL);
						newtListboxClear(list);
						prozubi_image_foreach(p, c->id, predicate, list, get_images);
					}
				}
			}
			continue;
		}
		if (key == NEWT_KEY_F7 || key == 'a'){
			// add items
			char *file = file_select_new(p, c, NULL, cols, rows);
			if (file){
				struct image_t *img =
						prozubi_image_new(p, time(NULL), 
								"снимок", c->id, NULL, 0, NULL);
				if (img){
					if (prozubi_image_set_image_from_file(
								p, img, file)){
						prozubi_image_remove(p, img);
					}
				}
				free(file);
			}
			images_list_on_destroy(list, NULL);
			newtListboxClear(list);
			prozubi_image_foreach(p, c->id, predicate, list, get_images);
			continue;
		}
	} while (key != NEWT_KEY_F10);

	if (predicate)
		free(predicate);
	
	newtFormDestroy(form);
	newtPopWindow();
}
#endif /* ifndef IMAGES_LIST_H */
