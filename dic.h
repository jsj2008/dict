#ifndef _DIC_H_
#define _DIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define PATH_MAX	4096
#define SYS_DEFAULT_DICT			  "dict.txt"
#define SYS_DEFAULT_BIN_INDEX_DICT    "dict.dat"
#define INDEX_HEADER_DESC	"This is header index description."

typedef struct {
	char *str;
	int trans_num;
	char **trans;
} word;

/*Search mode*/
typedef enum dic_mode {
	DIC_MODE_SEARCH_BY_TEXT = 0,
	DIC_MODE_SEARCH_BY_BIN_INDEX,
	DIC_MODE_SEARCH_BY_TEXT_WITH_SELF_DICT,
	DIC_MODE_SEARCH_BY_BIN_INDEX_WITH_SELF_DICT,
	DIC_MODE_CREATE_BINARY_INDEX
} dic_mode;

typedef struct {
	enum dic_mode mode;
	char self_dict[PATH_MAX];
	char sys_def_dict[PATH_MAX];
	char sys_def_bin_index_dict[PATH_MAX];
} dic_option;

extern void destroy_word(void *data);
extern void display_trans(word *w);
extern char *get_user_input(char *buffer, int size);
extern void load_dict_from_file(const char *file, Queue *queue);
extern void create_dict_binary_index(Queue *queue, dic_option *option);
extern void dic_add_new_word_to_self_dict(Queue *queue, dic_option *option);
#endif //_DIC_H_
