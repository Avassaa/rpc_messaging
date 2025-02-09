struct client {
    char *name;
};

struct message {
    char *sender;
    char *recipient;
    char *content;
};

program MYAPP_PROG {
    version MYAPP_VERSION {
        boolean register_client(client) = 1;
        boolean send_message(message) = 2;
        message fetch_message(client) = 3;
    } = 1;
} = 0x12471;