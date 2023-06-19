#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"

struct Client {
    int client_id;
    int client_socket;
    char username[MAX_USERNAME_LENGTH];
};

int tcp_port;
int unix_socket;
int tcp_socket;
int ids = 0;
int clients_num = 0;
struct Client clients_sockets[MAX_NUMBER_OF_CLIENTS];
struct sockaddr_un server_address;
struct sockaddr_in tcp_address;
int epoll_fd;
struct epoll_event epoll_events[MAX_NUMBER_OF_EVENTS];
int poll_nfds;
char buffer[BUFFER_SIZE];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void clean() {
    unlink(server_address.sun_path);
}

void clear_sockets() {
    for (int i = 0; i < clients_num; i++) {
        struct Message request;
        request.order = STOP;
        request.client_id = clients_sockets[i].client_id;

        if (write(clients_sockets[i].client_socket, &request, sizeof(struct Message)) == -1) {
            perror("write");
            exit(1);
        }
    }

    for (int i = 0; i < clients_num; i++) {
        close(clients_sockets[i].client_socket);
    }

    shutdown(tcp_socket, SHUT_RDWR);
    shutdown(unix_socket, SHUT_RDWR);
    close(tcp_socket);
    close(unix_socket);
    exit(0);
}

void *handle_pings(void *arg) {
    while (1) {
        sleep(PING_INTERVAL);

        pthread_mutex_lock(&mutex);

        struct Message request;
        request.order = PING;

        for (int i = 0; i < clients_num; i++) {
            if (write(clients_sockets[i].client_socket, &request, sizeof(struct Message)) == -1) {
                perror("write");
                exit(1);
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void disconnect_client(int client_fd) {
    printf("Disconnecting client");

    int index = -1;
    for (int i = 0; i < clients_num; i++) {
        if (clients_sockets[i].client_socket == client_fd) {
            index = i;
            break;
        }
    }

    for (int i = index; i < clients_num - 1; i++) {
        clients_sockets[i] = clients_sockets[i + 1];
    }

    close(client_fd);
    clients_num--;
}

void handle_init(int i) {
    int client_socket = accept(epoll_events[i].data.fd, NULL, NULL);
    if (client_socket == -1) {
        perror("Error in accept function\n");
        exit(1);
    }

    char buffer[sizeof(struct Message)];
    ssize_t count = recv(client_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    if (count < 0) {
        perror("Error in recv function\n");
        exit(1);
    }

    struct Message request;
    memcpy(&request, buffer, sizeof(struct Message));
    printf("%s\n", request.msg);

    if (clients_num == MAX_NUMBER_OF_CLIENTS) {
        printf("Clients limit reaached\n");
        close(client_socket);
        return;
    }

    clients_sockets[clients_num].client_socket = client_socket;
    clients_sockets[clients_num].client_id = ids;
    strcpy(clients_sockets[clients_num].username, request.username);
    clients_num++;
    ids++;

    struct Message response;
    response.client_id = ids- 1;
    if (write(client_socket, &response, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
        perror("epoll_ctl");
        exit(1);
    }
}

void handle_list(int client_fd, struct Message received_message) {
    printf("Server start list order\n");

    struct Message response;
    response.order = LIST;
    response.client_id = received_message.client_id;
    memset(response.msg, 0, sizeof(response.msg));

    for (int i = 0; i < clients_num; i++) {
        sprintf(response.msg + strlen(response.msg), "Client with ID: %d is active\n", clients_sockets[i].client_id);
    }

    if (write(client_fd, &response, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }

    printf("Listed clients on server side\n");
}

void handle_to_one(int client_fd, struct Message received_message) {
    printf("Server start to_one order\n");

    int i;
    int receiver_index = -1;
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        if (clients_sockets[i].client_id == received_message.receiver_id) {
            receiver_index = i;
            break;
        }
    }

    if (receiver_index == -1) {
        printf("Client with id: %d, is not active\n", received_message.receiver_id);
        return;
    }

    struct Message response;
    response.order = TO_ONE;
    response.client_id = received_message.client_id;
    strcpy(response.msg, received_message.msg);

    if (write(clients_sockets[receiver_index].client_socket, &response, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }
}

void handle_to_all(int client_fd, struct Message received_message) {
    printf("Server start to_all order\n");

    for (int i = 0; i < clients_num; i++) {
        if (clients_sockets[i].client_socket != client_fd) {
            struct Message response;
            response.order = TO_ALL;
            response.client_id = received_message.client_id;
            strcpy(response.msg, received_message.msg);

            if (write(clients_sockets[i].client_socket, &response, sizeof(struct Message)) == -1) {
                perror("write");
                exit(1);
            }

            printf("Server send message to client with ID %d\n", clients_sockets[i].client_id);
        }
    }
}

void handle_stop(int client_fd, struct Message received_message) {
    printf("Server start stop order\n");

    int index = -1;
    for (int i = 0; i < clients_num; i++) {
        if (clients_sockets[i].client_id == received_message.client_id) {
            index = i;
            break;
        }
    }

    for (int i = index; i < clients_num - 1; i++) {
        clients_sockets[i] = clients_sockets[i + 1];
    }

    clients_num--;

    struct Message response;
    response.order = STOP;
    response.client_id = received_message.client_id;

    if (write(client_fd, &response, sizeof(struct Message)) == -1) {
        perror("write");
        exit(1);
    }

    close(client_fd);

    printf("Client with id: %d, stop working\n", received_message.client_id);
}

void run_server() {
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if ((poll_nfds = epoll_wait(epoll_fd, epoll_events, MAX_NUMBER_OF_EVENTS, -1)) == -1) {
            perror("epoll_wait");
            exit(1);
        }

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < poll_nfds; i++) {
            if (epoll_events[i].data.fd == unix_socket || epoll_events[i].data.fd == tcp_socket) {
                handle_init(i);
            }
            else {
                int client_fd = epoll_events[i].data.fd;
                ssize_t count = recv(client_fd, buffer, sizeof(buffer), 0);

                if (count == -1) {
                    perror("recv");
                    exit(1);
                }
                else if (count == 0) {
                    disconnect_client(client_fd);
                }
                else {
                    struct Message received_message;
                    memcpy(&received_message, buffer, sizeof(struct Message));

                    switch (received_message.order) {
                        case LIST:
                            handle_list(client_fd, received_message);
                            break;
                        case TO_ONE:
                            handle_to_one(client_fd, received_message);
                            break;
                        case TO_ALL:
                            handle_to_all(client_fd, received_message);
                            break;
                        case STOP:
                            handle_stop(client_fd, received_message);
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments");
        exit(1);
    }

    atexit(clean);
    tcp_port = atoi(argv[1]);

    if ((unix_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    strcpy(server_address.sun_path, argv[2]);
    server_address.sun_family = AF_UNIX;

    if (bind(unix_socket, (struct sockaddr *)&server_address, SUN_LEN(&server_address)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(unix_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("LOCAL KONIEC\n");

    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&tcp_address, sizeof(tcp_address));
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(tcp_port);
    tcp_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(tcp_socket, (struct sockaddr *)&tcp_address, sizeof(tcp_address)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(tcp_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("TCP KONIEC\n");

    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event local_event;
    local_event.events = EPOLLIN;
    local_event.data.fd = unix_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, unix_socket, &local_event) == -1) {
        perror("epoll_ctl");
        exit(1);
    }

    struct epoll_event tcp_event;
    tcp_event.events = EPOLLIN;
    tcp_event.data.fd = tcp_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_socket, &tcp_event) == -1) {
        perror("epoll_ctl");
        exit(1);
    }

    struct sigaction action;
    action.sa_handler = clear_sockets;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, handle_pings, NULL);

    run_server();

    return 0;
}