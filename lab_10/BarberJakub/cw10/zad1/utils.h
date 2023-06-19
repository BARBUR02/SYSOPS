#define STOP 1
#define LIST 2
#define TO_ONE 3
#define TO_ALL 4
#define INIT 5
#define PING 6
#define MAX_NUMBER_OF_CLIENTS 11
#define MAX_NUMBER_OF_EVENTS 11
#define MAX_MESSAGE_LENGTH 512
#define MAX_USERNAME_LENGTH 128
#define BUFFER_SIZE 1024
#define PING_INTERVAL 5

struct Message {
    int receiver_id;
    int client_id;
    int sender_id;
    int order;
    char username[MAX_USERNAME_LENGTH];
    char msg[MAX_MESSAGE_LENGTH];
};