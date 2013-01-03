#include "dic.h"
#include "basic.h"
#include "queue.h"

char *get_user_input(char *buffer, int size)
{
	printf("Your Input: ");
	fgets(buffer, size, stdin);
	buffer[strlen(buffer) - 1] = 0;
	return buffer;
}

int counter_trans_num(const char *str)
{
	int i, counter = 1;

	for (i = 0; str[i]; i ++) {
		if (str[i] == '@') {
			counter ++;
		}
	}
	return counter;
}

void display_trans(word *w)
{
	int i;

	for (i = 0; i < w->trans_num; i ++) {
		printf("%s ", w->trans[i]);
	}
	putchar('\n');
}

word *make_word(char buffer[2][4096])
{
	word *w;
	int i = 0;
	char *ret, *tmp;

	w = MALLOC(sizeof(word));
	w->str = MALLOC(strlen(buffer[0]));
	strcpy(w->str, buffer[0] + 1);

	w->trans_num = counter_trans_num(buffer[1]);
	w->trans = MALLOC(sizeof(char *) * w->trans_num);

	tmp = buffer[1] + strlen("Trans:");
	while((ret = strsep(&tmp, "@")) != NULL) {
		w->trans[i] = MALLOC(strlen(ret) + 1);
		strcpy(w->trans[i ++], ret);
	}

	return w;
}

void destroy_word(void *data)
{
	word *w = data;
	FREE(w->str);
	FREE(w->trans);
	FREE(w);
}

int search_cmp(const void *f, const void *s)
{
	char buffer[4096];
	const word *w = f;
	const char *str = s;
	
	strcpy(buffer, str + 1);
	buffer[strlen(buffer) - 1] = 0;
	return strcmp(w->str, buffer);
}

void load_dict_from_file(const char *file, Queue *queue)
{
	FILE *fp;
	word *w;
	char buffer[2][4096];

	fp = fopen_r(file, "a+");

	while(1) {
		if((! fgets(buffer[0], 4096, fp)) ||
		   (! fgets(buffer[1], 4096, fp))) {
			break;
		} else {
			if ((buffer[0][0] != '#') || 
				strncmp(buffer[1], "Trans:", 6)) {
				printf("Bad word in %s.\n", file);
				continue;
			} else {
				buffer[0][strlen(buffer[0]) - 1] = 0;
				buffer[1][strlen(buffer[1]) - 1] = 0;
				w = make_word(buffer);
				if (! queue_is_full(queue)) {
					queue_enqueue(queue, w);
				}
			}
		}
	}
	fclose(fp);
}

void create_dict_binary_index(Queue *queue, dic_option *option)
{
	FILE *fp;
	word *w;
	int i, word_len, trans_len;

	fp = fopen_r(option->sys_def_bin_index_dict, "wb");

	//write index header description.
	fwrite_r(INDEX_HEADER_DESC, 1, sizeof(INDEX_HEADER_DESC), fp);	
	//write index header word number.
	int word_num = queue_element_num(queue);
	fwrite_r(&word_num, sizeof(int), 1, fp);

	while(! queue_is_empty(queue)) {
		//get a word
		w = queue_dequeue(queue);
		//write word length
		word_len = strlen(w->str) + 1;
		fwrite_r(&word_len, sizeof(int), 1, fp);
		//write word
		fwrite_r(w->str, 1, word_len, fp);
		//write trans num
		fwrite_r(&(w->trans_num), sizeof(int), 1, fp);

		for (i = 0; i < w->trans_num; i ++) {
			//write trans[i] lenght;
			trans_len = strlen(w->trans[i]) + 1;
			fwrite_r(&trans_len, sizeof(int), 1, fp);
			//write trans[i]
			fwrite_r(w->trans[i], 1, trans_len, fp);
		}
	}
	fclose(fp);
}

int sort_compare(const void *f, const void *s)
{
	const word *first = f;
	const word *second = s;

	return strcmp(first->str, second->str);
}

void dic_add_new_word_to_self_dict(Queue *queue, dic_option *option)
{
	int i;
	FILE *fp;
	word *w;
	char buffer[2][4096];

	fp = fopen_r(option->self_dict, "w");

	printf("Exit for exit.\n");
	while(1) {
		printf("Input New Word: ");
		strcpy(buffer[0], "#");
		fgets(buffer[0] + 1, 4096, stdin);
		if (! strcmp(buffer[0], "#EXIT\n")) {
			break;
		} else if (queue_search(queue, buffer[0], search_cmp)) {
			printf("Has exist, ignore it.\n");
			continue;
		}
		printf("Input Trans: ");
		//add 'Trans:'
		strcpy(buffer[1], "Trans:");
		fgets(buffer[1] + 6, 4096, stdin);

		//remove '\n'
		buffer[0][strlen(buffer[0]) - 1] = 0;
		buffer[1][strlen(buffer[1]) - 1] = 0;

		w = make_word(buffer);
		if (! queue_is_full(queue)) {
			queue_enqueue(queue, w);
		}
	}

	queue_sort(queue, sort_compare);

	//save words in queue.
	while(! queue_is_empty(queue)) {
		w = queue_dequeue(queue);
		//write word begin with '#'
		fprintf(fp, "#%s\n", w->str);
		//write 'Trans:'
		fprintf(fp, "Trans:");
		for (i = 0; i < w->trans_num; i ++) {
			//write each trans
			fprintf(fp, "%s", w->trans[i]);
			if (i < w->trans_num - 1) {
				fprintf(fp, "@");
			}
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}
