#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

void die(const char *msg, bool close_socket, int fd) {
    perror(msg);
    if (close_socket) {
        close(fd);
    }
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE] = {0}, message[MAX_BUFFER_SIZE] = {0};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Socket creation failed", false, 0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        die("Invalid address or address not supported", true, sockfd);
    }

    printf("Enter message to send to server: ");
    fgets(message, MAX_BUFFER_SIZE, stdin);
    sendto(sockfd, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("\nMessage sent to server.\n");

    strcpy(buffer, "\0");
    int recv_len = recvfrom(sockfd, (char *)buffer, MAX_BUFFER_SIZE, MSG_WAITALL, NULL, NULL);
    if (recv_len < 1) {
        die("recvfrom() failed", true, sockfd);
    }
    buffer[recv_len] = '\0';
    printf("Received message from server: %s\n", buffer);

    close(sockfd);
}
