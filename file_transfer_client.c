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

    printf("Connected to the server. Starting file transfer...\n");

    FILE *f;
    int words = 0;
    char c;

    // Open the file to be sent
    f = fopen("hello.txt", "r");
    if (f == NULL) {
        error("Error opening file");
    }

    // Count the number of words in the file
    while ((c = getc(f)) != EOF) {
        if (isspace(c) || c == '\t') {
            words++;
        }
    }

    // Send the word count to the server
    write(sockfd, &words, sizeof(int));
    rewind(f);

    char ch;
    // Send the file content word by word
    while ((ch = getc(f)) != EOF) {
        fscanf(f, "%s", buffer);
        write(sockfd, buffer, 255);
    }

    printf("The file has been sent successfully!\n");

    fclose(f);
    close(sockfd);
    return 0;
}
