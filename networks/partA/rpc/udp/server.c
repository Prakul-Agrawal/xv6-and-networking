#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define PORT_A 8080
#define PORT_B 8081
#define MAX_BUFFER_SIZE 1024

void die(const char *msg, bool close_socket_A, bool close_socket_B, int fd_A, int fd_B) {
    perror(msg);
    if (close_socket_A) {
        close(fd_A);
    }
    if (close_socket_B) {
        close(fd_B);
    }
    exit(EXIT_FAILURE);
}

int main() {
    int server_socket_a, server_socket_b, client_socket_a, client_socket_b, valread;
    struct sockaddr_in server_address_a, server_address_b, client_address_a, client_address_b;
    socklen_t addrlen_a = sizeof(client_address_a);
    socklen_t addrlen_b = sizeof(client_address_b);
    int choice_a, choice_b;
    char judgment[MAX_BUFFER_SIZE] = {0};

    if ((server_socket_a = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Socket creation for clientA failed", false, false, 0, 0);
    }

    if ((server_socket_b = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Socket creation for clientB failed", true, false, server_socket_a, 0);
    }

    memset(&server_address_a, 0, sizeof(server_address_a));
    server_address_a.sin_family = AF_INET;
    server_address_a.sin_addr.s_addr = INADDR_ANY;
    server_address_a.sin_port = htons(PORT_A);

    memset(&server_address_b, 0, sizeof(server_address_b));
    server_address_b.sin_family = AF_INET;
    server_address_b.sin_addr.s_addr = INADDR_ANY;
    server_address_b.sin_port = htons(PORT_B);

    if (bind(server_socket_a, (struct sockaddr *)&server_address_a, sizeof(server_address_a)) < 0) {
        die("Bind for clientA failed", true, true, server_socket_a, server_socket_b);
    }

    if (bind(server_socket_b, (struct sockaddr *)&server_address_b, sizeof(server_address_b)) < 0) {
        die("Bind for clientB failed", true, true, server_socket_a, server_socket_b);
    }

    printf("Server is waiting for clients...\n");

    int play_again = 1;

    while (play_again) {
        valread = recvfrom(server_socket_a, &choice_a, sizeof(choice_a), 0, (struct sockaddr *)&client_address_a, &addrlen_a);
        if (valread < 1) break;

        valread = recvfrom(server_socket_b, &choice_b, sizeof(choice_b), 0, (struct sockaddr *)&client_address_b, &addrlen_b);
        if (valread < 1) break;

        printf("ClientA chose: %d\n", choice_a);
        printf("ClientB chose: %d\n", choice_b);

        if (choice_a == choice_b) {
            strcpy(judgment, "Draw");
        } else if ((choice_a == 0 && choice_b == 2) ||
                   (choice_a == 1 && choice_b == 0) ||
                   (choice_a == 2 && choice_b == 1)) {
            strcpy(judgment, "ClientA Wins!");
        } else {
            strcpy(judgment, "ClientB Wins!");
        }

        sendto(server_socket_a, judgment, strlen(judgment), 0, (struct sockaddr *)&client_address_a, addrlen_a);
        sendto(server_socket_b, judgment, strlen(judgment), 0, (struct sockaddr *)&client_address_b, addrlen_b);

        printf("Result: %s\n", judgment);

        char play_response_a[MAX_BUFFER_SIZE];
        char play_response_b[MAX_BUFFER_SIZE];
        memset(play_response_a, 0, sizeof(play_response_a));
        memset(play_response_b, 0, sizeof(play_response_b));

        recvfrom(server_socket_a, play_response_a, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&client_address_a, &addrlen_a);
        recvfrom(server_socket_b, play_response_b, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&client_address_b, &addrlen_b);

        if (strcmp(play_response_a, "yes") != 0 || strcmp(play_response_b, "yes") != 0) {
            play_again = 0;
        }

        sendto(server_socket_a, &play_again, sizeof(play_again), 0, (struct sockaddr *)&client_address_a, addrlen_a);
        sendto(server_socket_b, &play_again, sizeof(play_again), 0, (struct sockaddr *)&client_address_b, addrlen_b);
    }

    close(server_socket_a);
    close(server_socket_b);
}
