// /**************************************************************
// /* file.c - Vinayaka Gadag (vgadag)
// /* CREATED: 10/22/2021
// /* Transfer files from a client to a server over TCP and UDP.
// /*
// /**************************************************************

#include "file.h"
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
/*
 *  Here is the starting point for your netster part.2 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void file_server(char *iface, long port, int use_udp, FILE *fp)
{
    int server_socket;
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    char client_buffer[256];

    if (!use_udp)
    {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        size_t file_data;
        if (server_socket < 0)
        {
            perror("socket() failed");
            exit(EXIT_FAILURE);
        }
        // bind
        int status = bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr));

        if (status < 0)
        {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_socket, 9) != 0)
        {
            perror("listen failed");
        }

        struct sockaddr_in cli_addr;
        int client_new_sock, client_data_;
        socklen_t cli_len = sizeof(cli_addr);
        client_new_sock = accept(server_socket, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_new_sock < 0)
            perror("Couldn't accept the connection!");
        while (0 < (client_data_ = recv(client_new_sock, client_buffer, sizeof(client_buffer) - 1, 0)))
        {
            printf("Rading file input data...\n");
            file_data = fwrite(client_buffer, sizeof(char), client_data_, fp);
            if (file_data < 0)
                perror("Error writing file\n");
            // Clear buffer
            bzero(client_buffer, sizeof(client_buffer));
        }
    }
    else
    {
        // UDP
        int udp_socket, client_data_;
        char client_buffer[256];
        udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket < 0)
        {
            perror("Binidng failed for UDP socket\n");
        }
        int status = bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (status < 0)
        {
            perror("Binidng failed for UDP socket\n");
        }

        size_t file_data;
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);

        while (0 < (client_data_ = recvfrom(udp_socket, client_buffer, sizeof(client_buffer) - 1, 0, (struct sockaddr *)&cli_addr, &cli_len)))
        {
            printf("Writing file over UDP\n");
            file_data = fwrite(client_buffer, sizeof(char), client_data_, fp);
            if (client_data_ < 255)
            {
                perror("Failed to write file\n");
                // exit(EXIT_FAILURE);
                break;
            }
            if (file_data < 0)
            {
                perror("Error Writing file\n");
            }
            // Clear buffer
            bzero(client_buffer, 256);
        }
    }
}

void file_client(char *host, long port, int use_udp, FILE *fp)
{
    // client
    struct sockaddr_in server_addr;
    struct hostent *he;
    struct in_addr **addr_list;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    // Handle localhost
    he = gethostbyname(host);
    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(host, inet_ntoa(*addr_list[0]));

    if (!use_udp)
    {
        char server_buffer[256];
        size_t file_data;
        bzero(server_buffer, sizeof(server_buffer));
        int cli_sock_desc, cli_conn;
        cli_sock_desc = socket(AF_INET, SOCK_STREAM, 0);
        cli_conn = connect(cli_sock_desc, (struct sockaddr *)&server_addr, sizeof(server_addr));

        if (cli_conn < 0)
            perror("Something went wrong, connection failed\n");

        while (0 < (file_data = fread(server_buffer, sizeof(char), sizeof(server_buffer) - 1, fp)))
        {
            printf("sending file data over TCP...\n");
            send(cli_sock_desc, server_buffer, file_data, 0);
        }
    }

    else
    {
        int cli_sock_desc;
        cli_sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
        char server_buffer[256];
        bzero(server_buffer, sizeof(server_buffer));
        size_t file_data;
        // socklen_t serv_len = sizeof(server_addr);

        while (0 < (file_data = fread(server_buffer, sizeof(char), sizeof(server_buffer) - 1, fp)))
        {
            printf("Sending file data over udp\n");
            sendto(cli_sock_desc, server_buffer, file_data, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }
}
