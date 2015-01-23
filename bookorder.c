#include "bookorder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


order *CreateBookOrder(char *title, float cost, int id, char *category) {
	order *newOrder = (order *)malloc(sizeof(order));
	newOrder->title = title;
	newOrder->cost = cost;
	newOrder->id = id;
	newOrder->category = category;

	return newOrder;
}

report *CreateReport(char *title, float cost, float remainingCredit) {
	report *report = malloc(sizeof(report));
	report->title = (char *) malloc(strlen(title)+1);
	strcpy(report->title, title);
	report->cost = cost;
	report->remainingCredit = remainingCredit;
	
	return report;
}

