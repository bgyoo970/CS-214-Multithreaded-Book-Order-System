#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "database.c"
#include "bookorder.c"
#include "queue.c"

/* Global vars */
Queue *MasterQueue;
char **categoryList;
int condFlag = 0; /*is producer thread done?*/
database *DB;

void *producerThread(void *argv) {
	FILE *fp = fopen((char*)argv, "r");
	char line[1024];
	char *title, *category, *temp;
	int id, flag, boolean; float cost;
	/* tokenize the orders.txt */
	while(!feof(fp)){
		if(fgets(line, 1024, fp)){
			flag = 0;
			// title, cost, id, category
			for((temp = strtok(line, "|\n\r")); temp != NULL; temp = strtok(NULL, "\n\r|")){
				if(flag == 0){
					title = (char*)malloc(strlen(temp)+1);
					strcpy(title, temp);
				}
				else if(flag == 1) {
					cost = atof(temp);
				}
				else if(flag == 2) {
					id = atoi(temp);
				}
				else if(flag == 3){
					category = (char*)malloc(strlen(temp)+1);
					strcpy(category, temp);
				}
				if(flag > 3) continue;
				flag++;
			}

			
			/* check if the category is an existing category. if 0, then fail. */
			boolean = hasCategory(categoryList, category);
			if (boolean == 0) {
				printf("The category: %s does not exist within the given category list. The current order cannot be processed. \n" , category);
				continue;
			}
			
			// Creates an order, prints the information.
			order *newOrder = CreateBookOrder(title, cost, id, category);
			

				// Lock the mutex going in and out of the queue data structure
				pthread_mutex_lock(&MasterQueue->mutex);
				Enqueue(MasterQueue, (void *) newOrder);
				printf("Producer resumes. Queues orders ready for processing\n");
				pthread_mutex_unlock(&MasterQueue->mutex);
			
				// Send out signal to the consumer threads
				pthread_cond_signal(&MasterQueue->cond);
				printf("Producer waits. \n");

			
		}
	}
	
	condFlag = 1;
	pthread_cond_broadcast(&MasterQueue->cond); /**/
	fclose(fp);
	return NULL;
}

void *consumerThread(void *category) {
	char *temp = NULL, *currcategory = NULL;
	order *currorder; order *peek;
	customer *currcustomer;
	report *report;
	int bool;
	
	// Obtains the current category for the current consumer thread.
	temp = (char *) category;
	currcategory = (char *) malloc(strlen(temp)+1);
	strcpy(currcategory, temp);

	while ( !condFlag || (isEmpty(MasterQueue) == 0) ) {
		printf("Consumer resumes. Queues have orders ready for processing. \n");
		pthread_mutex_lock(&MasterQueue->mutex);

		if( !condFlag && (isEmpty(MasterQueue) == 1)) {
			pthread_cond_signal(&MasterQueue->cond);
			pthread_cond_broadcast(&MasterQueue->cond);
			printf("Consumer waits because queues are empty. \n");
			pthread_cond_wait(&MasterQueue->cond, &MasterQueue->mutex);
		}
		/*No more orders to process. Exit thread.*/
		if ( condFlag && (isEmpty(MasterQueue) == 1) ) {
			pthread_mutex_unlock(&MasterQueue->mutex);
			return NULL;
		}
		else if (isEmpty(MasterQueue) == 1) {
			//printf("Consumer waits because queues are empty. \n");
			pthread_mutex_unlock(&MasterQueue->mutex);
			sched_yield();
			continue;
		}
		else if(isEmpty(MasterQueue) == 0){
		/* Process the current book order. */
		
		/* 
		* 1. MATCH CATEGORIES
		* First compare if the categories match.
		*/
		currorder = (order *) QueuePeek(MasterQueue);
		/* No such category */
		if (strcmp(currorder->category, currcategory) != 0) {
			pthread_mutex_unlock(&MasterQueue->mutex);
			sched_yield();
		}
		else if (strcmp(currorder->category, currcategory) == 0) {
			currorder = (order *) Dequeue(MasterQueue);
			/* 
			* 2. MATCH CUSTOMER IDS
			* See if the database has a corresponding customer id
			* hasCustomer is a success if 1.
			* Otherwise do nothing if there is no such customer id
			*/
			bool = hasCustomer(DB, currorder->id);
			if (bool == 1) {
				currcustomer = getCustomer(DB, currorder->id);
				/*
				* 3. CHECK CREDIT
				* See if the customer has enough credit to purchase the book.
				* Print a confirmation/rejection order depending on the credit.
				* Store these orders into corresponding queues to print in the FinalReport
				*/
				if (currcustomer->credit >= currorder->cost) {
					printf("\nA successful book order has been made, printing order confirmation. \n"
					"Title: %s\n"
					"Price: %.2f\n"
					"Customer Name: %s\n"
					"Shipping Information: %s, %s %s \n\n" , currorder->title, currorder->cost, currcustomer->name, currcustomer->address, currcustomer->state, currcustomer->zipcode);
					//Debit the credit. store the remaining balance within the report.
					currcustomer->credit = currcustomer->credit - currorder->cost;
					report = CreateReport(currorder->title, currorder->cost, currcustomer->credit);
					Enqueue(currcustomer->successfulOrders, report);
				}
				else if(currcustomer->credit < currorder->cost) {
					printf("\nThe book order was unsuccessful. Insufficient funds. \n"
					"Customer Name: %s\n"
					"Current Remaining Credit: %.2f\n"
					"Title: %s\n"
					"Price: %.2f\n\n", currcustomer->name, currcustomer->credit, currorder->title, currorder->cost);
					report = CreateReport(currorder->title, currorder->cost, currcustomer->credit);	
					Enqueue(currcustomer->unsuccessfulOrders, report);
				}
			}
			else if (bool == 0) {
				printf("No such customer exists with the given customer id: %s \n" , currorder->id);
			}
			pthread_mutex_unlock(&MasterQueue->mutex);

			/*free currorder*/
			if(currorder!=NULL){
				free(currorder->title);
				free(currorder->category);
				free(currorder);	
			}
		}
		
		}
	}
	
	free(currcategory);
	return NULL;
}

void *printFinalReport(database *DB) {
	float revenue = 0.0;
	customer *customer;
	report *report;
	int i;
	for (i = 0; i < 1024; i++) {
		if (DB->datahash[i] == NULL) continue;
		customer = DB->datahash[i];
		
		// Begin the Final Report
		printf("=== BEGIN CUSTOMER INFO === \n");
		printf("### BALANCE ###\n");
		printf("Customer name: %s \n" , customer->name);
		printf("Customer ID number: %d \n" , customer->id);
		printf("Remaining credit after all purchases (a dollar amount): %.2f\n", customer->credit);
	
		printf("### SUCCESSFUL ORDERS ###\n");
		while(!isEmpty(customer->successfulOrders)) {
			report = Dequeue(customer->successfulOrders);
			printf("%s|%.2f|%.2f\n" , report->title, report->cost, report->remainingCredit);
			revenue = revenue + report->cost;
		}
		
		printf("### REJECTED ORDERS ###\n");
		while(!isEmpty(customer->unsuccessfulOrders)) {
			report = Dequeue(customer->unsuccessfulOrders);
			printf("%s|%.2f\n" , report->title, report->cost);
		}
		
		
		printf("=== END CUSTOMER INFO === \n\n");
	}
	
	printf("Total revenue gained: %.2f\n" , revenue);
	return;
}

int main(int argc, char** argv) {

	if (argc != 4) {
		printf("ERROR: Incorrect number of arguments. \n");
		printf("Arg 1: the name of the database input file \n");
		printf("Arg 2: the name of the book order input file \n");
		printf("Arg 3: the name of a file containing category names \n");
		return 0;	
	}
	
	// Note to self: May need to free stuff later.
	void *temp;
	categoryList = (char **) calloc(1024, sizeof(char *));
	DB = (database *) malloc(sizeof(database));
	MasterQueue = CreateQueue();
	PopulateDB(DB, argv[1]);
	categoryList = CreateCategories(argv[3]);
	
	// Thread IDs
	pthread_t threadids[getNumCategories(argv[3]) + 1];
	
	// Create the producer thread
	pthread_create(&threadids[0], NULL, producerThread, (void *) argv[2]);
	
	// Create a consumer thread for each category. Skip over threadids[0] since a producer occupies it.
	int i;
	for (i = 0; i < getNumCategories(argv[3]); i++) {
		pthread_create(&threadids[i+1], NULL, consumerThread, categoryList[i]);	
	}
	
	
	for (i = 0; i < getNumCategories(argv[3]) + 1; i++) {
		pthread_join(threadids[i], &temp);
	}
	
	printFinalReport(DB);


	free(categoryList);
	free(DB);
	return 0;
}






