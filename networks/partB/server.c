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

    while (1) {

        char chunk[CHUNK_SIZE + 8];
        memset(chunk, 0, sizeof(chunk));
        int valread = recvfrom(sockfd, chunk, sizeof(chunk), 0, (struct sockaddr*)&client_addr, &client_len);
        if (valread < 1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("Received chunk %s\n", chunk);

        if (chunk[0] == 'T') break;

        int seq_num;
        seq_num = (chunk[4] - '0') * 10 + (chunk[5] - '0');
        strncpy(received_chunks[seq_num], chunk + 7, CHUNK_SIZE);
        received_flag[seq_num] = 1;
        
        // Send acknowledgment to the client for the received chunk
        char ack[6];
        // printf("Seq num: %d\n", seq_num);
        // printf("Sending ACK%02d\n", seq_num);
        if (seq_num == 2) continue;
        snprintf(ack, sizeof(ack), "ACK%02d", seq_num);
        sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr*)&client_addr, client_len);
    }

    printf("Received message: ");
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
        handle_client(sockfd);
    }
    
    close(sockfd);
    return 0;
}
