#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <time.h>
#include "nectrmit.h"
#include "tcpClient.h"
#include "udpClient.h"
#define GPIO_PIN_OUT	2
#define BASEHZ		(560)
//gcc -o main *.c -lwiringPi 


int btn2[35] = {  
8, 1, 1, 1, 1,
1, 1, 1, 1, 3,
3, 3, 3, 3, 3,
3, 3, 1, 1, 1,
3, 1, 1, 1, 1,
3, 3, 3, 1, 3,
3, 3, 3, 70, 4 };
 
int btn_pulse2[35] = {  
15, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 
0, 0, 0, 0, 0,
0, 0, 0, 0, 15 };

void sendIR()
{
 	int i;
        for (i=0; i<35; i++)
	{		
		NECsend(GPIO_PIN_OUT, btn_pulse2[i], btn2[i]);		
         }
	NECsend(GPIO_PIN_OUT, 0, BASEHZ); //end
}

int main()
{
	if (wiringPiSetup() == -1)
    	{
       		printf("fault:wiringPiSetup\n");
      		return 0;
    	}
   	pinMode(GPIO_PIN_OUT, OUTPUT);

    	char *hostIP = NULL;
	char szbufIP[256];
	memset(szbufIP, 0, 256);

	FILE *fp;
	fp = fopen("./config.txt", "r");
        if (fp)
	{
		fscanf(fp, "%s", szbufIP);
		fclose(fp);	
		if (strlen(szbufIP)>0) hostIP = szbufIP;	
		printf("Load Config IP=%s\n", hostIP);
	}
	
	if (hostIP == NULL) hostIP = StartBroadcast(8090);
	printf("%sFind server:%s%s%s\n", KMAG, KGRN, hostIP, KNRM);
	

	while (1)
	{
		char *szRecv = RecvMessage(hostIP,  5000);
		printf("szRecv=%s\n", szRecv);
		if (szRecv == NULL)
		{
			sleep(1);
			printf("Server online\n");
		} else if ( strcmp(szRecv, "exit") == 0) {
			break;
		} else if ( strcmp(szRecv, "sendir") == 0){
			printf("szRecv=%s\n", szRecv);
			sendIR();
			sleep(1);
			sendIR();
			sleep(1);
			sendIR();		
		}
	}
	
   	printf("End!\n");
    	return 0;
}

