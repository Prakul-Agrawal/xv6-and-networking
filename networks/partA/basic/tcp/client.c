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
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[MAX_BUFFER_SIZE] = {0}, buffer[MAX_BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        die("Socket creation failed", false, 0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        die("Invalid address or address not supported", true, sock);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        die("Connection failed", true, sock);
    }

    printf("Enter message to send to server: ");
    fgets(message, MAX_BUFFER_SIZE, stdin);
    send(sock, message, strlen(message), 0);
    printf("\nMessage sent to server.\n");

    strcpy(buffer, "\0");
    recv(sock, buffer, MAX_BUFFER_SIZE, 0);
    printf("\nReceived message from server: %s\n", buffer);

    close(sock);
}
