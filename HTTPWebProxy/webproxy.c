#include <stdio.h>
#include <sys/types.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


#define Length 9999999 

void client_handle(int,int,char*);
int cachecheck(char*,int,int);


struct sockaddr_in serverAddr; /* client addr   */
struct sockaddr_in clientAddr; /* server addr   */
struct sockaddr_in hostAddr;
struct hostent *he;            /* Gethostbyname */
struct in_addr **addr_list;


int sock,newsocket,hostsock,hostsock1;
int clientlen,recvd;
unsigned int i,n,k,check;
char recv_buffer[Length] = "\0";
char req_method[10],URL[512],http_method[10] = "\0";
char get_error[]         = "<html><body><H!>Error 400 Bad Request: Invalid Method </H1></body></html>";
char http_error[]        = "<html><body><H1>Error 400 Bad Request: Invalid HTTP Version</H1></body></html>";
char host_error[10];
char temp_host[Length]   = "\0";
char host[Length]        = "\0";
char host_req[Length]    = "\0";
char buffer[Length], readbuff[Length]  = "\0";
      							//= "/home/meher/Desktop/hTTPProxyServer/cache/";
char* req,*req1,*req2,*p;
char* path;
size_t urlen,conread;
FILE *fp;
FILE *fp1;
unsigned char c[MD5_DIGEST_LENGTH];
char buff[MD5_DIGEST_LENGTH]="\0";
char filename[50] = "\0";
char prefetch[300][100];
char linkpath[300][100];
char str[100] = "\0";



int main (int argc, char* argv[]) 
{


	char *cache_dir;
	char cmd[200] = "\0";

	if (argc <3)
	{
		printf("Error in usage\nCorrect Usage: ./object_file port number Cache time out\n");
		exit(1);
	}


	int PORT = atoi(argv[1]);
	int CACHE_TIMER = atoi(argv[2]);
	
	
	cache_dir = getenv("PWD");
	strcat(cache_dir,"/cache/");
	//printf("cache_dir:%s",cache_dir);
	
	
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
	
		
	while (1)
	{	
		newsocket = accept(sock, (struct sockaddr *) &clientAddr, &clientlen);
		if (newsocket == -1) printf("Error Accepting the connection");
	
		//printf("Connection Accepted....\n");
		int pid = fork();
		//printf("\n Fork ID: %d\n", pid);
		if (pid<0)
		printf("Error on Fork !!");
		if (pid ==0)
		{
			close(sock);
			client_handle(newsocket,CACHE_TIMER,cache_dir);
			exit(0);
		}
		close(newsocket);
	}	// While 1 //
	close(sock);
	return 0;
	
} // Main
	


	
void client_handle(int newsocket,int CACHE_TIMER,char *cache_dir)
{

	//char *md5 = "\0";

	//while(1)
	//{
		recvd = recv (newsocket,recv_buffer,sizeof(recv_buffer),0);
		if (recvd < 0)
		{
			printf("Error in receiving");
		}


		//printf("Received Buffer:%s\n",recv_buffer);

		/*---------_Parsing the Client Request--------*/
		sscanf(recv_buffer,"%s %s %s",req_method,URL,http_method);

		//printf("Req Method:%s\n",req_method);
		//printf("URL:%s\n",URL);
		//printf("HTTP Method:%s\n",http_method);

		/*----------------------------------------------*/


		if (strncmp(req_method,"GET",3))
		{
			write(newsocket,get_error,strlen(get_error));
			exit(1);	
		}

		else if (strncmp(http_method,"HTTP/1.0",8))
		{
		 	write(newsocket,http_error,strlen(http_error));
		 	exit(1);
		}

		else
		{
	
			/*----------------Parsing the request ---------------*/
			req = strstr(URL,"//");
			req = req+2;

			for (i=0;i<strlen(req);i++)
			{
				if (req[i] == '/')
					break;			
				host[i] = req[i];
			}

			//printf("Host:%s\n", host);

			path = strstr(req,"/");
			//printf("Path:%s\n",path);


			//-------Calculating md5hash of the URL to cache it ------//
			MD5_CTX mdContext;
			MD5_Init(&mdContext);
  
    			MD5_Update (&mdContext, URL, strlen(URL));
   			MD5_Final (c, &mdContext);
			for (i = 0; i< MD5_DIGEST_LENGTH; i++)
			{
				//printf("Hash Value: %02x",c[i]);
				sprintf(&buff[2*i],"%02x", c[i]);
			}
			
			//printf("Hash Value1:%s\n", buff);
			
			sprintf(filename,"%s.html",buff);
			//printf("HashFileName:%s\n",filename);
				
			
			strcat(cache_dir,filename);
			//printf("cache:%s\n",cache_dir);
			
			check = cachecheck(cache_dir,newsocket,CACHE_TIMER);   // Check Whether File is present in cache (return 2 if it is or 3 if it is not)
			
			
			//printf("Cache Check = %d\n",check);
			
			if (check==3)
			{ 
				fp = fopen(cache_dir,"ab");
						
				//---------------Resolving the Remote Host and connecting to it ------------------//		
				he = gethostbyname(host);   // get tbrowserhe host info
				if (!he)
				{
					printf("\nHost Lookup Failed\n");
					exit(-1);
				}

				//printf("h_name:%s\n",he->h_name);

				//printf("IP Address: %s\n",inet_ntoa(*((struct in_addr *)he->h_addr)));


				/*---------- Connecting to remote host ------------*/

				bzero((char*)&hostAddr,sizeof(hostAddr));


				hostAddr.sin_family = AF_INET; // Protocol Family
				hostAddr.sin_port = htons(80); // Port number
				bcopy((char*)he->h_addr,(char*)&hostAddr.sin_addr.s_addr,he->h_length);

				if((hostsock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
				{
			   		printf("ERROR: Failed to obtain Socket Descriptor");
			   		exit(1);
				}
				else 
			   		//printf("[Server] Obtaining socket descriptor successfully.\n");
			   		
			   	
			   	hostsock1=connect(hostsock,(struct sockaddr*)&hostAddr,sizeof(struct sockaddr));
			   	if(hostsock1 < 0)
			   	printf("Error in Connecting Remote Host");
			   	//printf("Host Connect:%d\n",hostsock1);
			   	sprintf(temp_host,"Connected to %s on IP Address- %s",host, inet_ntoa(*((struct in_addr *)he->h_addr)));
			   	
			   	//printf("%s\n",temp_host);
			   	
			   	
			   	if (strlen(path) !=0)
			   	sprintf(host_req,"GET %s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",path,http_method,host);
			   	else
			   	sprintf(host_req,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",http_method,host);
			   	
			   	//printf("Host:%s\n",host_req);

				n=send(hostsock,host_req,strlen(host_req),0);

		    		if (n<0)
		    		{
		    			printf("Error in Sending the request to remote host");
		    		}

				else
				{
					do
					{
						bzero((char*)recv_buffer,Length);
						n=recv(hostsock,recv_buffer,sizeof recv_buffer,0);
						//printf("Content Length:%d\n", n);
						k = k+n;
						fwrite(recv_buffer,1,n,fp);
						//printf("Content Return:%s\n",recv_buffer);
						if(!(n<=0))
						send(newsocket,recv_buffer,n,0);
					}while(n>0);
					//printf("Total Content:%d\n",k);
				}

				fclose(fp);
				
				fp = fopen(cache_dir,"rb");      // Prefetching the Embedded links in the .html file // 
				fread(readbuff,1,Length,fp);
				fclose(fp);
				p = &readbuff[0];
				
				
				i =0;	
				while (strstr(p,"<a href=\"")!=NULL)
				{
	
					req1 =strstr(p,"<a href=\"");
					req1 = req1 + strlen("<a href=\"");
					req2 = strstr(req1,"\"");

					strncpy(prefetch[i],req1,(req2-req1));
					//printf("%s\n",prefetch[i]);
					if (prefetch[i][0] == '/' && prefetch[i][1] != '/')
					{
						strncat(str,URL,(strlen(URL)-1));
						strcat(str,prefetch[i]);
						strcpy(linkpath[i],str);
						bzero(str,sizeof str);
						//printf("Links to Prefetch:%s\n",linkpath[i]);
						
					}
	
					else
					{
						strcpy(linkpath[i],prefetch[i]);
						//printf("Links to Prefetch:%s\n",linkpath[i]);
					}
					
					p = p + (req1-p);
					i++;
				}
					
					
				
				
			}   // Cache Request // 
		 
		} 	// Error Handling 
	 
	close(hostsock);
	close(newsocket);
}



/*---------------------------Cache Check-----------------------*/
//It returns 2 if cache is present and valid, 3 otherwise//
/*--------------------------------------------------------------*/


int cachecheck(char* cache_dir, int newsocket, int CACHE_TIMER )
{
	
	char filetime[50] = "\0";
	char currtime[50] = "\0";
	int timemod,timecur;
	struct stat buf;
	stat(cache_dir, &buf);
	//printf("CacheDir:%s\n",cache_dir);

	if( access(cache_dir, F_OK ) != -1 )
	{
   		//printf("Cache Present\n");
   		strcpy(filetime,ctime(&buf.st_mtime));
		//printf("TimeMod:%s\n",filetime);

		timemod = ((filetime[11] - '0') * 10 + (filetime[12] - '0')) * 3600 + ((filetime[14] - '0') * 10 + (filetime[15] - '0')) * 60 + ((filetime[17] - '0') * 10 + (filetime[18] - '0'));


		time_t t;
		time(&t);
		strcpy(currtime,ctime(&t));

		//printf("Time:%s\n",currtime);


		timecur = ((currtime[11] - '0') * 10 + (currtime[12] - '0')) * 3600 + ((currtime[14] - '0') * 10 + (currtime[15] - '0')) * 60 + ((currtime[17] - '0') * 10 + (currtime[18] - '0'));	

	
		if ((timecur-timemod)>CACHE_TIMER)
		{
			printf("Cache To Old, Connecting to the remote Host......\n");
			//remove(cache_dir);
			return 3;		
		}
	
		
		   				
		printf("Fetching the webpage from the cache........\n\n");
		fp1 = fopen(cache_dir,"rb");
		conread = fread(buffer,1,Length,fp1);
		//printf("Buffer Length:%ld\n",strlen(buffer));
		//printf("Content Read Len:%ld\n",conread);
		send(newsocket,buffer,conread,0);
		fclose(fp1);
		close(newsocket);
		return 2;

	}

		else 
		{
		printf("Cache not Present, Connecting Remote Host......\n");
		return 3;
	
		}
}








