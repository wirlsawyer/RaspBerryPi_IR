#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int SendMessage(char *ip, int port, char *send_data)
{
    int sock, bytes_recieved;  
    
    struct hostent *host;
    struct sockaddr_in server_addr;  

    host = gethostbyname(ip);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("error:socket(AF_INET, SOCK_STREAM, 0)\n");
        return 0;
    }

    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(port);   
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8); 

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
    {
        printf("error:socket connect\n");
        return 0;
    }
    
    if (send(sock, send_data, strlen(send_data), 0) < 0)  
    {
        printf("error:socket send\n");
    }
    
    close(sock);
    return 1;
}