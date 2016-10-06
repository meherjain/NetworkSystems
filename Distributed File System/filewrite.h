#define Length 99999

void filewrite(int socket, char* filename1,char* filename2, char* username, char* password, char* req_method)
{
	
	
	int filesize1,filesize2,nbytes;
	char sendbuff[Length] = "\0";
	char recvbuff[Length] = "\0";
	
	
	FILE *fp2 = fopen(filename1,"r");
	fseek(fp2, 0L, SEEK_END);
	filesize1 = ftell(fp2);
	fseek(fp2, 0L, SEEK_SET);
	fclose(fp2);
	
	FILE *fp3 = fopen(filename2,"r");
	fseek(fp3, 0L, SEEK_END);
	filesize2 = ftell(fp3);
	fclose(fp3);
	
	
	sprintf(sendbuff,"%s %d %s %s %s %s %d",filename1,filesize1,username,password,req_method,filename2,filesize2);
	
	//printf("SendBuff:%s",sendbuff);
	
	write(socket,sendbuff,strlen(sendbuff));	
	
		
	recv(socket,recvbuff,sizeof recvbuff,0);
	//printf("ACK RECEIVED:%s\n", recvbuff);
	
	send(socket,"ACK",3,0);
	bzero(recvbuff,Length);
	
	recv(socket,recvbuff,sizeof recvbuff,0);
	
	
	if (strcmp(recvbuff,"Invalid Username/Password, Please try again") ==0)
	{
		printf("Server Replied with Invalid Username Password Message\n\n");
		//send(socket,"ACK",3,0);
		exit(0);
	}
	else
	{
	bzero(recvbuff,Length);
	
	
	
	fp2 = fopen(filename1,"r");
	if (fp2)
	{
		while (!feof(fp2))
		{
			bzero(sendbuff,Length);
			nbytes = fread(sendbuff,1,Length,fp2);
						
			if (send(socket,sendbuff,nbytes,0)<0)
			{
				printf("Error in sending the file");
				exit(1);
			}
	
	
			bzero (recvbuff,Length);
			recv(socket,recvbuff,Length,0);
			printf("%s\n",recvbuff);
			if (strcmp(recvbuff,"ACK")!=0)
			{
				printf("Error in receiving ACK");
				//exit(1);
			}
		
		}
		printf("File Sent.....\n");
		fclose(fp2);
	}
	else
	{
		printf("Unable to find the file...\n");
		exit(1);
		
	}
	
	
	
	/*	
	bzero(sendbuff,Length);	
	fp2 = fopen(filename2,"r");
	fseek(fp2, 0L, SEEK_END);
	filesize = ftell(fp2);
	fseek(fp2, 0L, SEEK_SET);
	
	
	
	sprintf(sendbuff,"%s %d",filename2,filesize);
	
	//printf("SendBuff:%s",sendbuff);
	
	write(socket,sendbuff,strlen(sendbuff));
	
	recv(socket,recvbuff,sizeof recvbuff,0);
	printf("ACk:%s\n", recvbuff);*/
	
	//printf("2nd File:%s\n\n",filename2);
	
	fp3 = fopen(filename2,"r");
	

	
	
	bzero(recvbuff,Length);
	
	if (fp3)
	{
		while (!feof(fp3))
		{
			//printf("Entered the Loop\n");
			bzero(sendbuff,Length);
			nbytes = fread(sendbuff,1,Length,fp2);
			//printf("FileRead:%s",sendbuff);
			//printf("Socket Descriptor:%d\n",socket);
			if (send(socket,sendbuff,nbytes,0)<0)
			{
				printf("Error in sending the file");
				exit(1);
			}
			//printf("Socket Descriptor:%d\n",socket);
			//printf("Reached here\n");
			bzero (recvbuff,Length);
			recv(socket,recvbuff,Length,0);
			//printf("Reached here\n");
			//printf("Bytes Received:%d\n",bytesrecvd);
			if (strcmp(recvbuff,"ACK")!=0)
			{
				printf("Error in receiving ACK\n");
				exit(1);
			}
		
		}
		printf("File Sent.....\n");
		fclose(fp3);
	}
	else
	{
		printf("Unable to find the file...\n");
		exit(1);
		
	}
	
	}
	
	
	
	
	
	
	


}

