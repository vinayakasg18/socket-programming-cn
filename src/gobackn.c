// /**************************************************************
// /* gobackn.c - Vinayaka Gadag (vgadag)
// /* CREATED: 11/26/2021
// /* go back N Implementation
// /* References:
// /* https://www2.tkn.tu-berlin.de/teaching/rn/animations/gbn_sr/ 
// /* https://www.baeldung.com/cs/networking-go-back-n-protocol 
// /* https://www.youtube.com/watch?v=QD3oCelHJ20&ab_channel=NesoAcademy
// /**************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include "gobackn.h"
#define MAX 190
/*
 *  Here is the starting point for your netster part.4 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */

typedef struct Frame
{
    short frame_kind;
    short seq_nor;
    short data_len;
    char data[MAX];
} Frame;

int data_chunk(char *file_buff, char *data, int file_size, int start)
{
    printf("Insize get chunk size\n");
    int start_pointer = start * MAX;
    int size_of_chunk = MAX;
    if (start_pointer > file_size)
    {
        return 0;
    }
    if ((start_pointer + MAX) > file_size)
    {
        size_of_chunk = file_size - start_pointer;
    }
    int s = 0;
    while (s < size_of_chunk)
    {
        // printf("Inside get chunk size while loop\n");
        data[s] = file_buff[start_pointer + s];
        s++;
    }
    // printf("Got chunk size: %d\n", size_of_chunk);
    return size_of_chunk;
}

void gbn_server(char *iface, long port, FILE *fp)
{
    printf("I'm gobackN server\n");
    int server_socket;
    struct sockaddr_in server_addr, client_addr;

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
    int status = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

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
    // short frame_id = 0;
    short seq_nor = 0;
    int count = 0;

    printf("Waiting for connection\n");
    while (0 < (data_received = recvfrom(server_socket, &frame_recv, sizeof(Frame), 0, (struct sockaddr *)&client_addr, &client_len)))
    {
        printf("Inside Server, count: %d\n", count++);
        printf("[+] Frame received %s\n", frame_recv.data);
        printf("Size of the data %d", frame_recv.data_len);

        if (frame_recv.frame_kind && frame_recv.seq_nor == seq_nor)
        {
            printf("Size of data writing to server:  %d\n", frame_recv.data_len);
            fwrite(frame_recv.data, sizeof(char), frame_recv.data_len, fp);
            printf("Data Received\n");
            frame_send.seq_nor = seq_nor;
            frame_send.frame_kind = 0;
            seq_nor += 1;
            sendto(server_socket, &frame_send, sizeof(frame_send), 0, (struct sockaddr *)&client_addr, client_len);
            printf("ACK SENT; frame kind: %d\n", frame_send.frame_kind);
            printf("ACK SENT; seq nor: %d\n", frame_send.seq_nor);

            if (frame_recv.data_len < MAX - 1)
            {
                printf("Not enough data\n");
                // exit(EXIT_FAILURE);
                break;
            }
        }
        else
        {
            printf("Frame didn't match\n");
            printf("Resending ...\n");
            frame_send.seq_nor = seq_nor - 1;
            frame_send.frame_kind = 0;
            printf("ACK resend; server: %d\n", frame_send.seq_nor);
            sendto(server_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr *)&client_addr, client_len);
            printf("ACK re-sent \n");
            continue;
        }
    }
}

void gbn_client(char *host, long port, FILE *fp)
{
    printf("I'm gobackN client\n");
    int client_socket;
    int window_size = 3;
    int seq_nor = 0;
    int base = 0;
    struct sockaddr_in server_addr;
    struct hostent *he;
    struct in_addr **addr_list;
    char client_data[MAX];
    char data_buffer[7000];
    Frame frame_send;
    Frame frame_recv;
    size_t data_;
    int nframes = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    printf("After sock creation\n");
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
    printf("After set sock\n");
    int flag = 1;
    if (-1 == setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
    {
        perror("setsockopt fail\n");
    }

    // Handle localhost
    he = gethostbyname(host);
    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(host, inet_ntoa(*addr_list[0]));

    // memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    socklen_t server_len = sizeof(server_addr);

    while (1)
    {
        if ((data_ = fread(client_data, sizeof(char), MAX - 1, fp)) > 0)
        {
            printf("Inside file reading\n");
            int data_size = MAX - 1;
            if (data_ < MAX - 1)
            {
                data_size = data_;
            }
            int st = 0;
            while (st < data_size)
            {
                data_buffer[nframes + st] = client_data[st];
                st++;
            }
            nframes += data_;
        }
        else
        {
            break;
        }
    }

    int frames = 0;
    printf("Number of frames: %d\n", (nframes / MAX) + 1);
    if (nframes % MAX == 0)
    {
        frames = nframes / MAX;
        printf("Number of frames: %d\n", frames);
    }
    else
    {
        frames = nframes / MAX + 1;
        printf("Number of frames: %d\n", frames);
    }
    // int frames = (nframes/MAX)+1;
    while (1)
    {
        //  while (seq_nor >= base && (base + window_size) >= seq_nor)
        printf("%d < (%d + %d)\n", seq_nor, base, window_size);
        while (seq_nor < (base + window_size))
        {
            if (seq_nor > frames)
            {
                break;
            }
            // printf("Inside second while nested\n");
            char chunk_data[MAX];
            int chunk_size = data_chunk(data_buffer, chunk_data, nframes, seq_nor);
            if (chunk_size > 0)
            {
                printf("Got chunk size: %d \n", chunk_size);
                frame_send.seq_nor = seq_nor;
                frame_send.data_len = chunk_size;
                frame_send.frame_kind = 1;
                memcpy(frame_send.data, chunk_data, chunk_size);
                printf("Sending a data to server; data length: %d\n", frame_send.data_len);
                printf("[+] Frame kind: %d\n", frame_send.frame_kind);
                printf("[+] Frame sequence number: %d\n", frame_send.seq_nor);
                sendto(client_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr *)&server_addr, server_len);
                seq_nor++;
            }
            else
            {
                printf("\n----------\n");
                printf("Got no data to send\n");
                break;
            }
        }
        int read_bytes = recvfrom(client_socket, &frame_recv, sizeof(Frame), 0, NULL, NULL);
        if (read_bytes > 0)
        {
            printf("Acknowledgement received for : %d\n", frame_recv.seq_nor);
            if (base == frames - 1)
            {
                printf("[+] Transfer complete [+]\n");
                exit(EXIT_SUCCESS);
            }
            base = frame_recv.seq_nor + 1;
            window_size = window_size + 1;
        }
        else
        {
            printf("\n Resending the data \n");
            seq_nor = base;
            window_size /= 2;
            if (window_size < 1)
            {
                window_size = 1;
            }
        }
    }
}
