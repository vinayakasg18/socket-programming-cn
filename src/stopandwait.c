#include <stdio.h>
#include "stopandwait.h"
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
#include <time.h>
#include <errno.h>
#define MAX 200
/*
 *  Here is the starting point for your netster part.3 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

// int time_out(int sock_d)
// {
//     struct timeval timeout;
//     timeout.tv_sec = 5;
//     timeout.tv_usec = 0;

//     if (setsockopt(sock_d, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
//     {
//         perror("socket time_out");
//         return -1;
//     }
//     return 0;
// }

// typedef struct packet
// {
//     char data[256];
// } Packet;

typedef struct frame
{
    short frame_kind;
    short seq_nor;
    short frame_id;
    char data[MAX];
    short data_len;
    // Packet packet;
} Frame;

/* Add function definitions */
void stopandwait_server(char *iface, long port, FILE *fp)
{
    int server_socket;
    // time_t t;
    // srand((unsigned)time(&t));
    struct sockaddr_in server_addr, client_addr;
    // char client_buffer[256];

    // socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
    {
        perror("Socket connection failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind
    int status = bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    if (status != 0)
    {
        perror("Bind failed\n");
        exit(EXIT_FAILURE);
    }

    socklen_t client_len = sizeof(client_addr);
    int data_received;
    // size_t file_data;
    Frame frame_recv;
    Frame frame_send;
    short frame_id = 0;
    short seq_nor = 0;
    int count = 0;

    // time_out(server_socket);
    printf("Waiting for connection\n");
    // printf("Inside Server, count: %d\n", count++);
    while (1)
    {
        // time_out(server_socket);
        // data_received = recvfrom(server_socket, client_buffer, sizeof(client_buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        printf("Inside Server, count: %d\n", count++);
        if (0 < (data_received = recvfrom(server_socket, &frame_recv, sizeof(Frame), 0, (struct sockaddr *)&client_addr, &client_len)))
        {
            printf("[+] Frame received %s\n", frame_recv.data);
            printf("Size of the data %d", frame_recv.data_len);

            if (frame_recv.frame_kind && frame_recv.seq_nor == seq_nor)
            {
                printf("Size of data in server before writing:  %d\n", frame_recv.data_len);
                fwrite(frame_recv.data, sizeof(char), frame_recv.data_len, fp);
                // printf("Data received %lu\n", strlen(frame_recv.data));
                printf("Data Received\n");
                frame_send.frame_id = frame_id;
                frame_send.seq_nor = frame_recv.seq_nor + 1;
                frame_send.frame_kind = 0;
                sendto(server_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr *)&client_addr, client_len);
                printf("ACK server: %d\n", frame_send.frame_id);

                if (frame_recv.data_len < MAX - 1)
                {
                    printf("Not enough data\n");
                    exit(EXIT_FAILURE);
                }

                printf("ACK sent\n");
                frame_id++;
                seq_nor++;
            }
            else
            {
                printf("Frame didn't match\n");
                printf("Resending ...\n");
                printf("ACK resend server: %d\n", frame_send.seq_nor);
                sendto(server_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }
        }
        else
        {
            printf("Closing the connection\n");
            break;
        }
    }
}

void stopandwait_client(char *host, long port, FILE *fp)
{
    int client_socket;
    struct sockaddr_in server_addr;
    struct hostent *he;
    struct in_addr **addr_list;
    char client_data[MAX];
    size_t data_;
    int data_received;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 300000;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    int flag = 1;
    if (-1 == setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
    {
        perror("setsockopt fail\n");
    }

    // Handle localhost
    he = gethostbyname(host);
    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(host, inet_ntoa(*addr_list[0]));

    // Set to 0
    // memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    socklen_t server_len = sizeof(server_addr);

    short frame_id = 0;
    Frame frame_send;
    Frame frame_recv;
    // int ack_recv = 1;
    // int resend = 1;

    while (0 < (data_ = fread(client_data, sizeof(char), MAX - 1, fp)))
    {
        // bzero(client_data, sizeof(client_data));
        // printf("Inside client fread\n");
        frame_send.frame_id = frame_id;
        frame_send.frame_kind = 1;
        frame_send.seq_nor = frame_id;
        frame_send.data_len = data_;

        // strcpy(frame_send.data, client_data);
        bzero(frame_send.data, sizeof(frame_send.data));
        memcpy(frame_send.data, client_data, data_);
        while (1)
        {
            sendto(client_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr *)&server_addr, server_len);
            data_received = recvfrom(client_socket, &frame_recv, sizeof(Frame), 0, NULL, NULL);
            if (data_received > 0)
            {
                // printf("ACK Received %d \n", frame_recv.ack);
                if (frame_recv.frame_kind == 0 && (frame_recv.seq_nor == frame_send.seq_nor + 1))
                {
                    printf("ACK received client %d\n", frame_recv.seq_nor);
                    frame_id++;
                    break;
                }
                else{
                    printf("Retrying......\n");
                    continue;
                }
            }
            if (data_received < 0)
            {
                if (errno == EAGAIN)
                {
                    printf("TIMEOUT\n");
                    continue;
                }
                else
                {
                    printf("Recv failed\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
