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
    int server_socket, client_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};
    const char *message = "Server says Hello! =)\n";

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        die("Socket creation failed", false, 0);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        die("Bind failed", true, server_socket);
    }

    if (listen(server_socket, 3) < 0) {
        die("Listen failed", true, server_socket);
    }

    printf("TCP Server is listening on port %d...\n", PORT);

    while(1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        while(1) {
            valread = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
            if (valread < 1) break;
            buffer[valread] = '\0';
            printf("Received message from client: %s\n", buffer);

            strcpy(buffer, "\0");

            send(client_socket, message, strlen(message), 0);
            printf("Message sent to client.\n\n");
        }

        close(client_socket);
    }

    close(server_socket);
}
