#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Please provide the port number!\n");
        return 0;
    }

    int portno = atoi(argv[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    char buffer[255];
    struct sockaddr_in server_addr, client_addr;

    // Clear the server address structure
    bzero((char *)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error binding socket");
    }

    listen(sockfd, 5);
    int client_len = sizeof(client_addr);

    int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (newsockfd < 0) {
        error("Error accepting connection");
    }

    printf("Client connected. Waiting for messages...\n");

    while (1) {
        bzero(buffer, 255);

        // Read data from the client
        int n = read(newsockfd, buffer, 255);
        if (n < 0) {
            perror("Warning: Error reading from socket");
            continue;
        } else if (n == 0) {
            printf("Client disconnected.\n");
            break;
        }

        // Display the client's message
        printf("Client: %s\n", buffer);

        // Check for termination message
        if (strncasecmp(buffer, "Bye", 3) == 0) {
            printf("Client sent 'Bye'. Closing connection...\n");
            break;
        }

        // Clear buffer for server's response
        bzero(buffer, 255);
        printf("You: ");
        fgets(buffer, 255, stdin);

        // Write response to the client
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Warning: Error writing to socket");
        }
    }

    close(newsockfd);

    close(sockfd);
    return 0;
}
