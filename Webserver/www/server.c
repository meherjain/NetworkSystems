#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
//#include <initialize.h>


#define BYTES 99999
	
void client_handle(int); 
int n;
char path[9999] = "\0";
char * ROOT;
FILE *fd1;
int newsocket;


void alarm_handler(void){     										// Handles the time out // 
  //close(newsocket);
  //printf("Buzz Buzz BUzz");
  //printf("Server is closing the Client \n");
  //getchar();
  exit(0);
}


int main (int argc, char *argv [])
{
	FILE* config_file;
	char *configfile;
	int port; 
	configfile = "ws.conf";
	char config_buffer[1024];
	config_file = fopen (configfile, "r");
	if (config_file<0){
		printf("Read Error");}
		while(fgets(config_buffer, sizeof config_buffer,config_file)!=NULL){		
		 if (strstr (config_buffer, "Listen")!=NULL)
		 {
			 char * portnum = strstr(config_buffer, " ");
			 port = atoi(portnum);
			// printf("%d\n", port); 
			 }}
			 
	struct sockaddr_in  serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);
	int clientlen = sizeof (clientAddr);
	
	
	int sock= socket(AF_INET, SOCK_STREAM, 0);
    //If this fails exit and print the error
    if (sock == -1) 
    {
        printf("Error:cannot create the socket");
        return 1;
    }
    //printf("SOCKET CREATED!");
     // printf("Server Runnign on port %d", ntohs(serverAddr.sin_port));
    
    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("Error cannot bind: Address in Use\n");
        return 1;
    }
      
   
    if (listen(sock, 10) == -1)
    {
        printf("Error:cannot listen");
         }
    //else
   //printf("\nListening on the port %d", ntohs(serverAddr.sin_port));
		
    
   while (1)
    {
		
		newsocket = accept(sock, (struct sockaddr *) &clientAddr, &clientlen);
		if (newsocket == -1)
		printf("Error Accepting the connection");
		
		
		
		//printf("\nConnection Accpeted....\n");
		int pid = fork();
		//printf("\n Fork ID : %d\n", pid);
		if (pid <0)
		printf("Error on Fork !!");
		
		if (pid == 0)
		{
			close(sock);  // child doesn't need Parent's Socket //
			client_handle(newsocket);
			exit(0);
			}		
	close(newsocket);		 // Parent doesn't need this //
		
	}
	close(sock);
	return 0;

}
 
   
   
   
   
 void client_handle(int newsocket)
   {
	  int recvd, indx, data_read;
	  char buffer[1024] = "\0";
	  int i,j=0;
	  char ZEROARRAY[1024] = {0};
	  char data[BYTES];
	  FILE *fp;	
	  char html_buff1[1024] = "<html><body><H1>";
	  char html_buff2[32] = "</H1><body></html>";
	  char req_buff[128] = "HTTP/1.1 ERROR 404 Not Found: ";
	  char req_buff1[128] = "HTTP/1.1 Error 501 Not Implemented: ";
	  char cont_type[1024] = "Content-Type: ";
	  char cont_len[1024] = "Content-Length: ";
      char temp1[] = "GET";
      char temp2[] = " HTTP";
	  char temp4[] = "Connection";
	  char *req, *req1,*req2,*req3;
	  int len;
	  char getrequest[100] = "\0";
	  char httprequest[100] = "\0";
	  char connrequest[100]= "\0";
	  char* rootdir;
	  
	  
	  
	  
	  struct itimerval timeout;
	  
	  signal(SIGALRM, (void (*)(int)) alarm_handler);
	   
	   
	  timeout.it_value.tv_sec = 10;
	  timeout.it_value.tv_usec = 0;
	  timeout.it_interval = timeout.it_value;
	 
      		
	   FILE*conf_file;
	   char *config_file;
	   config_file = "ws.conf";
	   char conf_buffer[1024];
	   char ext_buff[1024]="\0";
	   int counter = 0;
	   char ext1[10],ext2[10],ext3[10],ext4[10],ext5[10],ext6[10],ext7[10],ext8[10],ext9[10] = "\0";
	  
	  
	  
	  
	  // -------------------------- Read from Config File ------------------------------//
	   
		conf_file = fopen (config_file, "r");				
		if (conf_file<0){
		printf("Read Error");}
		
		while(fgets(conf_buffer, sizeof conf_buffer,conf_file)!=NULL){		
			if (strstr(conf_buffer,"DocumentRoot")!=NULL){
			rootdir = strstr(conf_buffer, " ");
			rootdir = rootdir+1;
			strncpy(path,rootdir,(strlen(rootdir)-1));
			}
		else if (strstr(conf_buffer, "DirectoryIndex")!= NULL)
		{char* homepage = strstr(conf_buffer, " ");
				//printf("%s\n",homepage);
				}	
							
		else if (conf_buffer[0] =='.' && counter == 0)
		{	for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++;}
			ext_buff[j]=' ';
			counter ++;}
		else if(conf_buffer[0] =='.' && counter == 1)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
			counter ++;}
		else if(conf_buffer[0] =='.' && counter == 2)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
			ext_buff[j] = conf_buffer[i];
			j++ ;}
			ext_buff[j]=' ';
			counter ++;}
		else if(conf_buffer[0] =='.' && counter == 3)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
				counter ++;}
		else if(conf_buffer[0] =='.' && counter == 4)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
			    counter ++;}
		else if(conf_buffer[0] =='.' && counter == 5)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
			counter ++;}
		else if(conf_buffer[0] =='.' && counter == 6)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
			    counter ++;}			
		else if(conf_buffer[0] =='.' && counter == 7)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
				counter ++;}	
			
		else if(conf_buffer[0] =='.' && counter == 8)
		{j++;
			for (i=1; conf_buffer[i] != ' ';i++){
				ext_buff[j] = conf_buffer[i];
				j++ ;}
				ext_buff[j]=' ';
				counter ++;}	
		
		}	// End of config File Reading //
		strcpy(ext1, strtok(ext_buff, " "));
		strcpy(ext2, strtok(NULL,  " "));
		strcpy(ext3, strtok(NULL, " "));
		strcpy(ext4, strtok(NULL, " "));
		strcpy(ext5, strtok(NULL, " "));
		strcpy(ext6, strtok(NULL,  " "));
		strcpy(ext7, strtok(NULL, " "));
		strcpy(ext8, strtok(NULL, " "));
		strcpy(ext9, strtok(NULL, " ")); 
	   
	  // printf("File Format Supported By server: %s %s %s %s %s %s %s %s %s\n\n", ext1,ext2,ext3,ext4,ext5,ext6,ext7,ext8,ext9);
	 
			
		//---------------------------------------------------------//
		
		while (1) {	
		
			   
	   recvd = recv(newsocket,buffer,sizeof buffer,0);		// Receiving the request from the client //
   
	   if (recvd ==0 || recvd == -1)
		printf("receive error\n");
	   	   	   	         
	   
	 // fcntl(newsocket, F_SETFL, O_NONBLOCK);
		
		
		// signal(SIGALRM, alarm_handler);
		//printf("Initial Buffer Content: %s\n", buffer);
	
		// SORTING THE LONG REQUEST // REMOVING CARRIAGE RETURN AND NEW LINE //
		for (indx = 0; indx < recvd; indx++)
		{
			if (buffer[indx] == '\r' || buffer[indx] == '\n')
			{
				buffer[indx] = '#';}
		}
	   
		//printf("After Sorting: %s\n\n",buffer); 
		//------------------- Parsing to find GET,HTTP, Connection: Keep-alive --------------------//
	    char *buffer1 = malloc(strlen(buffer) * sizeof (char));
		strcpy	(buffer1,buffer);
		
		//printf("Copied Buffer: %s\n\n", buffer1);
			
		req  = strstr(buffer1,temp1);
		req1 = strstr(buffer1,temp2);
		
		//printf("GET FIND1:%s\n\n",req);
		//printf("GET FIND2:%s\n\n",req1);
		
		len = req1-req;
		//printf("Length with address:%d\n\n",len);
		if (req!=NULL){
		strncpy(getrequest,req,len);}
		printf("Get Request:%s\n\n",getrequest);
		
		
		//printf("Should Point to HTTP1:%s\n\n",buffer1);
		buffer1 = req1;
		//printf("Should Point to HTTP2:%s\n\n",buffer1);
		printf("HTTP:%s\n\n", req1);
		
		if (req1!=NULL){
		strncpy(httprequest,req1,9);}
		printf("HTTP Request:%s\n\n",httprequest);
		//printf("Should Point to HTTP3:%s\n\n",buffer1);
				
		req3 = strstr(buffer1,temp4);
		if (req3!=NULL){
		strncpy(connrequest,req3,22);
		buffer1 = req3;}
		else{connrequest[0] = '\0';}
		//printf("Connection Request:%s\n\n",connrequest);
		
		
		//----------------------------------------------------------------------//
		
	
		
					
		// ------------------------__Error Checking-----------------------------------//	
		
		if (strncmp(httprequest," HTTP/1.0",9) !=0 && strncmp(httprequest, " HTTP/1.1", 9)!=0 )
	   
			{
			write(newsocket, "<html><body><H1>Error 400 Bad Request: Invalid HTTP Version</H1></body></html>",78);
			exit(0);
	  }
	  	  
		else if(strncmp(getrequest,"GET",3)){
			
			write(newsocket,"<html><body><H!>Error 400 Bad Request: Invalid Method </H1></body></html>", 73);
				exit(0);
			
				}
		else 
		{	    	    
	   	// If nothing is there then open default page //
						
		if (!strncmp (getrequest, "GET /\0", 6))
		{
			strncpy (getrequest, "GET /index.html", strlen("GET /index.html"));
			}
			
			//printf("Client Request:  %s\n", buffer);
		
		j =0;	
		// Get The URL of requested File in the directory //  //( After the GET Command)
		for (i =4; i<strlen(getrequest); i++)
		{
		ZEROARRAY[j] = getrequest[i];
		j++;
		}
		//printf("\nRequested File Path: %s\n", ZEROARRAY);
		
		char* url;
		url = strstr(ZEROARRAY, ".");
		//printf("\n\nURL %s",url);
		/*
		if (strcmp((url+1),"?") || strcmp((url+1), " ") )
		{
			write(newsocket, "<html><body><H1>HTTP/1.1 400 Bad Request: Invalid URI</H1></body></html>",72);
			exit(0);
			}
*/

		
		
				
		//ROOT = getenv("PWD");			// Get Present Working Directory (PWD) as root //
		//ROOT = rootdir;
		//strcpy(,rootdir);
		
		//printf("ROOT PATH TAKEN FROM FILE:%s\n\n",ROOT);
	
		//strcpy(path, root_buffer);

		//printf("\nFINAL Path:%s\n%d\n\n",path,strlen(path));
      
		strcat(path,ZEROARRAY);   // add the requested file path to the PWD //		
		//printf("\nFull File Path:%s", path);
				
		int jj;
		//----------Supporterd Extension Check---------------//
			for (i= 4;i<strlen(buffer); i++)
			{
				if (ZEROARRAY[i] == '.')
			 {
				 jj=i+1;
				 }
			 
			}
		//printf("\n*******%d*******\n", jj);  //char after 1st '.' appears //
	
		char temp_buff2[4] ={0};
		int kk = 0;
		
		for (i =jj; i<strlen(buffer);i++)
		{
		temp_buff2[kk]= ZEROARRAY[i];
		//printf("%c", ZEROARRAY[i]);
		kk++;
		}
	
		//printf("\nRequested File Format :%s File Format Leng:%d\n",temp_buff2,strlen(temp_buff2));
		
		
		strcat(req_buff1,path);
		strcat(html_buff1,req_buff1);
		strcat(html_buff1,html_buff2); 
		
		//printf("%d")
		
		
		
		
		
		if (strcmp (temp_buff2, ext1)!=0 && strcmp(temp_buff2, ext2)!=0 && strcmp(temp_buff2, ext3)!=0 && strcmp(temp_buff2,ext4)!=0 && strcmp(temp_buff2,ext5)!=0 && strcmp(temp_buff2,ext6)!=0 && strcmp(temp_buff2,ext7)!=0 && strcmp(temp_buff2,ext8)!=0 && strcmp(temp_buff2,ext9)!=0 && strcmp (temp_buff2, "html")!=0 && strcmp(temp_buff2, "txt")!=0 && strcmp(temp_buff2, "png")!=0 && strcmp(temp_buff2,"gif")!=0 && strcmp(temp_buff2,"jpg")!=0 && strcmp(temp_buff2,"css")!=0 && strcmp(temp_buff2,"js")!=0 && strcmp(temp_buff2,"ico")!=0 && strcmp(temp_buff2,"htm")!=0)
		{ write (newsocket,html_buff1 ,sizeof(html_buff1));
		  write(newsocket, "\n<html><body><H1>Only HTML, PNG, JPG, GIF Extensions are supported </H1></body></html>",84);
			
				//printf("\n----1 File Extension Not Found----\n");	
				} 
				
		else
		{
			//-------------Formatting Messages to send to  the client---------------//
		char html_buff1[1024] = "<html><body><H1>";
		char html_buff2[32] = "</H1><body></html>";
		
		strcat(req_buff,path);
		strcat(html_buff1,req_buff);
		strcat(html_buff1,html_buff2);   
		
		
		FILE *fp1 = fopen(path,"r");
		if (fp1<1){
		printf("Error in Opening File");
          //  write(newsocket, "<html><body><H1>Error 404:File Not Found</H1></body></html>",59);
		  write(newsocket, html_buff1,sizeof(html_buff1));}
		
		fseek(fp1, 0L, SEEK_END); // Finding the Content size of the file to send -----------//
		int sz = ftell(fp1);
		//fseek(fp, 0L, SEEK_SET);
		//printf("\nSize of the file Sent: %d\n", sz);
		
		char tempbuff1[10] = "\n";
		char tempbuff[10] = "\n\n";
		char size[10];
		strcat(cont_type, temp_buff2);
		strcat(cont_type, tempbuff1);
		
		snprintf(size, 10 ,"%d",sz);
		
		strcat(cont_len, size);
		strcat(cont_len, tempbuff);
		
		
		
		
		
		
		
		
		//printf("Temp BuFF %s", tempbuff);
		
		//printf("\nContent Type Sent: %s\n", cont_type);
		//printf("\nContent Length Sent: %s\n", cont_len);
	
	//------------------------------------Sending requested Data--------------------//
							
	 if ( (fp=open(path, O_RDONLY))!=NULL )    //FILE FOUND
                {
                   
                  write(newsocket,"HTTP/1.0 200 OK\n", 17);
                  write(newsocket,cont_type,strlen(cont_type));
				  write(newsocket,cont_len,strlen(cont_len));
				   //write()
                    
				}
                   else {
          //  write(newsocket, "<html><body><H1>Error 404:File Not Found</H1></body></html>",59);
				   
				   write(newsocket, html_buff1,sizeof(html_buff1));
				   }            
                   
                    while ( (data_read=read(fp, data, BYTES))>0 )
                    {
                        send (newsocket, data, data_read,0);
                        if(send == -1){
							write(newsocket, "<html><body><H1>HTTP/1.1 500:Internal Server Error:Cannot allocate memory</H1></body></html>",92);
				   
							}  
					}
					
					if (!strcmp(connrequest,"Connection: Keep-alive"))
					{
						setitimer(ITIMER_REAL,&timeout,NULL);
						
						}
						else {
							write(newsocket,"No Persistent Connection, Socket Closed\n\n",sizeof("No Persistent Connection, Socket Closed\n\n"));
							exit(0);}
					
					
		}
 
	  
	  
	  
	  }
	  // close(newsocket);
	  
	  
	  
	
	 
	 // timeout.it_value.tv_sec = 10;
	  //timeout.it_value.tv_usec = 0;
	
	
	
	//free(buffer1);
	}  
	  
}
		
		
		

	   
	   
	  
		
	  
	  
	  

   
   
   




    
