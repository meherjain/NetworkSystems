#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>


#define Length 99999

void client_handle(int, char*); 


int sock,newsocket,num,sin_size; 
int i =0;
char *dfs_config =	"dfs.conf";
char revbuf[Length]	    = "\0"; // Receiver buffer
char dirname[Length]    = "\0";
char dirname2[Length]   = "\0";
char filename1[Length]   = "\0";
char filename2[Length]  = "\0";
char filesize1[Length]   = "\0";
char filesize2[Length]   = "\0";
char username[Length]   = "\0";
char password[Length]   = "\0";
char usrname[Length]    = "\0";
char passwrd[Length]    = "\0";
char filebuffer[Length] = "\0";
char req_method[Length] = "\0";
char cmd [Length]       = "\0";
char cmd1 [Length]      = "\0";
char sendbuff[Length]   = "\0";
int clientlen,recvd;
double nbytes =0;
double nbytes1 =0;
char * ROOT;
char msg[] = "ACK";
	






int main (int argc, char *argv [])
{
	/* Defining Variables */
	struct sockaddr_in serverAddr; /* client addr */
	struct sockaddr_in clientAddr; /* server addr */
	int PORT = atoi(argv[2]);
	char parentdir[Length] = "\0";
	
	
	
	
	FILE* fconfig = fopen(dfs_config,"r");
	if (fconfig == NULL)
	{
		printf("Cannot Read from Config File ");
		exit(0);
	}
	
	while(fgets(filebuffer,sizeof filebuffer,fconfig)!=NULL)
	{
		if (i==0) strcpy(usrname,filebuffer);
		if (i==1)	strcpy(passwrd,filebuffer);
		i++;
		
	}
	 
	usrname[strlen(usrname)-1] = '\0';
	passwrd[strlen(passwrd)-1] = '\0';
	
	ROOT = getenv("PWD");					// GET Present Working Directory //
	strcpy(parentdir,argv[1]);
	
	/* Get the Socket file descriptor */
	   if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
	   printf("ERROR: Failed to obtain Socket Descriptor");
	   exit(1);
	}
	else 
	   printf("[Server] Obtaining socket descriptor successfully.\n");

	/* Fill the client socket address struct */ 
	serverAddr.sin_family = AF_INET; // Protocol Family
	serverAddr.sin_port = htons(PORT); // Port number
	serverAddr.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	bzero(&(serverAddr.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */
	if( bind(sock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) == -1 )
	{
	   printf("ERROR: Failed to bind Port.\n");
	   exit(1);
	}
	else 
	   printf("Binded tcp port %d sucessfully.\n",PORT);

	/* Listen remote connect/calling */ 
	if(listen(sock,10) == -1)
	{
	   printf("ERROR: Failed to listen Port.\n");
	   exit(1);
	}
	else
	   printf ("Listening on the port %d ....\n", PORT);
	   

	clientlen = sizeof(struct sockaddr_in);

	while(1)
	{
		newsocket = accept(sock, (struct sockaddr *) &clientAddr, &clientlen);
		if (newsocket == -1) printf("Error Accepting the connection");
	
		printf("Connection Accepted....\n");
		int pid = fork();
		//printf("\n Fork ID : %d\n", pid);
		if (pid <0)
		printf("Error on Fork !!");
		if (pid == 0)
		{
			close(sock);  			// child doesn't need Parent's Socket //
			client_handle(newsocket, parentdir);
			exit(0);
		}	// Child
		close(newsocket);
	} 							// While //
	close (sock);
	return 0;
}  // Main
	
	
	
	
	
	
	
	
	
	void client_handle(int newsocket, char* parentdir)
	{	
		int rcvd = recv(newsocket, revbuf,sizeof revbuf ,0);
		if (rcvd <= 0)
		{
			printf("No Data received from client, Exiting from the child Sever...\n");
			close(newsocket);
			exit(0);
		}

		//printf("Request from the Client:%s\n", revbuf);

		//bzero(filename,Length);

		send(newsocket,"ACK",3,0);    // Sending ACK to the client //
	
		sscanf(revbuf,"%s %s %s %s %s %s %s",filename1,filesize1,username,password,req_method,filename2,filesize2);
		int size1 = atoi(filesize1);
		int size2 = atoi(filesize2);

		bzero(revbuf,Length);
		recv(newsocket,revbuf,sizeof revbuf,0);
		//printf("ACK Blah Blah:%s\n",revbuf);
		bzero(revbuf,Length);
		//printf("%s %s\n\n", username,password);
		if (strcmp(username,usrname) || strcmp(password,passwrd) !=0)
		{
			printf("Invalid Username/Password, Sending to Client...\n");
			char err_msg[] = "Invalid Username/Password, Please try again";
			send(newsocket,err_msg,sizeof err_msg,0);
			close(newsocket);
			exit(0);
		}
	
		else
		{
	
			if (!strcmp(req_method,"PUT") || !strcmp(req_method,"put"))
			{
				send(newsocket,"ACK",3,0);
		
				strcpy(dirname,parentdir);					// Get Server Directory Name //
				//strcat(ROOT,dirname);
				//printf("Server Dire:%s\n", ROOT);
				strcat(dirname,username);
				strcat(dirname,"/");
				sprintf(cmd,"mkdir -p %s",dirname);


				//printf("Command:%s\n", cmd);
				system(cmd);

				sprintf(cmd1,"%s/%s%s", ROOT,dirname,filename1);
				//printf("Sprintf:%s\n",cmd1);

				//strcat(dirname,filename);
				//strcat(ROOT,"/");
				//strcat(ROOT,dirname);
				//printf("Final Root Dir:%s\n",ROOT);	


	
				bzero(revbuf,Length);
				FILE *fr = fopen(cmd1, "wb");

				if (fr)
				{
					//printf("receiving...\n");
					while (nbytes<size1)
					{
						nbytes1 = recv(newsocket,revbuf,sizeof revbuf,0);
						//printf("%s\n", revbuf);
						fwrite(revbuf,nbytes1,1,fr);
						send(newsocket,msg,sizeof(msg),0);
						nbytes = nbytes + nbytes1;
		
		
					}
	
					//printf("File Received\n");
					nbytes = 0;
					fclose(fr);
	
				}
				else	printf("Unable to Locate the file\n");





				/*-------------------Receiving the Second File ---------------------------*/


				//printf("File Name Read:%s File Name Length:%ld\n",filename2,strlen(filename2));
				bzero(cmd1,Length);
				sprintf(cmd1,"%s/%s%s", ROOT,dirname,filename2);
				//printf("Opening the file2:%s\n",cmd1);


				bzero(revbuf,Length);
				nbytes 	=	0;
				nbytes1   = 	0;
				fr = fopen(cmd1, "wb");
				

				if (fr)
				{
					//printf("Size of the file receiving:%d\n",size2);
					//printf("receiving...\n");
					while (nbytes<size2)
					{
						//printf("Socket Descriptor:%d\n",newsocket);
						nbytes1 = recv(newsocket,revbuf,sizeof revbuf,0);
						//printf("Bytes Received%f\n", nbytes1);
						fwrite(revbuf,nbytes1,1,fr);
						send(newsocket,msg,sizeof(msg),0);
						nbytes = nbytes + nbytes1;
		
		
					}
	
					//printf("File Received\n");
					nbytes = 0;
					fclose(fr);
	
				}
				else	printf("Unable to Locate the file\n");
			}  // PUT
		
		
		
		
			else if (!strcmp(req_method,"GET") || !strcmp(req_method,"get"))
			{
				
				send(newsocket,"ACK",3,0);
				bzero(revbuf,Length);
				recv(newsocket,revbuf,sizeof revbuf,0);
				
				strcpy(dirname,parentdir);	
				strcat(dirname,username);
				strcat(dirname,"/");
						
				bzero(cmd,Length);
				sprintf(cmd,"%s/%s%s",ROOT,dirname,filename1);
				
				
				FILE *fp4 = fopen(cmd,"r");
				
				
				if (fp4)
				{
					while (!feof(fp4))
					{
						bzero(sendbuff,Length);
						nbytes = fread(sendbuff,1,Length,fp4);
						
						if (send(newsocket,sendbuff,nbytes,0)<0)
						{
							printf("Error in sending the requested file");
							exit(1);
						}
						
						bzero(revbuf,Length);
						recv(newsocket,revbuf,Length,0);
						//printf("%s\n",revbuf);
						
						if (strcmp(revbuf,"ACK")!=0)
						{
							printf("Error in Receiving ACK");
						}
					}
					fclose(fp4);
				}
					else
					{
						printf("Unable to find the file...\n");
						exit(0);
					}
					
					
				
				
				
			/*-----------------------Sending 2nd Requested File ---------------------- */	
				
			
				
				bzero(cmd,Length);
				sprintf(cmd,"%s/%s%s",ROOT,dirname,filename2);
				fp4 = fopen(cmd,"r");
				
				
				if (fp4)
				{
					while (!feof(fp4))
					{
						bzero(sendbuff,Length);
						nbytes = fread(sendbuff,1,Length,fp4);
						
						if (send(newsocket,sendbuff,nbytes,0)<0)
						{
							printf("Error in sending the requested file");
							exit(1);
						}
						
						bzero(revbuf,Length);
						recv(newsocket,revbuf,Length,0);
						//printf("%s\n",revbuf);
						
						if (strcmp(revbuf,"ACK")!=0)
						{
							printf("Error in Receiving ACK");
						}
					}
					fclose(fp4);
				}
					else
					{
						printf("Unable to find the file...\n");
						exit(0);
					}
				
							
			
			}  // GET
		
		
			else if (!strcmp(req_method,"LIST") || !strcmp(req_method,"list"))
			{
		
				//printf("RequestedMethod:%s\n",req_method);
				
				char listdir1[Length] = "\0";
				char listdir2[Length] = "\0";
				char listdir3[Length] = "\0";
				char listdir4[Length] = "\0";
				char listdirname[Length] = "\0";
				char dirValues[Length]   = "\0";
				DIR *dir;
  				struct dirent *file;
  				int nn=0,ii=0;
									
				
				strcpy(listdirname,parentdir);
				sprintf(listdir1,"%s/%s%s",ROOT,listdirname,username);
				dir = opendir(listdir1);
				
				
				if (dir)
            		{
              		while ((file = readdir(dir)) != NULL)
              		{
               	 	if (file->d_type == DT_REG)
               	 	strcat(dirValues,strcat(file->d_name," "));
               	}
              		closedir(dir);
              		}
				
				
				
				send(newsocket,dirValues,sizeof dirValues,0);
				
			} // LIST
		
		
		}
		close(newsocket);
		
		
	}

	

