/***
 * TCP Client/Server Project
 * @file client.c
 * @authors: Kevin Funderburg
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(){
    int clientSocket, n;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        error("ERROR opening socket");

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);

     // serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // local host
    serverAddr.sin_addr.s_addr = inet_addr("147.26.231.156"); // zeus server
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    for (;;) {
        bzero(buffer, sizeof(buffer));
        printf("What would you like to do? (enter h for help) : ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n')
            ;

        write(clientSocket, buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer));
        read(clientSocket, buffer, sizeof(buffer));

        if ((strncmp(buffer, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        } else
            printf("%s", buffer);
    }
    return 0;
}
