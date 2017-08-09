#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */

	//**************my code*****************
	char expression[32];
	int value;
	//**************************************

} WP;

#endif
