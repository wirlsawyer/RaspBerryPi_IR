void clearLog()
{
	FILE *fp = NULL;
 	fp = fopen("/home/pi/IR/log.txt", "w");
       	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
}

void saveToLog(char *msg)
{
	FILE *fp = NULL;
 	fp = fopen("/home/pi/IR/log.txt", "a+");
       	if (fp != NULL)
	{
		fprintf(fp, "%s\n", msg);
		fclose(fp);
		fp = NULL;
	}
}
