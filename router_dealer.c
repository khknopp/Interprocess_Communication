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

char client2dealer_name[22] = "/Req_queue_KasraKai_24";
char dealer2worker1_name[22] = "/S1_queue_KasraKai_24";
char dealer2worker2_name[22] = "/S2_queue_KasraKai_24";
char worker2dealer_name[23] = "/Rsp_queue_KasraKai_24";

void throwError(bool condition, char* message){
  // Throw error if the condition is true
  if(condition){
    perror(message);
    exit(EXIT_FAILURE);
  }
}

void clientProcess(void) {
  // Execute the client
  execlp ("./client", "queue", client2dealer_name, NULL);

  // Throw error if the following line is launched
  throwError(true, "execlp client");
}

void serviceProcess(pid_t processID, int workers, char* read_queue, char* worker_name) {
  // Create the correct number of workers for S1
  for(int i = 0; i < workers; i++){
    // Create new process
    processID = fork();
    // Exit the loop if the process is a child; only fork from the initial worker 1 process
    if(processID == 0){
      break;
    }
  }

  // -> worker (doing jobs)
  if(processID == 0 && workers > 0){
    execlp (worker_name, "read_queue", read_queue, "write_queue", worker2dealer_name, NULL);

    // Throw error if the following line is launched
    throwError(true, "execlp worker");
  }

  // -> parent worker
  // Wait for all of the workers to be finished
  while(wait(NULL) > 0);
  return 0;
}

void dealerProcess(mqd_t Req_queue, mqd_t S1_queue, mqd_t S2_queue, mqd_t Rsp_queue){
  // Create the request message
  MQ_REQUEST_MESSAGE  req;

  // Using the request queue, send the requests to the workers
  while(mq_receive(Req_queue, (char*) &req, sizeof(req), NULL) != -1){
    // Check if the request is for Service 1
    if(req.Service_ID == 1){
      MQ_SERVICE_1_MESSAGE s1;
      // Create a new Service 1 message
      s1.Request_ID = req.Request_ID;
      s1.data = req.data;

      // Send the request to the Service 1 queue
      if(mq_send(S1_queue, (char*) &s1, sizeof(s1), 0) == -1){
        perror("mq_send Service 1");
        exit(EXIT_FAILURE);
      }
    }
    
    // Check if the request is for Service 2
    else if(req.Service_ID == 2){
      // Create a new Service 2 message
      MQ_SERVICE_2_MESSAGE s2;
      s2.Request_ID = req.Request_ID;
      s2.data = req.data;

      // Send the request to the Service 2 queue
      if(mq_send(S2_queue, (char*) &s2, sizeof(s2), 0) == -1){
        perror("mq_send Service 2");
        exit(EXIT_FAILURE);
      }
    }

    // Else final message has been sent, so we break out of the loop
    else if(req.Service_ID == -1 && req.Request_ID == -1 && req.data == -1){
      // Send a final message to Worker 1
      MQ_SERVICE_1_MESSAGE s1;
      s1.Request_ID = -1;
      s1.data = -1;

      // Send the request to the Service 1 queue
      for(int i = 0; i < N_SERV1; i++){
        if(mq_send(S1_queue, (char*) &s1, sizeof(s1), 0) == -1){
          perror("mq_send Service 1");
          exit(EXIT_FAILURE);
        }
      }
      
      // Send a final message to Worker 2
      MQ_SERVICE_2_MESSAGE s2;
      s2.Request_ID = -1;
      s2.data = -1;

      // Send the request to the Service 2 queue
      for(int i = 0; i < N_SERV2; i++){
        if(mq_send(S2_queue, (char*) &s2, sizeof(s2), 0) == -1){
          perror("mq_send Service 2");
          exit(EXIT_FAILURE);
        }
      }
      break;
    }
  }
  return 0;
}

void routerProcess(pid_t clientPID, pid_t service1PID, pid_t service2PID, pid_t dealerPID, mqd_t Req_queue, mqd_t Rsp_queue, mqd_t S1_queue, mqd_t S2_queue){
  // Create the response message
  MQ_RESPONSE_MESSAGE rsp;
  
  // Variables to count the number of final messages received from the workers
  int final_message_s1 = 0;
  int final_message_s2 = 0;

  // Using the response queue, print the responses from the workers
  while(mq_receive(Rsp_queue, (char*) &rsp, sizeof(rsp), NULL) != -1){
    // Check if the message is the final message for each of the workers
    if(rsp.Request_ID == -1 && rsp.result == -1){
      final_message_s1++;
    } else if(rsp.Request_ID == -2 && rsp.result == -2) {
      final_message_s2++;
    } else{
      printf("%d -> %d\n", rsp.Request_ID, rsp.result);
    }

    // Finish execution in case bpoth workers sent a final message
    if(final_message_s1 >= N_SERV1 && final_message_s2 >= N_SERV2){
      break;
    }
  }
  // Wait for the dealer process to be finished
  waitpid(dealerPID, NULL, 0);

  // Release resources for the children processes
  waitpid(clientPID, NULL, 0);
  waitpid(service1PID, NULL, 0);
  waitpid(service2PID, NULL, 0);

  // Close the message queues
  mq_close(Req_queue);
  mq_close(Rsp_queue);
  mq_close(S1_queue);
  mq_close(S2_queue);

  // Unlink the message queues
  mq_unlink(client2dealer_name);
  mq_unlink(worker2dealer_name);
  mq_unlink(dealer2worker1_name);
  mq_unlink(dealer2worker2_name);
}

void createProcesses(mqd_t Req_queue, mqd_t Rsp_queue, mqd_t S1_queue, mqd_t S2_queue) {
  // Creating the process identifier
  pid_t processID;

  // Defining the PID's of the processes
  pid_t clientPID;
  pid_t service1PID;
  pid_t service2PID;
  pid_t dealerPID;


  /* 
    Creating the client process;
      client: processID = 0
      router: processID = client
  */
  processID = fork();

  // Throw error if creating the client process failed
  throwError(processID < 0, "fork client");

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

    // Throw error if creating the Service 1 process failed
    throwError(processID < 0, "fork service 1");

    // -> service 1 process
    if(processID == 0){
      // Execute the Service 1 process
      serviceProcess(processID, N_SERV1, dealer2worker1_name, "./worker_s1");
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

      // Throw error if creating the Service 2 process failed
      throwError(processID < 0, "fork service 2");

      // -> service 2 process
      if(processID == 0){
        // Execute the Service 2 process
        serviceProcess(processID, N_SERV2, dealer2worker2_name, "./worker_s2");
      }
      // -> router process
      else {
        // We know that processID for the router currently holds the Service 2 processID, so we save it
        service2PID = processID;

        // Final fork to create the dealer process
        /*
          Creating the dealer process;
            dealer: processID = 0
            router: processID = dealer
        */
        processID = fork();

        // Throw error if creating the dealer process failed
        throwError(processID < 0, "fork dealer");

        // -> dealer process
        if(processID == 0){
          // Execute the dealer process
          dealerProcess(Req_queue, S1_queue, S2_queue, Rsp_queue);
        }
        // -> router process
        else{
          // We know that processID for the router currently holds the dealer processID, so we save it
          dealerPID = processID;

          // Execute the router process
          routerProcess(clientPID, service1PID, service2PID, dealerPID, Req_queue, Rsp_queue, S1_queue, S2_queue);
        }
      }
    }
  } 
  // -> client process
  else {
    // Execute client process
    clientProcess();
  }
}


int main (int argc, char * argv[])
{
  if (argc != 1)
  {
    fprintf (stderr, "%s: invalid arguments\n", argv[0]);
  }

  // Defining the attributes
  struct mq_attr      attr;
  attr.mq_maxmsg = MQ_MAX_MESSAGES;

  // Creating the request queue
  attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
  mqd_t Req_queue = mq_open(client2dealer_name, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

  // Creating the response queue
  attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
  mqd_t Rsp_queue = mq_open(worker2dealer_name, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

  // Creating the Service 1 queue
  attr.mq_msgsize = sizeof (MQ_SERVICE_1_MESSAGE);
  mqd_t S1_queue = mq_open(dealer2worker1_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  // Creating the Service 2 queue
  attr.mq_msgsize = sizeof (MQ_SERVICE_2_MESSAGE);
  mqd_t S2_queue = mq_open(dealer2worker2_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  // Throw error if creating the message queues failed
  throwError(Req_queue < 0, "mq_open Req_queue");
  throwError(Rsp_queue < 0, "mq_open Rsp_queue");
  throwError(S1_queue < 0, "mq_open S1_queue");
  throwError(S2_queue < 0, "mq_open S2_queue");

  // Execute the code for the different actors
  createProcesses(Req_queue, Rsp_queue, S1_queue, S2_queue);

  return (0);
}
