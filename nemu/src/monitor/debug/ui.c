#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
//***********my code**********************
void free_wp(int index);
void set_wp(char *expression);
void print_wp();
int check_wp();
//****************************************

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

//***********my add code**********************
static int cmd_si(char *args){
	int n = 1;
	if(args){
		n = atoi(args);	
	}
	cpu_exec(n);

	return 0;
}

static int cmd_info(char *args){
	char *str = strtok(NULL, "$");
	if (strlen(str) == 1) {
		if (str[0] == 'r') {
			int i = 0;
		
			for (i = 0; i < 7; i++){
				printf("%s\t0x%x\t%d\n", regsl[i], reg_l(i), reg_l(i));
			}

			for (i = 0; i < 7; i++){
				printf("%s\t0x%x\t%d\n", regsw[i], reg_w(i), reg_w(i));
			}

			for (i = 0; i < 7; i++){
				printf("%s\t0x%x\t%d\n", regsb[i], reg_b(i), reg_b(i));
			}
		}
		else if (str[0] == 'w'){
			print_wp();
		}
	}
	else {
		int i = 0;
		for(i = 0; i < 7; i++){
			if(strcmp(str, regsl[i]) == 0){
				printf("%s\t0x%x\t%d\n", str, reg_l(i), reg_l(i));
			}
		}
		for(i = 0; i < 7; i++){
			if(strcmp(str, regsw[i]) == 0){
				printf("%s\t0x%x\t%d\n", str, reg_w(i), reg_w(i));
			}
		}
		for(i = 0; i < 7; i++){
			if(strcmp(str, regsb[i]) == 0){
				printf("%s\t0x%x\t%d\n", str, reg_b(i), reg_b(i));
			}
		}
	}

	return 0;
}

static int cmd_p(char *args){
	bool flag = true;
	int num = expr(args, &flag);	
	if(flag == false){
		return 0;
	}
	printf("%d\n", num);

	return 0;
}

static int cmd_x(char *args){
	bool *flag = false;
	char *num_str = strtok(NULL, " ");
	char *expr_str = strtok(NULL, " ");
	int num = atoi(num_str);
	int addr = expr(expr_str, flag);

	int i = 0;
	for(i = 0; i < num; i++){
		printf("0x%x\t", swaddr_read(addr + i, 1));
	}
	printf("\n");

	return 0;
}

static int cmd_w(char *args){
	set_wp(args);

	return 0;
}

static int cmd_d(char *args){
	int index = atoi(args);
	free_wp(index);

	return 0;
}

//****************************************************

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
//********************my add code******************
	{ "si", "Execute step by step", cmd_si},
	{ "info", "Print info of registers and watchpoint", cmd_info},
	{ "p", "Compute the value of an expression", cmd_p},
	{ "x", "Read content of the given address in memory", cmd_x},
	{ "w", "Set watchpoint", cmd_w},
	{ "d", "Delete watchpoint", cmd_d},
//***************************************************

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
