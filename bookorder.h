#ifndef BOOKORDER_H
#define BOOKORDER_H

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bookorder order;
typedef struct orderStatusReport report;


struct bookorder {
	char* title;
	float cost;
	int id;
	char* category;
};

struct orderStatusReport {
	char *title;
	float cost;
	float remainingCredit;
};


order *CreateBookOrder(char *title, float cost, int id, char *category);
report *CreateReport(char *title, float cost, float remainingCredit);



#endif
