#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>   
#include <ctype.h>

#define h_addr h_addr_list[0] /* for backward compatibility */

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[255];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error: no such host found\n");
        exit(1);
    }

    // Initialize the server address structure
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(portno);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to the server");
    }

    printf("Connected to the server. Type your messages below:\n");

    while (1) {
        bzero(buffer, 255);
        printf("You: ");
        fgets(buffer, 255, stdin);

        // Remove newline character from the buffer
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Append a newline delimiter to frame the message
        strcat(buffer, "\n");

        // Write data to the server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Warning: Error writing to socket");
            continue;
        }

        // Check if the message is "Bye" to terminate
        if (strncasecmp(buffer, "Bye", 3) == 0) {
            printf("Connection closing...\n");
            break;
        }

        bzero(buffer, 255);

        // Read data from the server
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            perror("Warning: Error reading from socket");
            continue;
        }

        printf("Server: %s", buffer);
    }

    close(sockfd);
    return 0;
}
