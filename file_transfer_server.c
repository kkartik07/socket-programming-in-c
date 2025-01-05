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

    printf("Client connected. Starting file transfer...\n");

    FILE *f;
    f = fopen("store.txt", "a");
    if (f == NULL) {
        error("Error opening file to write");
    }

    int words = 0;
    if (read(newsockfd, &words, sizeof(int)) < 0) {
        error("Error reading number of words from client");
    }

    for (int i = 0; i < words; i++) {
        bzero(buffer, 255);
        if (read(newsockfd, buffer, 255) < 0) {
            error("Error reading word from client");
        }
        fprintf(f, "%s ", buffer);
    }

    printf("Successfully received and stored file data!\n");

    fclose(f);
    close(newsockfd);
    close(sockfd);
    return 0;
}
