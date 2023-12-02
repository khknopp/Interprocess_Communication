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

#ifndef MESSAGES_H
#define MESSAGES_H

// Message structure definitions for each of the queues:

// Defintion of the Req_queue message structure
typedef struct
{
    int                     Request_ID;
    int                     Service_ID;
    int                     data;
} MQ_REQUEST_MESSAGE;

// Defintion of the Rsp_queue message structure
typedef struct
{
    int                     Request_ID;
    int                     data;
} MQ_RESPONSE_MESSAGE;

// Defintion of the S1_queue message structure
typedef struct
{
    int                     Request_ID;
    int                     data;
} MQ_SERVICE_1_MESSAGE;

// Defintion of the S2_queue message structure
typedef struct
{
    int                     Request_ID;
    int                     data;
} MQ_SERVICE_2_MESSAGE;

#endif