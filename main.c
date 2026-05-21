#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define MAX_VARS 32
#define MAX_CMD_LEN 64

#define OK 0
#define ERR 1

typedef struct {
	char id[16];
	int val;
} var_t;

static var_t var_tabl[32];
static uint8_t var_count = 0;

void print_int(int n, FILE* stream);

void throw_error(int line_num, char* error_msg);
void throw_immediate_error(char* error_msg);

void var_set(char* id, int val) {

    if(strcmp(id, "stdout") == 0) {
	    print_int(val, stdout);
	    fputs("\n", stdout);
	    return;
    }

    if(strcmp(id, "stderr") == 0) {
	    print_int(val, stderr);
	    fputs("\n", stderr);
	    return;
    }

    if(strcmp(id, "stdin") == 0) {
	    throw_immediate_error("cannot move into stdin!");
    }

    for(int i = 0; i < var_count; i++) {
        if(strcmp(var_tabl[i].id, id) == 0) {
            var_tabl[i].val = val;
            return;
        }
    }

    if(var_count < MAX_VARS) {
        strncpy(var_tabl[var_count].id, id, 15);
        var_tabl[var_count].val = val;
        var_count++;
    }
}

int var_get(char* id) {

	if(strcmp(id, "stdin") == 0) {
		char buf[16];
		fgets(buf, sizeof(buf), stdin);
		return atoi(buf);
	}

	for(int i = 0; i < var_count; i++) {
		if(strcmp(var_tabl[i].id, id) == 0) {
			return var_tabl[i].val;
		}
	}

	return 0;
}

int var_resolve(char* tok) {
	if((strcmp(tok, "stdout")) == 0
	||(strcmp(tok, "stderr")) == 0)
	throw_immediate_error("not a valid variable name!");

	if(strcmp(tok, "zero") == 0) return 0;

	if(tok[0] == '-') {
	       	if(tok[1] >= '0' && tok[1] <= '9') {
			return atoi(tok);
		}

		return -1 * var_get(tok + 1);
	}

	if(tok[0] >= '0' && tok[0] <= '9') {
		return atoi(tok);
    	}

	return var_get(tok);
}

void print_int(int n, FILE* stream) {
    char buf[12];
    uint8_t i = 0;
    if(n < 0) {
        fputc('-', stream);
        n = -n;
    }
    if(n == 0) {
        fputc('0', stream);
        return;
    }
    while(n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while(i > 0) {
        fputc(buf[--i], stream);
    }
}

void throw_error(int line_num, char* error_msg) {
	fputs("line ", stderr);
	print_int(line_num, stderr);
	fputs(": ", stderr);
	fputs(error_msg, stderr);
	fputs("\n", stderr);
}

void throw_immediate_error(char* error_msg) {
	fputs("runtime error: ", stderr);
	fputs(error_msg, stderr);
	fputs("\n", stderr);
	
	exit(1);
}

uint8_t execute_line(char* line, int line_num) {
	char* op = strtok(line, " ");
	char* dest = strtok(NULL, " ");
	char* src1 = strtok(NULL, " ");
	char* src2 = strtok(NULL, " ");

	if(!op) return OK;
	
	if(strcmp(op, "#") == 0) return OK;

	if(strcmp(op, "add") == 0) {
		if(strcmp(dest, "zero") != 0) {
			int result = var_resolve(src1) + var_resolve(src2);
			var_set(dest, result);
		}
	}

	else if(strcmp(op, "mul") == 0) {
		if(strcmp(dest, "zero") != 0) {
			int result = var_resolve(src1) * var_resolve(src2);
			var_set(dest, result);
		}
	}

	else if(strcmp(op, "div") == 0) {
		if(strcmp(dest, "zero") != 0) {
			int src1_resolved = var_resolve(src1);
			int src2_resolved = var_resolve(src2);
			if(src2_resolved == 0) {
				throw_error(line_num, "division by zero");
				return ERR;	
			}
			int result = src1_resolved / src2_resolved;
			var_set(dest, result);
		}
	}

	else if(strcmp(op, "movstr") == 0) {
		FILE* stream = NULL;
		if(strcmp(dest, "stdout") == 0) {
			stream = stdout;
		}
		else if(strcmp(dest, "stderr") == 0) {
			stream = stderr;
		}
		else {
			throw_error(line_num, "not a valid output stream!");
			return ERR;
		}
		fputs(src1, stream);

		if(src2) {
			if(strcmp(src2, "br") == 0) {
				fputs("\n", stream);
			}
		}
	}

	else if(strcmp(op, "mov") == 0) {
		if(strcmp(src1, "stdin") == 0) {
			char buf[16];
			fgets(buf, sizeof(buf), stdin);

			buf[strcspn(buf, "\n")] = '\0';

			var_set(dest, var_resolve(buf));
			return OK;
		}

		FILE* stream = NULL;
		if(strcmp(dest, "stdout") == 0) {
			stream = stdout;
		}
		else if(strcmp(dest, "stderr") == 0) {
			stream = stderr;
		}
		else if(strcmp(dest, "stdin") == 0) {
			throw_error(line_num, "stdin is not a valid destination!");
			return ERR;
		}
		else {
			var_set(dest, var_resolve(src1));
		}

		if(stream) {
			print_int(var_resolve(src1), stream);
			fputs("\n", stream);
		}
	}

	else {
		throw_error(line_num, "unrecognized instruction!");
		return ERR;
	}

	return OK;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fputs("usage: planck <filename>", stderr);
		return ERR;
	}
	
	const char* path = argv[1];

	FILE* file = fopen(path, "r");

	if(file == NULL) {
		fputs(path, stderr);
		fputs(": file does not exist!", stderr);
		return 1;
	}

	char line[MAX_CMD_LEN];
	int line_num = 0;
	while((fgets(line, sizeof(line), file)) != NULL) {
		line_num++;
		line[strcspn(line, "\n")] = '\0';
		uint8_t status = execute_line(line, line_num);

		if(status == ERR) {
			return ERR;	
		}
	}

	return OK;
}




