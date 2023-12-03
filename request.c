/* 
 * Operating Systems (2INCO) Practical Assignment
 * Interprocess Communication
 *
 * Contains functions that are used by the clients
 *
 */

#include "request.h"

// Array of requests
const Request requests[] = { {1, 26, 1}, {2, 5, 2}, {3, 10, 2}, {5, 13, 1}, {4, 3, 1} };
// const Request requests[] = { {1, 26, 1}, {2, 5, 1}, {3, 10, 1}, {5, 13, 1}, {4, 3, 1},
// 							{6, 26, 1}, {9, 5, 1}, {14, 10, 1}, {15, 13, 1}, {20, 3, 1},
// 							{7, 26, 1}, {10, 5, 1}, {13, 10, 1}, {16, 13, 1}, {19, 3, 1},
// 							{8, 26, 1}, {11, 5, 1}, {12, 10, 1}, {17, 13, 1}, {18, 3, 1} };

// const Request requests[] = { {-1, -1, 1}, {-1, -1, 2}, {-1, -1, 1}, {-1, -1, 1}, {-1, -1, 1} };

// Places the information of the next request in the parameters sent by reference.
// Returns NO_REQ if there is no request to make.
// Returns NO_ERR otherwise.
int getNextRequest(int* jobID, int* data, int* serviceID) {
	static int i = 0;
	static int N_REQUESTS = sizeof(requests) / sizeof(Request);

	if (i >= N_REQUESTS) 
		return NO_REQ;

	*jobID = requests[i].job;
	*data = requests[i].data;
	*serviceID = requests[i].service;		
	++i;
	return NO_ERR;
		
}