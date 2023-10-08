#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define CHUNK_SIZE 10
#define TIMEOUT_SEC 0.25
#define MAX_SEQ_NUM 100

bool ackarr[MAX_SEQ_NUM];
struct timeval tv[MAX_SEQ_NUM];
char bufchonks[MAX_SEQ_NUM][CHUNK_SIZE + 1];
int num_chunks;
struct sockaddr_in server_addr;

void* ack_thread(void* sockfdptr) {
    int sockfd = *((int*)sockfdptr);
    
    while (1) {
        char buf[6];
        int seq_num;
        
        recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if (sscanf(buf, "ACK%02d", &seq_num) == 1) {
            ackarr[seq_num] = true;
            printf("Received ACK%d\n", seq_num);
        }

        for (int i = 0; i < num_chunks; i++) {
            if (!ackarr[i]) {
                break;
            }
            if (i == num_chunks - 1) {
                // printf("All chunks acknowledged\n");
                return NULL;
            }
        }    
    }
}

void* resend_thread(void* sockfdptr) {
    int sockfd = *((int*)sockfdptr);
    
    while (1) {
        struct timeval curr;
        gettimeofday(&curr, NULL);
        int flag = 0;
        
        for (int i = 0; i < num_chunks; i++) {
            if (!ackarr[i]) {
                flag = 1;
                if (bufchonks[i][0] != '\0') {
                    float time_elapsed = (curr.tv_sec - tv[i].tv_sec) + (curr.tv_usec - tv[i].tv_usec) / 1000000.0;
                    if (time_elapsed > TIMEOUT_SEC) {
                        char buffer[CHUNK_SIZE + 8];
                        snprintf(buffer, sizeof(buffer), "DATA%02d|%s", i, bufchonks[i]);                    
                        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                        gettimeofday(&tv[i], NULL);
                        printf("Resending chunk %d\n", i);
                    }
                }
            }
        }
        
        if (!flag) {
            // printf("All chunks acknowledged. Stopping resend\n");
            sendto(sockfd, "T", 1, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            return NULL;
        }
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    pthread_t ack_thread_id, resend_thread_id;
    
    while (1) {
        char input[1024];
        printf("Enter a message: ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';
        
        int input_len = strlen(input);
        num_chunks = (input_len + CHUNK_SIZE - 1) / CHUNK_SIZE;

        memset(ackarr, false, sizeof(ackarr));
        memset(bufchonks, '\0', sizeof(bufchonks));

        char num_chunks_str[4];
        snprintf(num_chunks_str, sizeof(num_chunks_str), "%02d", num_chunks);
        sendto(sockfd, num_chunks_str, sizeof(num_chunks_str), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        pthread_create(&ack_thread_id, NULL, ack_thread, (void*)&sockfd);
        pthread_create(&resend_thread_id, NULL, resend_thread, (void*)&sockfd);


        for (int i = 0; i < num_chunks; i++) {
            char chunk[CHUNK_SIZE + 8];
            snprintf(chunk, sizeof(chunk), "DATA%02d|%.*s", i, CHUNK_SIZE, input + i * CHUNK_SIZE);
            
            gettimeofday(&tv[i], NULL);
            strncpy(bufchonks[i], chunk + 7, CHUNK_SIZE);
            
            sendto(sockfd, chunk, strlen(chunk), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Sent chunk %d\n", i);
            printf("Chunk %d: %s\n", i, chunk);
        }

        pthread_join(ack_thread_id, NULL);
        pthread_join(resend_thread_id, NULL);
    }
    
    close(sockfd);
}
