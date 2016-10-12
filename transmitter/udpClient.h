#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "color.h"
#include "log.h"
#include "HostIP.h"

#define DWORD unsigned long
#define TIMEOUT 3

void test()
{
    printf("test\n");
}


//void GetHostName(char *pName)
//{
// 	gethostname(pName, sizeof(pName));
//}

char *GetHostIP()
{
	char *IP = getIP("eth0");
   	printf("%seth0:%s%s%s\n", KMAG, KGRN, IP, KNRM);
	
    	if (strcmp(IP, "0.0.0.0") == 0 || strcmp(IP, "1.0.0.0") == 0 || strcmp(IP, "2.0.0.0") == 0) 
    	{
       		IP = getIP("wlan0");
       		printf("%swlan0:%s%s%s\n", KMAG, KGRN, IP, KNRM);
    	}
	saveToLog("Local IP");
	saveToLog(IP);
	return IP;
}

DWORD GetBoardHostIP()
{ 
    char *ip = GetHostIP();
    char hname[128];
    struct hostent *host;
    gethostname(hname, sizeof(hname));
    //host = (struct hostent *) gethostbyname(hname);
    host = (struct hostent *) gethostbyname(ip);

    printf("+-----------------------------------+\n");	
    printf("%shostname: %s%s%s\n", KMAG, KCYN, hname, KNRM);
    printf("%saddress list:%s\n", KMAG, KNRM);
    int i;
    DWORD dwIP;

   	 for (i=0; host->h_addr_list[i]; i++) 
	{
	        char *pAddress = inet_ntoa(*(struct in_addr*)(host->h_addr_list[i]));
        	printf("%s\t     %s%s\n", KCYN, pAddress, KNRM);

	        DWORD nMyIP = inet_addr(pAddress);
	        DWORD nMySubMask = inet_addr("255.255.255.0");
	        DWORD nTemp1 = nMyIP & nMySubMask;
	        DWORD nTemp2 = nMySubMask ^ 0xFFFFFFFF;
	        dwIP = nTemp1 + nTemp2;
	        //dwIP = nMyIP;
    	}    
   	 printf("+-----------------------------------+\n");
	return dwIP;
}

char *StartBroadcastWithHost(char *ip, int port)
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    struct hostent *host;
    char *send_data = "SYBROADCAST_V1.00.00";
    char hname[128];

    gethostname(hname, sizeof(hname));
    host = (struct hostent *) gethostbyname(ip);

    printf("+-----------------------------------+\n");	
    printf("%shostname: %s%s%s\n", KMAG, KCYN, hname, KNRM);
    printf("%saddress list:%s\n", KMAG, KNRM);
    int i;
    DWORD dwIP;

    for (i=0; host->h_addr_list[i]; i++) {
        char *pAddress = inet_ntoa(*(struct in_addr*)(host->h_addr_list[i]));
        printf("%s\t     %s%s\n", KCYN, pAddress, KNRM);

        DWORD nMyIP = inet_addr(pAddress);
        DWORD nMySubMask = inet_addr("255.255.255.0");
        DWORD nTemp1 = nMyIP & nMySubMask;
        DWORD nTemp2 = nMySubMask ^ 0xFFFFFFFF;
        dwIP = nTemp1 + nTemp2;
        //dwIP = nMyIP;
    }    
    printf("+-----------------------------------+\n");

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
	saveToLog("fault:socket(AF_INET, SOCK_DGRAM, 0)");
        printf("%sfault:socket(AF_INET, SOCK_DGRAM, 0)%s\n", KRED, KNRM);
        return 0;
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
	saveToLog("fault:setsockopt timeout");
        printf("%sfault:setsockopt timeout%s\n", KRED, KNRM);
        return NULL;
    }
    
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
	saveToLog("fault:setsockopt broadcast");
        printf("%sfault:setsockopt broadcast%s\n", KRED, KNRM);
        return NULL;
    }

    //memset(&server_addr, 0, sizeof(server_addr));   
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    //server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    server_addr.sin_addr.s_addr = dwIP;
    printf("%sSend To IP:%s%s %sPort:%s%d%s\n", KMAG, KGRN, inet_ntoa(server_addr.sin_addr), KMAG, KGRN, port, KNRM);
    bzero(&(server_addr.sin_zero), 8);
   
    int addr_len = sizeof(struct sockaddr);  
    char recv_data[1024];
    char *hostIP = NULL;

    while(1)
    {
        int iResult = sendto(sock, send_data, strlen(send_data), 0,
              (struct sockaddr *)&server_addr, addr_len);
              
    	if (iResult < 0) {
		saveToLog("fault:setsockopt broadcast");
        	printf("%ssend broadcast is fault! error:%d%s\n", KRED, iResult, KNRM);
    	}
	
	int bytes_read = recvfrom(sock, recv_data, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
    	recv_data[bytes_read] = '\0';
        if (bytes_read > 0)
        { 
    		hostIP = inet_ntoa(client_addr.sin_addr);
    		int hostPort = ntohs(client_addr.sin_port);

    		//printf("Server=>%s:%d\n", hostIP, hostIP);
    		//printf("Server Say=>%s\n", recv_data);
                break;    
    	}else{
		saveToLog("Server timeout:3 sec");
                printf("%sServer timeout:%s%d sec\n", KMAG, KNRM, TIMEOUT);
        }
	printf("+-----------------------------------+\n");
    } 
    saveToLog("Server IP");
    saveToLog(hostIP);
    return hostIP;
}

//======================================================================================================
//======================================================================================================

char *StartBroadcast(int port)
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    struct hostent *host;
    char *send_data = "SYBROADCAST_V1.00.00";
   

    DWORD dwIP =  GetBoardHostIP();

 
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
	saveToLog("fault:socket(AF_INET, SOCK_DGRAM, 0)");
        printf("%sfault:socket(AF_INET, SOCK_DGRAM, 0)%s\n", KRED, KNRM);
        return 0;
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
	saveToLog("fault:setsockopt timeout");
        printf("%sfault:setsockopt timeout%s\n", KRED, KNRM);
        return NULL;
    }
    
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
	saveToLog("fault:setsockopt broadcast");
        printf("%sfault:setsockopt broadcast%s\n", KRED, KNRM);
        return NULL;
    }

    //memset(&server_addr, 0, sizeof(server_addr));   
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    //server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    server_addr.sin_addr.s_addr = dwIP;
    printf("%sSend To IP:%s%s %sPort:%s%d%s\n", KMAG, KGRN, inet_ntoa(server_addr.sin_addr), KMAG, KGRN, port, KNRM);
    bzero(&(server_addr.sin_zero), 8);
   
    int addr_len = sizeof(struct sockaddr);  
    char recv_data[1024];
    char *hostIP = NULL;

    while(1)
    {
        int iResult = sendto(sock, send_data, strlen(send_data), 0,
              (struct sockaddr *)&server_addr, addr_len);
              
    	if (iResult < 0) {
		saveToLog("fault:setsockopt broadcast");
        	printf("%ssend broadcast is fault! error:%d%s\n", KRED, iResult, KNRM);
    	}
	
	int bytes_read = recvfrom(sock, recv_data, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
    	recv_data[bytes_read] = '\0';
        if (bytes_read > 0)
        { 
    		hostIP = inet_ntoa(client_addr.sin_addr);
    		int hostPort = ntohs(client_addr.sin_port);

    		//printf("Server=>%s:%d\n", hostIP, hostIP);
    		//printf("Server Say=>%s\n", recv_data);
                break;    
    	}else{
		dwIP =  GetBoardHostIP();
                server_addr.sin_addr.s_addr = dwIP;

		saveToLog("Server timeout:3 sec");
                printf("%sServer timeout:%s%d sec\n", KMAG, KNRM, TIMEOUT);
        }
	printf("+-----------------------------------+\n");
    } 
    saveToLog("Server IP");
    saveToLog(hostIP);
    return hostIP;
}

