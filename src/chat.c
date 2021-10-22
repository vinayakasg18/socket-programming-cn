// /**************************************************************/
// /* Client/Server application - VINAYAKA GADAG (vgadag)
// /* CREATED: 09/27/2021
// /*
// /* Fun with sockets
// /*
// /* Reference: https://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf
// /* Reference: https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
// /* Reference: Multi Threading: https://www.youtube.com/watch?v=qPhP86HIXgg
// /**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include "chat.h"
#include <pthread.h>

/*
 *  Here is the starting point for your netster part.1 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */

// Multi threading data
typedef struct td
{
  pthread_t *tid;
  char *cli_ip;
  struct sockaddr_in cli_addr;
  int sock_fd;
} td;

void *chat_messages(void *arg);

void chat_server(char *iface, long port, int use_udp)
{
  // Server
  int server_socketdesc, conn_count;
  conn_count = 0;
  struct sockaddr_in server_addr;
  // char message_buffer[256];

  // Clean up the memory area by given bytes
  // bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if (use_udp == 0)
  {
    // Passive socket creation
    server_socketdesc = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socketdesc < 0)
    {
      perror("socket() failed");
      exit(EXIT_FAILURE);
    }

    // Assign address to socket
    int status = bind(server_socketdesc, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (status < 0)
    {
      perror("Bind failed");
      exit(EXIT_FAILURE);
    }
    // Set socket to listen
    listen(server_socketdesc, 9);
    while (1)
    {
      struct sockaddr_in cli_addr;
      int client_conn_fd;
      socklen_t addr_len = sizeof(cli_addr);
      client_conn_fd = accept(server_socketdesc, (struct sockaddr *)&cli_addr, &addr_len);
      if (client_conn_fd < 0)
        perror("Couldn't accept the connection");

      char cli_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, sizeof(cli_ip));
      printf("connection %d from ('%s', %d)\n", conn_count++, cli_ip, cli_addr.sin_port);

      // Multi Threading data
      pthread_t tid1;
      td thd;
      thd.cli_addr = cli_addr;
      thd.cli_ip = cli_ip;
      thd.sock_fd = client_conn_fd;
      thd.tid = &tid1;

      // creating a thread using pthread_create(thread_id, NULL, function_to_execute, <-args_for_the_function)
      pthread_create(&tid1, NULL, chat_messages, (void *)&thd);

      // pthread_exit(NULL);
    }
  }
  else
  {
    // UDP passive socket
    int udp_socket;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Assign address to socket
    int status = bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (status < 0 || udp_socket < 0)
    {
      perror("socket creation/binding failed");
      exit(EXIT_FAILURE);
    }

    while (1)
    {
      struct sockaddr_in cli_addr;
      socklen_t addr_len = sizeof(cli_addr);
      int recv;
      char message_buffer[256];
      bzero(message_buffer, 256);

      recv = recvfrom(udp_socket, message_buffer, 256, 0, (struct sockaddr *)&cli_addr, &addr_len);
      char cli_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, sizeof(cli_ip));

      printf("got message from ('%s', %d)\n", cli_ip, cli_addr.sin_port);
      if (strncmp("hello", message_buffer, strlen("hello")) == 0)
      {
        sendto(udp_socket, "world\n", recv, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
      }
      else if (strncmp("goodbye", message_buffer, strlen("goodbye")) == 0)
      {
        sendto(udp_socket, "farewell\n", recv, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
      }
      else if (strncmp("exit", message_buffer, strlen("exit")) == 0)
      {
        sendto(udp_socket, "ok\n", recv, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
        break;
      }
      else
      {
        sendto(udp_socket, message_buffer, recv, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
      }
      bzero(message_buffer, 256);
    }
    close(udp_socket);
  }
}

void chat_client(char *host, long port, int use_udp)
{
  // client
  struct sockaddr_in cli_addr;
  struct hostent *he;
  struct in_addr **addr_list;
  char cli_message_buffer[256];

  // handle localhost input
  he = gethostbyname(host);
  addr_list = (struct in_addr **)he->h_addr_list;
  strcpy(host, inet_ntoa(*addr_list[0]));

  cli_addr.sin_family = AF_INET;
  cli_addr.sin_port = htons(port);
  cli_addr.sin_addr.s_addr = inet_addr(host);

  if (use_udp == 0)
  {
    // TCP Active socket
    int cli_sock_desc, cli_conn;
    cli_sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    cli_conn = connect(cli_sock_desc, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
    if (cli_conn < 0)
      perror("Something gone wrong, connection failed\n");
    while (1)
    {
      char buffer[256];
      bzero(cli_message_buffer, 256);
      if (fgets(cli_message_buffer, 256, stdin) == NULL)
        perror("Reading from client unsuccessfull");
      send(cli_sock_desc, cli_message_buffer, strlen(cli_message_buffer), 0);
      if (strncmp("goodbye", cli_message_buffer, strlen("goodbye")) == 0 || strncmp("exit", cli_message_buffer, strlen("exit")) == 0)
        strcpy(buffer, cli_message_buffer);
      // bzero(cli_message_buffer, 255);
      recv(cli_sock_desc, cli_message_buffer, 256, 0);
      // cli_read_bytes = read(cli_sock_desc, cli_message_buffer, strlen(cli_message_buffer));
      printf("%s", cli_message_buffer);
      if (strncmp("goodbye", buffer, strlen("goodbye")) == 0 || strncmp("exit", buffer, strlen("exit")) == 0)
        break;
    }
    close(cli_sock_desc);
  }
  else
  {
    // UDP active socket
    int cli_udp_sock_desc;
    cli_udp_sock_desc = socket(AF_INET, SOCK_DGRAM, 0);

    while (1)
    {
      char buff[256];
      bzero(cli_message_buffer, 256);

      if (fgets(cli_message_buffer, 256, stdin) == NULL)
        perror("Reading from UDP client unsuccessfull");

      sendto(cli_udp_sock_desc, cli_message_buffer, strlen(cli_message_buffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
      if (strncmp("goodbye", cli_message_buffer, strlen("goodbye")) == 0 || strncmp("exit", cli_message_buffer, strlen("exit")) == 0)
        strcpy(buff, cli_message_buffer);
      bzero(cli_message_buffer, 256);
      recvfrom(cli_udp_sock_desc, cli_message_buffer, 256, 0, NULL, NULL);
      printf("%s", cli_message_buffer);
      if (strncmp("goodbye", cli_message_buffer, strlen("goodbye")) == 0 || strncmp("exit", cli_message_buffer, strlen("exit")) == 0)
        break;
    }
    close(cli_udp_sock_desc);
  }
}

// Helper function for the TCP chat messages
// Use this as a function argument in the pthread_create()
void *chat_messages(void *arg)
{

  int read_bytes;
  td *thd = (td *)arg;
  struct sockaddr_in cli_addr = thd->cli_addr;
  char *cli_ip = thd->cli_ip;
  int client_conn_fd = thd->sock_fd;
  while (1)
  {
    char message_buffer[256];
    bzero(message_buffer, 256);
    read_bytes = read(client_conn_fd, message_buffer, 256);

    if (read_bytes < 0)
    {
      perror("Reading failed");
    }

    printf("got message from ('%s', %d)\n", cli_ip, cli_addr.sin_port);
    if (strncmp("hello", message_buffer, strlen("hello")) == 0)
    {
      send(client_conn_fd, "world\n", strlen("world\n"), 0);
    }
    else if (strncmp("goodbye", message_buffer, strlen("goodbye")) == 0)
    {
      send(client_conn_fd, "farewell\n", strlen("farewell\n"), 0);
      close(client_conn_fd);
      break;
    }
    else if (strncmp("exit", message_buffer, strlen("exit")) == 0)
    {
      send(client_conn_fd, "ok\n", strlen("ok\n"), 0);
      close(client_conn_fd);
      // break client loop condition
      exit(EXIT_SUCCESS);
    }
    else
    {
      // printf("Insdie nothing before write message buufer : %s\n",message_buffer);
      send(client_conn_fd, message_buffer, strlen(message_buffer), 0);
    }
    // Clear the buffer
    bzero(message_buffer, 256);
  }
  return NULL;
}
