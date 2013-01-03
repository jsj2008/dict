#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dic.h"
#include "queue.h"
#include "basic.h"

void show_usage(void)
{
	printf("\n./app -[text|bin|index] [-f self_dict.txt]\n"
		   "\t-text: search by text mode.\n"
		   "\t-bin: search by binary index mode.\n"
		   "\t-index: create binary index.\n"
		   "\t-f: specific user's own dict.\n");
	exit(0);
}

void parser_option(int argc, char *argv[], dic_option *option)
{
	memset(option, 0, sizeof(*option));

	if (argc == 2) {
		if (! strcmp(argv[1], "-text")) {
			option->mode = DIC_MODE_SEARCH_BY_TEXT;
		} else if (! strcmp(argv[1], "-bin")) {
			option->mode = DIC_MODE_SEARCH_BY_BIN_INDEX;
		} else if (! strcmp(argv[1], "-index")) {
			option->mode = DIC_MODE_CREATE_BINARY_INDEX;
		} else {
			show_usage();
		}
	} else if (argc == 4) {
		if (! strcmp(argv[2], "-f")) {
			if (! strcmp(argv[1], "-text")) {
				option->mode = DIC_MODE_SEARCH_BY_TEXT_WITH_SELF_DICT;
			} else if (! strcmp(argv[1], "-bin")) {
				option->mode = DIC_MODE_SEARCH_BY_BIN_INDEX_WITH_SELF_DICT;
			} else {
				show_usage();
			}		
			strcpy(option->self_dict, argv[3]);
		} else {
			show_usage();
		}
	} else {
		show_usage();
	}
}

static int search_cmp(const void *f, const void *s)
{
	const word *first = f;
	const char *second = s;

	return strcasecmp(first->str, second);
}

void dic_search_by_text_mode(dic_option *option) 
{
	word *w;
	Queue queue;
	char user_input[4096];

	queue_init(&queue);
	load_dict_from_file(option->sys_def_dict, &queue);

	while(strcmp(get_user_input(user_input, 4096), "EXIT")) {
		w = queue_search(&queue, user_input, search_cmp);
		if (! w) {
			printf("%s not found.\n", user_input);
			continue;
		} else {
			display_trans(w);
		}
	}
	queue_destroy(&queue, destroy_word);
}

void load_sys_default_configure(dic_option *option)
{
	strcpy(option->sys_def_dict, SYS_DEFAULT_DICT);
	strcpy(option->sys_def_bin_index_dict, 
		   SYS_DEFAULT_BIN_INDEX_DICT);
}

void dic_search_by_bin_index_mode(dic_option *option) 
{
	FILE *fp;
	int i, word_len, trans_num, trans_len;
	char user_input[4096], buff[1024];

	if ((fp = fopen(option->sys_def_bin_index_dict, "rb")) == NULL) {
		printf("Load %s failed, please create binary index first.\n",
			   option->sys_def_bin_index_dict);
		show_usage();
	}
	while(strcmp(get_user_input(user_input, 4096), "EXIT")) {
		//skip index header description.
		while(fgetc(fp));
		//skip word number
		skip_bytes(fp, sizeof(int));
		while(1) {
			//read word lenght;
			if (fread(&word_len, sizeof(int), 1, fp) != 1) {
				printf("%s not found.\n", user_input);
				break;
			}
			//read word
			fread_r(buff, 1, word_len, fp);
			if (! strcmp(buff, user_input)) {
				//read trans num
				fread_r(&trans_num, sizeof(int), 1, fp);
				for (i = 0; i < trans_num; i ++) {
					//read trans[i]'s length
					fread_r(&trans_len, sizeof(int), 1, fp);
					//read trans[i]
					fread_r(buff, 1, trans_len, fp);
					printf("%s ", buff);
				}
				putchar('\n');
				break;
			} else {
				//read trans num
				fread_r(&trans_num, sizeof(int), 1, fp);
				for (i = 0; i < trans_num; i ++) {
					//read trans[i]'s length
					fread_r(&trans_len, sizeof(int), 1, fp);
					//skip this trans
					skip_bytes(fp, trans_len);
				}
			}
		}
		rewind(fp);
	}	
}

void dic_search_by_text_mode_with_self_dict(dic_option *option) 
{
	word *w;
	char user_input[4096];
	Queue sys_queue, self_queue;

	queue_init(&sys_queue);
	queue_init(&self_queue);

	load_dict_from_file(option->sys_def_dict, &sys_queue);
	load_dict_from_file(option->self_dict, &self_queue);

	printf("1. Search A Word. 2. Add New Word.\n");
	get_user_input(user_input, 1024);

	if (! strcmp(user_input, "1")) {
		while(strcmp(get_user_input(user_input, 4096), "EXIT")) {
			w = queue_search(&sys_queue, user_input, search_cmp);
			if (! w) {
				printf("%s not found in %s.\n", user_input,
					   option->sys_def_dict);
				w = queue_search(&self_queue, user_input, search_cmp);
				if (! w) {
					printf("%s not found in %s.\n", user_input,
						   option->self_dict);
				} else {
					display_trans(w);
				}
				continue;
			} else {
				display_trans(w);
			}
		}
	} else if (! strcmp(user_input, "2")) {
		dic_add_new_word_to_self_dict(&self_queue, option);
	} else {
		printf("Bad Select.\n");
	}

	queue_destroy(&sys_queue, destroy_word);
	queue_destroy(&self_queue, destroy_word);
}

void dic_search_by_bin_index_mode_with_self_dict(dic_option *option) 
{
	FILE *fp;
	word *w;
	Queue self_queue;
	char user_input[4096], buff[1024];
	int i, word_len, trans_num, trans_len;

	queue_init(&self_queue);
	load_dict_from_file(option->self_dict, &self_queue);


	if ((fp = fopen(option->sys_def_bin_index_dict, "rb")) == NULL) {
		printf("Load %s failed, please create binary index first.\n",
			   option->sys_def_bin_index_dict);
		show_usage();
	}

	printf("1. Search A Word. 2. Add New Word.\n");
	get_user_input(user_input, 1024);

	if (! strcmp(user_input, "1")) {
		while(strcmp(get_user_input(user_input, 4096), "EXIT")) {
			//skip index header description.
			while(fgetc(fp));
			//skip word number
			skip_bytes(fp, sizeof(int));
			while(1) {
				//read word lenght;
				if (fread(&word_len, sizeof(int), 1, fp) != 1) {
					printf("%s not found in %s.\n", 
						   user_input, option->sys_def_bin_index_dict);

					w = queue_search(&self_queue, user_input, search_cmp);
					if (! w) {
						printf("%s not found in %s.\n", user_input,
							   option->self_dict);
					} else {
						display_trans(w);
					}
					break;
				}
				//read word
				fread_r(buff, 1, word_len, fp);
				if (! strcmp(buff, user_input)) {
					//read trans num
					fread_r(&trans_num, sizeof(int), 1, fp);
					for (i = 0; i < trans_num; i ++) {
						//read trans[i]'s length
						fread_r(&trans_len, sizeof(int), 1, fp);
						//read trans[i]
						fread_r(buff, 1, trans_len, fp);
						printf("%s ", buff);
					}
					putchar('\n');
					break;
				} else {
					//read trans num
					fread_r(&trans_num, sizeof(int), 1, fp);
					for (i = 0; i < trans_num; i ++) {
						//read trans[i]'s length
						fread_r(&trans_len, sizeof(int), 1, fp);
						//skip this trans
						skip_bytes(fp, trans_len);
					}
				}
			}
			rewind(fp);
		}
	} else if (! strcmp(user_input, "2")) {
		dic_add_new_word_to_self_dict(&self_queue, option);
	} else {
		printf("Bad Select.\n");
	}
	queue_destroy(&self_queue, destroy_word);
}

void dic_create_binary_index(dic_option *option)
{
	Queue queue;

	queue_init(&queue);
	load_dict_from_file(option->sys_def_dict, &queue);
	create_dict_binary_index(&queue, option);
	queue_destroy(&queue, destroy_word);
}

typedef void (*psearch_func)(dic_option *);

psearch_func search_funcs[] = {
	dic_search_by_text_mode,
	dic_search_by_bin_index_mode,
	dic_search_by_text_mode_with_self_dict,
	dic_search_by_bin_index_mode_with_self_dict,
	dic_create_binary_index
};

int main(int argc, char *argv[])
{
	dic_option option;

	parser_option(argc, argv, &option);
	load_sys_default_configure(&option);
	search_funcs[option.mode](&option);

	return 0;
}
