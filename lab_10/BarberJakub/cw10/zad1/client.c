#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include "utils.h"

int c_socket;
int client_id;
char username[MAX_USERNAME_LENGTH];

int is_tcp;
char* unix_socket_path;
int server_port;
char* server_address;
struct sockaddr_in addr;
struct sockaddr_un unix_address;
char buffer[BUFFER_SIZE];

void clean_client() {
    shutdown(c_socket, SHUT_RDWR);
    close(c_socket);
}


void init_client() {
    printf("Begin client init...\n");

    struct Message request;
    request.client_id = -1;
    request.order = INIT;
    strcpy(request.username, username);
    snprintf(request.msg, sizeof(request.msg), "%s has joined the chat", username);

    if (write(c_socket, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }

    struct Message respond;
    if (read(c_socket, &respond, sizeof(struct Message)) == -1) {
        perror("read");
        exit(1);
    }

    client_id = respond.client_id;

    printf("Client initialized with ID: %d\n", client_id);
}

void connect_local() {
    unix_socket_path = server_address;

    if ((c_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&unix_address, sizeof(unix_address));
    unix_address.sun_family = AF_UNIX;
    strncpy(unix_address.sun_path, unix_socket_path, sizeof(unix_address.sun_path) - 1);

    if (connect(c_socket, (struct sockaddr *)&unix_address, sizeof(unix_address)) == -1) {
        perror("connect");
        exit(1);
    }
}

void connect_tcp() {
    char *address = strtok(server_address, ":");
    char *port = strtok(NULL, ":");
    server_port = atoi(port);

    if ((c_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, address, &addr.sin_addr) == -1) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(c_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(1);
    }
}




void handle_list() {
    printf("Client start list order...\n");

    struct Message request;
    request.order = LIST;
    request.client_id = client_id;

    if (write(c_socket, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}

void handle_to_one(char command[]) {
    printf("Client start to_one order...\n");

    struct Message request;
    request.order = TO_ONE;
    request.client_id = client_id;

    char *type = strtok(command, " ");
    char *id_str = strtok(NULL, " ");
    char *msg = strtok(NULL, "\n");

    int receiver_id = atoi(id_str);
    request.receiver_id = receiver_id;
    strcpy(request.msg, msg);

    printf("Sending %s %s client with id: %d\n", msg, type, receiver_id);

    if (write(c_socket, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}

void handle_stop() {
    printf("Client start stop order...\n");

    struct Message request;
    request.order = STOP;
    request.client_id = client_id;

    if (write(c_socket, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}

void handle_to_all(char command[]) {
    printf("Client start to_all order...\n");

    struct Message request;
    request.order = TO_ALL;
    request.client_id = client_id;

    char *type = strtok(command, " ");
    char *msg = strtok(NULL, "\n");

    strcpy(request.msg, msg);
    printf("Sending %s %s clients\n", msg, type);

    if (write(c_socket, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}



void handle_ping() {
    struct Message response;
    response.order = PING;
    response.client_id = client_id;

    if (write(c_socket, &response, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}

void stop_handler(int sig) {
    exit(0);
}

void sigint_handler(int sig) {
    handle_stop();
}



void receiver() {
    while (1) {
        struct Message response;
        if (read(c_socket, &response, sizeof(struct Message)) == -1) {
            perror("read");
            exit(1);
        }

        switch (response.order) {
            case LIST:
                printf("List of active users:\n%s", response.msg);
                break;
            case TO_ONE:
            case TO_ALL:
                printf("Received message: %s, client with id: %d\n", response.msg, response.client_id);
                break;
            case STOP:
                printf("Closing client\n");
                kill(getppid(), SIGUSR1);
                exit(0);
            case PING:
                handle_ping();
                break;
            default:
                break;
        }
    }
}

void sender() {
    signal(SIGUSR1, stop_handler);
    char command[MAX_MESSAGE_LENGTH];

    while (1) {
        if (fgets(command, MAX_MESSAGE_LENGTH, stdin) == NULL) {
            continue;
        }

        if (strncmp(command, "LIST", 4) == 0) {
            handle_list();
        }
        else if (strncmp(command, "STOP", 4) == 0) {
            handle_stop();
        }
        else if (strncmp(command, "TO_ALL", 6) == 0) {
            handle_to_all(command);
        }
        else if (strncmp(command, "TO_ONE", 6) == 0) {
            handle_to_one(command);
        }
        else {
            printf("Invalid command type\n");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    atexit(clean_client);

    strncpy(username, argv[1], MAX_USERNAME_LENGTH - 1);
    is_tcp = strcmp(argv[2], "network") == 0;
    server_address = argv[3];

    if (!is_tcp) {
        connect_local();
    }
    else {
        
        connect_tcp();
    }

    init_client();
    signal(SIGINT, sigint_handler);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        receiver();
    }
    else {
        sender();
    }
}