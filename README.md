# CS-214-Multithreaded-Book-Order-System
###### Fall 2014 with Brian Russell

-Input instruction: 
-Argument 1 must be the name of the database textfile (e.g. database.txt)
-Argument 2 must be the name of the book order textfile (e.g. bookorder.txt)
-Argument 3 must be the name of the category textfile (e.g. category.txt)

The program first creates the database using an array as its data structure that holds a maximum
of 1024 customers. It will store each customer based on the customer ID and hash the ID into the
appropriate index. Then a single large queue is used to store the book orders. A thread ID array will be
created to keep track of how many threads need to be created from the number of categories given.

The producer thread parses the orders.txt file, locks the queue before storing each order into the
queue, unlocks it when its finished enqueuing, and then tells the consumer threads that there is
something ready to be processed. At the same time, the consumer threads will wait for the producer
threads to tell them that there are orders ready for processing. Then, the consumer threads will lock the
queue as it enters it, process the orders according to which specific category it is in, and store the
successful/rejected order into another queue to print in the final report. Then, when the book-order
queue is empty, the consumer threads will wait until there are orders ready for processing.

The only behavior that may be inconsistent was with the interleaving of book order creation and the
book order processing by the producer and consumer. The messages indicating when the
producer/consumer waits or resumes seems to vary from each time the program runs. At times, the
messages will alternate between the producer and consumer messages more consistently than at other
times. Some instances will run a long stream of consumer messages with not too many producer
messages. Despite the behavior of these print statements, the output remains consistently the same.

At the end, the program will print the final report by looking through the successful and
unsuccessful lists stored in each customer and print out the orders. As a result, the code will run in O(n)
time, as we must dequeue every element from the book-orders queue and the (un)successful order
queues from each customer.

All use of this code must comply with the [Rutgers University Code of Student Conduct](http://eden.rutgers.edu/%7Epmj34/media/AcademicIntegrity.pdf).
