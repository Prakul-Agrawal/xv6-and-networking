#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define PORT 8080
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
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE] = {0};
    const char *message = "Server says Hello! =)\n";

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Socket creation failed", false, 0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("Bind failed", true, sockfd);
    }

    printf("UDP Server is listening on port %d...\n", PORT);

    while(1) {
        int recv_len = recvfrom(sockfd, (char *)buffer, MAX_BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&client_addr, &len);
        if (recv_len < 1) {
            perror("recvfrom failed");
            continue;
        }
        buffer[recv_len] = '\0';
        printf("Received message from client: %s\n", buffer);

        strcpy(buffer, "\0");

        sendto(sockfd, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&client_addr, len);
        printf("Message sent to client.\n\n");
    }

    close(sockfd);
}
