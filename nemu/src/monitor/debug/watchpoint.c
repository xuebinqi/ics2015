#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	WP* tmp = free_;
	free_ = free_->next;

	return tmp;
}

void set_wp(char *expression){
	WP* tmp = new_wp();
	strcpy(tmp->expression, expression);
	bool success = true;
	tmp->value = expr(expression, &success);

	tmp->next = head;
	head = tmp;
}

void free_wp(int index){
	WP *wp = head;
	while(wp != NULL && wp->NO != index)
		wp = wp->next;
	if(wp == NULL){
		printf("Wrong watchpoint\n");
		return ;
	}
		
	if(wp == head)
		head = wp->next;
	else{
		WP *temp = head;
		while(temp->next != wp)
		temp = temp->next;
		temp->next = wp->next;
	}
	wp->next = free_;
	free_ = wp;
}

void print_wp(){
	WP *tmp = head;
	while(tmp!=NULL){
		printf("NO: %d\t%s = %d\n", tmp->NO, tmp->expression, tmp->value);
		tmp = tmp->next;
	}
}

int check_wp(){
	WP *tmp = head;
	while(tmp!=NULL){
		bool success = true;
		int num = expr(tmp->expression, &success);
		if(num!=tmp->value){
			return tmp->NO;
		}
		tmp = tmp->next;
	}
	return -1;
}
