//
// Created by ivan on 04.06.15.
//
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 4096
#define PORT 18219
#define ERROR(code, name) if ((code)<0) {perror(name); _exit(-1);}

char buffer[BUFSIZE];

int main(int argc, char *argv[])
{
    struct hostent* server;
    struct sockaddr_in servaddr;
    int sd, readb, i;
    char eof[1];
    *eof=EOF;

    if(argc < 3)
    {
        write(1, "USAGE: client <hostname> <dir>\n", 31);
        _exit(-1);
    }

    ERROR(sd=socket(AF_INET, SOCK_STREAM, 0), "socket");
    ERROR(server=gethostbyname(argv[1]), "ghbn");

    servaddr.sin_family=AF_INET;
    ERROR(memcpy((char *)&servaddr.sin_addr,server->h_addr,
               server->h_length), "memcpy");
    servaddr.sin_port=htons(PORT);

    ERROR(connect(sd,(struct sockaddr*)&servaddr,sizeof(servaddr)), "connect");

    for(i=2; i<argc; i++)
    {
        ERROR(send(sd, argv[i], strlen(argv[i])+1, 0), "send");

        do
        {
            ERROR(readb=recv(sd, buffer, BUFSIZE, 0), "recv");
            if (buffer[readb-1] == EOF)
            {
                write(1, buffer, readb-1);
                break;
            }
            write(1, buffer, readb);

        } while(1);
    }

    ERROR(send(sd, eof, 1, 0), "send");
    ERROR(close(sd), "close");

    return 0;
}
