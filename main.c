#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
//#include <lirc/lirc_client.h>
#include <time.h>
//#include <fcntl.h>
#include "udpClient.h"
#include "tcpClient.h"
//#include "log.h"
#include <pthread.h>

#define GPIO_PIN_IN	1
#define BASEHZ		560  	//NEC
//gcc -o IRTest *.c -lwiringPi -pthread


int btn1[32] = {   8, 1, 1, 1, 1,
               		3, 3, 1, 1, 3,
               		3, 3, 3, 1, 1,
              		3, 3, 3, 3, 3,
               		1, 3, 1, 1, 3,
               		1, 1, 1, 3, 1,
               		3, 3};

int btn1_press1[2] = { 4, 172 };

int btn2[32] = {  8, 1, 1, 3, 3,
               		    1, 3, 3, 1, 1,
               		    1, 1, 3, 1, 3,
              		    3, 1, 1, 3, 3,
               		    1, 3, 1, 3, 1,
               		    3, 1, 1, 3, 1,
               		    3, 1};
int btn2_press1[2] = { 4, 172 };

void sendToPC(char *hostIP, char *msg)
{
	 printf("%sRev IR and send message to server%s\n", KCYN, KNRM);
   	 SendMessage(hostIP, 5000, msg );
}

void AliveThread(void *arg)
{
       char *IP = (char *)arg; 
	while (1)
	{
		sendToPC(IP, "alive");
		printf("this is a thread\n");
		sleep(5);
	}
}

int main()
{
	clearLog();
	saveToLog("Start");

    	if (wiringPiSetup() == -1)
    	{
       		printf("fault:wiringPiSetup\n");
		saveToLog("fault:wiringPiSetup");
      		return 0;
    	}

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
	saveToLog("Broadcast IP");
	saveToLog(hostIP);

    	int in_invoke = 0;    
    	SendMessage(hostIP, 5000, "start from raspberrypi");
 
	pthread_t  id;
      	int ret = pthread_create(&id, NULL, (void *)AliveThread, hostIP);
	if (ret != 0)
	{
		printf("Create pthread fault\n");
		saveToLog("Create pthread fault");
	}
    	//--------------------------------------------------
   	 pinMode(GPIO_PIN_IN, INPUT);

  	clock_t start, stop;
   	start = clock();
    	stop = clock(); 
    	int i, j;
   	int count = -1;
    	int fBuf[64];
	int iDurSpaceBuf[64];
    	memset(fBuf, 0, sizeof(fBuf));
	memset(iDurSpaceBuf, 0, sizeof(iDurSpaceBuf));
	
    	while (1)
   	{
        	start = clock();
         	float durDown = ((float)(start-stop)/CLOCKS_PER_SEC);
         	while (digitalRead(GPIO_PIN_IN) == 0)
         	{
               		start = clock();              
        	}
         	float durPulse = ((float)(start-stop)/CLOCKS_PER_SEC); 
         	while (digitalRead(GPIO_PIN_IN) == 1)
         	{
               		stop = clock();
        	 }
         	float durSpace = ((float)(stop-start)/CLOCKS_PER_SEC); 
	 
	 	
         	
        	int diPulse = durPulse*1000000 > 1000 ? 1 : 0;
        	int diSpace = (durSpace*1000000 + BASEHZ/2)/ BASEHZ;

		
 		if (diPulse == 1 && diSpace == 8)
		{
			count = 0;
		}
		
		if (count != -1)
		{
         		fBuf[count] = (int)diSpace;
			iDurSpaceBuf[count] = (int)(durSpace*1000000);
         		count++;
         		count%=64;
			if (count == 32) 
                        {
				
                               	char szMsg[33];
                                memset(szMsg, 0, sizeof(szMsg));
				szMsg[32] = '\0';	
   				for (i=0; i<32; i++)
				{
					sprintf(szMsg, "%s%d", szMsg, fBuf[i]);							
				}//end for
				//printf("\n");
				//printf("%s\n\n", szMsg);
				sendToPC(hostIP, szMsg);
				//------------------------------------
				char szDurSpaceMsg[512];
                                memset(szDurSpaceMsg, 0, 512);
				for (i=0; i<32; i++)
				{
					if (i==0)
					{
						sprintf(szDurSpaceMsg, "%d", iDurSpaceBuf[i]);	
					}else{
						sprintf(szDurSpaceMsg, "%s,%d", szDurSpaceMsg, iDurSpaceBuf[i]);				
					}

				}//end for
				sendToPC(hostIP, szDurSpaceMsg);

				count = -1;				
				memset(fBuf, 0, sizeof(fBuf));				
			}// end if
		}// end if
   	}// end while

   	printf("End!\n");
    	return 0;
}

