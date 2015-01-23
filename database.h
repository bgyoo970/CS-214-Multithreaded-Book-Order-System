#ifndef DATABASE_H
#define DATABASE_H

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

typedef struct database database;
typedef struct customer customer;


/* Customer Node */
struct customer {
	char* name;
	int id;
	float credit;
	char* address;
	char* state;
	char* zipcode;
	Queue *successfulOrders;
	Queue *unsuccessfulOrders;
	
};

/* Database Hash */
struct database {
	customer *datahash[1024];
};

char** CreateCategories(char *argv);
int getNumCategories(char *argv);
int hasCategory(char** categoryList, char* category);

customer *CreateCustomer(char *name, int id, float credit, char* address, char* state, char* zipcode);
void PopulateDB(database *DB, char* database);
database *AddCustomer(database *database, customer *customer);
int hasCustomer(database *database, int id);
customer *getCustomer(database *database, int id);

#endif
