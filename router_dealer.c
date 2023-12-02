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
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>    // for execlp
#include <mqueue.h>    // for mq


#include "settings.h"  
#include "messages.h"

char client2dealer_name[20] = "/client2dealer";
char dealer2worker1_name[20] = "/dealer2worker1";
char dealer2worker2_name[20] = "/dealer2worker2";
char worker2dealer_name[20] = "/worker2dealer";

int main (int argc, char * argv[])
{
  if (argc != 1)
  {
    fprintf (stderr, "%s: invalid arguments\n", argv[0]);
  }
  
  // TODO:
    //  * create the message queues (see message_queue_test() in
    //    interprocess_basic.c)
    //  * create the child processes (see process_test() and
    //    message_queue_test())
    //  * read requests from the Req queue and transfer them to the workers
    //    with the Sx queues
    //  * read answers from workers in the Rep queue and print them
    //  * wait until the client has been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues
    // contain your goup number (to ensure uniqueness during testing)


  // Defining the number of workers for S1 and S2
  int S1_workers = N_SERV1;
  int S2_workers = N_SERV2;

  // Creating the message queues
  mqd_t Req_queue_KasraKai_24;
  mqd_t Rsp_queue_KasraKai_24;
  mqd_t S1_queue_KasraKai_24;
  mqd_t S2_queue_KasraKai_24;

  // Creating the messages
  MQ_REQUEST_MESSAGE  req;
  MQ_RESPONSE_MESSAGE rsp;
  MQ_SERVICE_1_MESSAGE s1;
  MQ_SERVICE_2_MESSAGE s2;

  
  // Defining the attributes
  // struct mq_attr      attr;
  // attr.mq_maxmsg = MQ_MAX_MESSAGES;

  // Creating the processes
  pid_t processID;

  // Defining the PID's of the processes
  pid_t clientPID;
  pid_t service1PID;
  pid_t service2PID;

  /* 
    Creating the client process;
      client: processID = 0
      router: processID = client
  */
  processID = fork();

  // -> router process
  if(processID > 0) {
    // We know that processID for the router currently holds the client processID, so we save it
    clientPID = processID;

    /* 
      Creating the Service 1 process;
        Service 1: processID = 0
        router: processID = service_1
    */ 
    processID = fork();

    // -> service 1 process
    if(processID == 0){
      // Create the correct number of workers for S1
      for(int i = 0; i < S1_workers; i++){
        // Create new process
        processID = fork();
        // Exit the loop if the process is a child; only fork from the initial worker 1 process
        if(processID == 0){
          break;
        }
      }

      // -> worker (doing jobs)
      if(processID == 0){
        // TODO:
      }
      // -> parent worker
      else {
        // Wait for all of the workers to be finished
        while(wait(NULL) > 0);
      }
    }
    // -> router process
    else {
      // We know that processID for the router currently holds the Service 1 processID, so we save it
      service1PID = processID;

      /* 
        Creating the Service 2 process;
          Service 2: processID = 0
          router: processID = service_2
      */ 
      processID = fork();

      // -> service 2 process
      if(processID == 0){
        // Create the correct number of workers for S2
        for(int i = 0; i < S2_workers; i++){
          // Create new process
          processID = fork();
          // Exit the loop if the process is a child; only fork from the initial worker 1 process
          if(processID == 0){
            break;
          }
        }
        
        // -> worker (doing jobs)
        if(processID == 0){
          // TODO:
        }
        // -> parent worker
        else {
          // Wait for all of the workers to be finished
          while(wait(NULL) > 0);
        }
      }
      // -> router process
      else {
        // We know that processID for the router currently holds the Service 2 processID, so we save it
        service2PID = processID;
        // TODO:


        // Release resources for the children processes
        waitpid(clientPID, NULL, 0);
        waitpid(service1PID, NULL, 0);
        waitpid(service2PID, NULL, 0);
      }
    }
  } 
  // -> client process
  else {
    // TODO:
  }


  return (0);
}
