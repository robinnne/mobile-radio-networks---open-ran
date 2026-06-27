//
// Created by Eugenio Moro on 24/06/22.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oai-oran-protolib/builds/ran_messages.pb-c.h"
#include "gnb_message_handlers.h"


void handle_master_message_local(void* buf, int buflen, int out_socket, struct sockaddr_in servaddr){
    RANMessage* in_mess = ran_message__unpack(NULL, (size_t)buflen, buf);
    if (!in_mess){
        printf("error decoding received message, printing for debug:\n");
        for(int i=0;i<buflen; i++){
            uint8_t* tempbuf = (uint8_t*) buf;
            printf(" %hhx ", tempbuf[i]);
        }
        printf("\n");
        return;
    }
    printf("ran message id %d\n", in_mess->msg_type);
    switch(in_mess->msg_type){
        case RAN_MESSAGE_TYPE__SUBSCRIPTION:
            printf("Subcription message received\n");
            handle_subscription(in_mess);
            break;
        case RAN_MESSAGE_TYPE__INDICATION_REQUEST:
            printf("Indication request message received\n");
            handle_indication_request(in_mess, out_socket, servaddr);
            break;
        case RAN_MESSAGE_TYPE__INDICATION_RESPONSE:
            printf("Indication response message received\n");
            build_indication_response(in_mess, out_socket, servaddr);
            break;
        case RAN_MESSAGE_TYPE__CONTROL:
            printf("Control message received\n");
            handle_control(in_mess);
            break;
        default:
            printf("Unrecognized message type\n");
    }
}

int main(int argc, char **argv) {
    int in_sockfd, out_sockfd;

    struct sockaddr_in out_sockaddr,in_sockaddr;
    int reuse = 1;
    int in_port = 6655;
    int out_port = 6600;    

    printf("Starting e2server emulator\n");
    /* Listen on localhost:port for UDP connections */

    // create sockets
    if((in_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Failed to create in socket\n");
        exit(EXIT_FAILURE);
    }
    setsockopt(in_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if((out_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Failed to create out socket\n");
        exit(EXIT_FAILURE);
    }
    setsockopt(out_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // bind to localhost for both sockets
    memset(&out_sockaddr, 0, sizeof(out_sockaddr));
    memset(&in_sockaddr, 0, sizeof(in_sockaddr));
    
    out_sockaddr.sin_family = AF_INET;
    out_sockaddr.sin_addr.s_addr = INADDR_ANY;
    out_sockaddr.sin_port = htons(out_port);
    //if (bind(in_sockfd, (struct sockaddr *) &out_sockaddr, sizeof(out_sockaddr)) != 0) {
    //    perror("Failed to bind in socket");
    //    exit(EXIT_FAILURE);
    //}

    in_sockaddr.sin_family = AF_INET;
    in_sockaddr.sin_addr.s_addr = INADDR_ANY;
    in_sockaddr.sin_port = htons(in_port);
    if (bind(in_sockfd, (struct sockaddr *) &in_sockaddr, sizeof(in_sockaddr)) != 0) {
        perror("Failed to bind out socket");
        exit(EXIT_FAILURE);
    }


    // wait for datagrams
    printf("Waiting for UDP datagrams\n");
    int max_rcv_len = 4096;
    uint8_t recv_buf[max_rcv_len];
    int rcv_len;
    unsigned slen;
    slen = sizeof(in_sockaddr);
    for(;;){
        rcv_len = recvfrom(in_sockfd, recv_buf, max_rcv_len, 0, (struct sockaddr *) &in_sockaddr, &slen);
        printf("\n");
        printf("-------------------------------\n");
        printf("Received %d bytes\n", rcv_len);
        handle_master_message(recv_buf, rcv_len, out_sockfd, out_sockaddr);   
    }
    /*
    for (;;) {
        connfd = accept(listenfd, NULL, NULL);

        if (connfd < 0) {
            perror("accept");
            return 1;
        }
        printf("Got connection.\n");
        //sleep(5);
        handle_connection(connfd);

        printf("Closing connection.\n");

        close(connfd);
    }
    */
    

    return 0;
}