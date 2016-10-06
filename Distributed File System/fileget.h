#define Length 99999

void fileget(int socket,char* filename, char* filename2, char* username, char* password, char* req_method)
{
	char sendbuff[Length]  = "\0";
	char recvbuff[Length]  = "\0";
	char temp[Length]      = "\0";
	int nbytes1 = 0,nbytes = 0;
	FILE *fi = fopen(filename,"r");
	fseek(fi, 0L, SEEK_END);
	int filesize = ftell(fi);
	fclose(fi);

	FILE *fi2 = fopen(filename2,"r");
	fseek(fi2,0L, SEEK_END);
	int filesize2 = ftell(fi2);
	fclose(fi2);

	sprintf(sendbuff,"%s %d %s %s %s %s %d",filename,filesize,username,password,req_method,filename2,filesize2);
	//printf("1st Get Request:%s\n",sendbuff);
	write(socket,sendbuff,strlen(sendbuff));
	
		
	recv(socket,recvbuff,sizeof recvbuff,0);
	//printf("ACK RECEIVED:%s\n", recvbuff);
	
	send(socket,"ACK",3,0);
	
	bzero(recvbuff,Length);
	
	recv(socket,recvbuff,sizeof recvbuff,0);
	
	//printf("Received From the Server:%s\n",recvbuff);
	
	if (strcmp(recvbuff,"Invalid Username/Password, Please try again") ==0)
	{
		printf("Server Replied with Invalid Username Password Message\n\n");
		send(socket,"ACK",3,0);
		close(socket);
		exit(0);
	}
	
	else
	{
		
		send(socket,"DUDE",4,0);	
		
		
		
		bzero(recvbuff,Length);
		strncpy(temp,&filename[1],strlen(filename));
		
		
		
		
		fi = fopen(temp,"wb");
		
		if (fi)
		{
			while(nbytes<filesize)
			{
				nbytes1 = recv(socket, recvbuff, sizeof recvbuff,0);
				fwrite(recvbuff,nbytes1,1,fi);
				send(socket,"ACK",3,0);
				nbytes = nbytes + nbytes1;
			}
			
			//printf("File Received\n");
			nbytes = 0;
			fclose(fi);
			
		}
		else printf("Unable to open the file\n");
		
		
		
		
		strncpy(temp,&filename2[1],strlen(filename2));
		
		bzero(recvbuff,Length);
		
		
		
		fi2 = fopen(temp,"wb");
		
		if (fi2)
		{
			while(nbytes<filesize2)
			{
				nbytes1 = recv(socket, recvbuff, sizeof recvbuff,0);
				fwrite(recvbuff,nbytes1,1,fi2);
				send(socket,"ACK",3,0);
				nbytes = nbytes + nbytes1;
			}
			
			printf("File Received\n");
			nbytes = 0;
			fclose(fi2);
			
			
		}
		else printf("Unable to open the file\n"); 
		
		
		
	} // PASSWORD USERNAME CHECK ..

}
