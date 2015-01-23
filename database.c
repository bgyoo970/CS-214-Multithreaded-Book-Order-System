#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

/* parses the categories.txt and stores them into an array */
char** CreateCategories(char *argv) {
	FILE *fp = fopen(argv, "r");
	char line[1024];
	char* category = NULL;
	char **categoryList = (char **) calloc(1024, sizeof(char *));
	int ct = 0;
	char *temp = NULL;
	
	while(!feof(fp)){
		if(fgets(line, 1024, fp)){
			temp = strtok(line, "\n\r");
			category = malloc(strlen(temp)+1);
			strcpy(category, temp);
			//str
			categoryList[ct] = category;
			ct++;
		}
	}
	fclose(fp);
	return categoryList;
}

/* obtains the number of categories within the textfile */
int getNumCategories(char *argv) {
	FILE *fp = fopen(argv, "r");
	char line[1024];
	int ct = 0;
	while(!feof(fp)){
		if(fgets(line, 1024, fp)){
			ct++;
		}
	}
	fclose(fp);
	return ct;
}

/* Checks if the incoming category is within the categories.txt. 1 = TRUE; 0 = FALSE */
int hasCategory(char** categoryList, char* category) {
	int i;
	for(i = 0; i < 1024 && categoryList[i] != NULL; i++) {
		if (strstr(categoryList[i], category) != NULL) {
				return 1;
		}
	}
	return 0;
}

void PopulateDB(database *DB, char* database){

	FILE *fp = fopen(database, "r");
	char line[1024];
	char *name, *temp, *address, *state, *zipcode;
	int id, flag;
	float credit;
	
	/* Null Check */
	if(fp == NULL){
		printf("Could not open file\n");
		exit(EXIT_FAILURE);	
	}
	
	/* tokenize the database.txt */
	while(!feof(fp)){
		if(fgets(line, 1024, fp)){
			flag = 0;
			
			for((temp = strtok(line, "\"|\n\r")); temp != NULL; temp = strtok(NULL, "\"|\n\r")){
				if(flag == 0){
					name = (char*)malloc(strlen(temp)+1);
					strcpy(name, temp);
				}
				else if(flag == 1) {
					id = atoi(temp);
				}
				else if(flag == 2) {
					credit = atof(temp);
				}
				else if(flag == 3)
				{
					address = (char*)malloc(strlen(temp)+1);
					strcpy(address, temp);
				}
				else if(flag == 4) {
					state = (char*)malloc(strlen(temp)+1);
					strcpy(state, temp);
				}
				else if(flag == 5) {
					zipcode = (char*)malloc(strlen(temp)+1);
					strcpy(zipcode, temp);
				}
				
				if(flag > 5) continue;
				flag++;
			}
			
			/* store the tokens into a Customer Node */
			customer *newCustomer = CreateCustomer(name, id, credit, address, state, zipcode);
			AddCustomer(DB, newCustomer);
		}
	}
	
	fclose(fp);
	//free(name); free(address); free(state); free(zipcode);
}


/* Creates a customer node */
customer *CreateCustomer(char *name, int id, float credit, char* address, char* state, char* zipcode) {
	customer *newCustomer = (customer *) malloc(sizeof(customer));
	newCustomer->name = name;
	newCustomer->id = id;
	newCustomer->credit = credit;
	newCustomer->address = address;
	newCustomer->state = state;
	newCustomer->zipcode = zipcode;
	newCustomer->successfulOrders = CreateQueue();
	newCustomer->unsuccessfulOrders = CreateQueue();
	
	return newCustomer;
}

/* 
* Adds customer node to DB hash
* the database will have a limit of 1024 customers 
*/
database *AddCustomer(database *database, customer *customer) {
	database->datahash[customer->id] = customer;
}

int hasCustomer(database *database, int id) {
	if ( id > 0 && id < 1024 )
		return 1;
	else {
		printf("The customer id, %d does not exist within the database."
		"The range of the database from 0 to 1024. \n" , id);
		return 0;
	}
	return 0;
}

customer *getCustomer(database *database, int id) {
	if (database)
		return database->datahash[id];
	else
		printf("Cannot getCustomer. Not a valid input database. \n");
		
	return NULL;
}
