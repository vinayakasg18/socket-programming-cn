/*
 *  Here is the starting point for your netster part.2 includes. Add the
 *  appropriate comment header as defined in the code formatting guidelines.
 */

#ifndef P2_H
#define P2_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

/* add function prototypes */
void file_server(char*, long, int, FILE*);
void file_client(char*, long, int, FILE*);

/*
int PROF_build_server_socket(char* iface, long port, int use_udp, struct addrinfo* addr);
int PROF_build_client_socket(char* host, long port, int use_udp, struct addrinfo* addr);
*/


#endif
