#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define SERVER_IP "127.0.0.1"
#define PORT_A 8080
#define PORT_B 8081
#define MAX_BUFFER_SIZE 1024

void die(const char *msg, bool close_socket, int fd) {
    perror(msg);
    if (close_socket) {
        close(fd);
    }
    exit(EXIT_FAILURE);
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    int choice, flag;
    char play_response[MAX_BUFFER_SIZE] = {0};

    printf("Are you ClientA or ClientB? (A/B): ");
    char client_choice;
    scanf(" %c", &client_choice);

    if (client_choice == 'A' || client_choice == 'a') {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            die("Socket creation for clientA failed", false, 0);
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT_A);

        if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
            die("Invalid address or address not supported", true, sock);
        }
    } else if (client_choice == 'B' || client_choice == 'b') {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            die("Socket creation for clientB failed", false, 0);
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT_B);

        if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
            die("Invalid address or address not supported", true, sock);
        }
    } else {
        printf("Invalid choice. Please choose 'A' or 'B'.\n");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        die("Connection failed", true, sock);
    }

    do {
        printf("Enter your choice (0 for Rock, 1 for Paper, 2 for Scissors): ");
        scanf("%d", &choice);

        if (choice < 0 || choice > 2) {
            printf("Invalid choice. Please choose 0, 1 or 2.\n");
            continue;
        }

        send(sock, &choice, sizeof(choice), 0);

        char buffer[MAX_BUFFER_SIZE] = {0};
        recv(sock, buffer, MAX_BUFFER_SIZE, 0);
        printf("Result: %s\n", buffer);

        while(1) {
            printf("Play again? (yes/no): ");
            scanf("%s", play_response);

            if (strcmp(play_response, "yes") != 0 && strcmp(play_response, "no") != 0) {
                printf("Invalid choice. Please choose 'yes' or 'no'.\n");
                continue;
            }
            break;
        }        

        send(sock, play_response, strlen(play_response), 0);

        recv(sock, &flag, sizeof(flag), 0);

    } while (flag);

    close(sock);
}
