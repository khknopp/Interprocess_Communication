/* 
 * Operating Systems  (2INCO)  Practical Assignment
 * Interprocess Communication
 *
 * Kajetan Knopp (1674404)
 * Kasra Gheytuli (1753665)
 *
 * Grading:
 * Your work will be evaluated based on the following criteria:
 * - Satisfaction of all the specifications
 * - Correctness of the program
 * - Coding style
 * - Report quality
 * - Deadlock analysis
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>      // for perror()
#include <unistd.h>     // for getpid()
#include <mqueue.h>     // for mq-stuff
#include <time.h>       // for time()

#include "messages.h"
#include "service1.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the
    //    arguments)
    //  * repeatedly:
    //      - read from the S1 message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do the job 
    //      - write the results to the Rsp message queue
    //    until there are no more tasks to do
    //  * close the message queues

    // Get the name of the worker queue, with the name provided under argument read_queue
    char* read_queue = argv[1];

    // Get the name of the response queue, with the name provided under argument write_queue
    char* write_queue = argv[3];

    // Open the read queue
    mqd_t worker = mq_open(read_queue, O_RDONLY);

    // Check if the read queue was opened successfully
    if (worker == -1)
    {
        perror("mq_open Worker 1");
        exit(EXIT_FAILURE);
    }

    // Open the write queue
    mqd_t response = mq_open(write_queue, O_WRONLY);

    // Check if the write queue was opened successfully
    if (response == -1)
    {
        perror("mq_open Response queue for worker 1");
        exit(EXIT_FAILURE);
    }

    // Create a new service 1 message
    MQ_SERVICE_1_MESSAGE s1;

    // While we can read from the read queue
    while (mq_receive(worker, (char*) &s1, sizeof(s1), NULL) != -1)
    {
        // Create a response message
        MQ_RESPONSE_MESSAGE response_message;

        // Check if the message is the final message
        if (s1.Request_ID == -1 && s1.data == -1)
        {
            response_message.Request_ID = -1;
            response_message.result = -1;
            // Send the message to the response queue
            if (mq_send(response, (char*) &response_message, sizeof(response_message), 0) == -1)
            {
                perror("mq_send Writing final response from worker 1");
                exit(EXIT_FAILURE);
            }

            // Break out of the loop
            break;
        }

        // Do the job
        response_message.Request_ID = s1.Request_ID;
        response_message.result = service(s1.data);

        // Sleep as defined in the task
        rsleep(10000);

        // Write the results to the response queue
        if (mq_send(response, (char*) &response_message, sizeof(response_message), 0) == -1)
        {
            perror("mq_send Writing response from worker 1");
            exit(EXIT_FAILURE);
        }
    }

    // Close the read and write queues
    if (mq_close(worker) == -1)
    {
        perror("mq_close Worker 1");
        exit(EXIT_FAILURE);
    }
    if (mq_close(response) == -1)
    {
        perror("mq_close Response queue for worker 1");
        exit(EXIT_FAILURE);
    }

    return(0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}
