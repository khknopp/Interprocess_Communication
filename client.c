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
#include "request.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the message queue (whose name is provided in the
    //    arguments)
    //  * repeatingly:
    //      - get the next job request 
    //      - send the request to the Req message queue
    //    until there are no more requests to send
    //  * close the message queue
    mqd_t queue;
    int jobID, data, serviceID;

    // Read the name of the queue from the arguments, with parameter name "queue"
    char* queue_name = argv[1];

    // Open the queue with the name provided in the arguments
    queue = mq_open(queue_name, O_WRONLY);

    // Check if the queue was opened successfully
    if (queue == -1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    while(getNextRequest(&jobID, &data, &serviceID) == 0)
    {
        // Create a new request message
        MQ_REQUEST_MESSAGE request;
        request.Request_ID = jobID;
        request.Service_ID = serviceID;
        request.data = data;

        // Send the request to the Req message queue
        if (mq_send(queue, (char*) &request, sizeof(request), 0) == -1)
        {
            perror("mq_send Client");
            exit(EXIT_FAILURE);
        }
    }

    // Close the message queue
    if (mq_close(queue) == -1)
    {
        perror("mq_close Client");
        exit(EXIT_FAILURE);
    }

    return(0);
}
