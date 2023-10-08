#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8080
#define CHUNK_SIZE 10
#define MAX_SEQ_NUM 100

struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

void handle_client(int sockfd) {
    char num_chunks_str[4];
    recvfrom(sockfd, num_chunks_str, sizeof(num_chunks_str), 0, (struct sockaddr*)&client_addr, &client_len);
    int num_chunks = atoi(num_chunks_str);

    printf("Number of chunks: %d\n", num_chunks);

    char received_chunks[MAX_SEQ_NUM][CHUNK_SIZE + 8];
    int received_flag[MAX_SEQ_NUM] = {0};

    // printf("Reached here\n");

    while (1) {
        // char num_chunks_str[4];
        
        // // Receive the number of chunks from the client
        // // recv(sockfd, num_chunks_str, sizeof(num_chunks_str), 0);
        // recvfrom(sockfd, num_chunks_str, sizeof(num_chunks_str), 0, (struct sockaddr*)&client_addr, &client_len);
        // int num_chunks = atoi(num_chunks_str);
        
        // char received_chunks[MAX_SEQ_NUM][CHUNK_SIZE + 4];
        // int received_count = 0;
        
        // Receive data chunks from the client
        // for (int i = 0; i < num_chunks; i++) {
        //     char chunk[CHUNK_SIZE + 4];
        //     recv(sockfd, chunk, sizeof(chunk), 0);
            
        //     int seq_num;
        //     if (sscanf(chunk, "DATA%d|%s", &seq_num, received_chunks[seq_num]) == 2) {
        //         printf("Received chunk %d: %s\n", seq_num, received_chunks[seq_num]);
        //         received_count++;
        //     }
            
        //     // Send acknowledgment to the client for the received chunk
        //     char ack[6];
        //     snprintf(ack, sizeof(ack), "ACK%02d", seq_num);
        //     send(sockfd, ack, sizeof(ack), 0);
        // }

        char chunk[CHUNK_SIZE + 8];
        memset(chunk, 0, sizeof(chunk));
        int valread = recvfrom(sockfd, chunk, sizeof(chunk), 0, (struct sockaddr*)&client_addr, &client_len);
        if (valread < 1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        // printf("Received chunk %s\n", chunk);
        // printf("valread = %d\n", valread);

        int seq_num;
        // if (sscanf(chunk, "DATA%d|%.*s", &seq_num, CHUNK_SIZE, received_chunks[seq_num]) == 2) {
        //     printf("Received chunk %d: %s\n", seq_num, received_chunks[seq_num]);
        //     received_flag[seq_num] = 1;
        // }
        // if (sscanf(chunk, "DATA%d|%s", &seq_num, received_chunks[seq_num]) == 2) {
        //     printf("Received chunk %d: %s\n", seq_num, received_chunks[seq_num]);
        //     received_flag[seq_num] = 1;
        // }
        // seq_num = the integer value of the 4 and 5 indices of chunk
        seq_num = (chunk[4] - '0') * 10 + (chunk[5] - '0');
        // the respective received chunk = the string from 7th index to the end of chunk
        strncpy(received_chunks[seq_num], chunk + 7, CHUNK_SIZE);
        received_flag[seq_num] = 1;

        // printf("12345\n");
        
        // Send acknowledgment to the client for the received chunk
        char ack[6];
        snprintf(ack, sizeof(ack), "ACK%02d", seq_num);
        sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr*)&client_addr, client_len);
        // Combine received chunks to form the complete message
        // printf("Received message: ");
        // for (int i = 0; i < received_count; i++) {
        //     printf("%s", received_chunks[i]);
        // }
        // printf("\n");
        int flag = 1;
        for (int i = 0; i < num_chunks; i++) {
            if (received_flag[i] == 0) {
                flag = 0;
                break;
            }
        }
        if (flag == 1) {
            break;
        }
    }

    printf("Received message: ");
    // for (int i = 0; i < received_count; i++) {
    //     printf("%s", received_chunks[i]);
    // }
    for (int i = 0; i < num_chunks; i++) {
        printf("%s", received_chunks[i]);
    }
    printf("\n");
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        // struct sockaddr_in client_addr;
        // socklen_t client_len = sizeof(client_addr);
        // int newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        // if (newsockfd == -1) {
        //     perror("accept");
        //     continue;
        // }
        
        // printf("Accepted a new client connection\n");
        // handle_client(newsockfd);
        // close(newsockfd);
        handle_client(sockfd);
    }
    
    close(sockfd);
    return 0;
}
