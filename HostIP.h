#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

char *getIP(char *name)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, name, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    //printf("%s:%s\n", name, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr); 
}

/*
char *getBroadCastIP(char *name)
{
        char *pAddress = getIP(name);
        DWORD nMyIP = inet_addr(pAddress);
        DWORD nMySubMask = inet_addr("255.255.255.0");
        DWORD nTemp1 = nMyIP & nMySubMask;
        DWORD nTemp2 = nMySubMask ^ 0xFFFFFFFF;
        DWORD dwIP = nTemp1 + nTemp2;

 	struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr)); 
        server_addr.sin_addr.s_addr = dwIP;
        return inet_ntoa(server_addr.sin_addr);

}
      */
