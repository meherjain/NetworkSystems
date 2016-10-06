#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <arpa/inet.h>
#include "md5_modcalc.h"     // Calculate the md5hash value and take mod with 4 //
#include "filewrite.h"	     // Write the file to the server //
#include "fileget.h"	     // Get the file from the server   //  

#define splitnum 4
#define Length 99999



int main (int argc, char * argv[])
{
     /* Socket Descriptors */
     
     	int newsocket1,newsocket2,newsocket3,newsocket4;
     	struct sockaddr_in serverAddr;
     
   /*  Read from Configuration file Parameters */ 
	//char *dfc_config = "dfc.conf";
	char username[Length] = "\0", password[Length] = "\0"; 
	int port[3],i;
	int kill1,kill2,kill3,kill4;
	char * file_parse;
	char config_buff[Length]; 
	char *filemod = ".mod.txt"; 
	FILE *fp1,*fpmod,*fpmod1,*fget1,*fget2,*fget3,*fget4;
	int length;
	char buffer[512]= "\0";
	char* req;


	fp1 = fopen(argv[1],"r");
	i = 0;
	while (fgets(config_buff,128,fp1) != NULL)
	{
		file_parse = strstr(config_buff,":");
		file_parse = file_parse + 1;
		if (i==0) port[i] = atoi(file_parse);
		if (i==1) port[i] = atoi(file_parse);
		if (i==2) port[i] = atoi(file_parse);
		if (i==3) port[i] = atoi(file_parse);
		if (i==4) strcpy(username,file_parse);
		if (i==5) {strcpy(password,file_parse); break;}
		i++;
	
	}
		
	fclose(fp1);	
	username[strlen(username)-1] = '\0';
	password[strlen(password)-1] = '\0';
     
     /*----------- DFS1 ----------------*/
     	serverAddr.sin_family = AF_INET;
     	serverAddr.sin_addr.s_addr = INADDR_ANY;
     	serverAddr.sin_port = htons(port[0]);
     
	newsocket1 = socket(AF_INET, SOCK_STREAM, 0);
	
	if (newsocket1 == -1)
		printf("Error Creating Socket");
				
	kill1 = connect(newsocket1, (struct sockaddr*) &serverAddr, sizeof(serverAddr)); 
	if (kill1<0)
	{
		printf("Error Connecting to DFS1\n");
		newsocket1 = -1;
	}
	serverAddr.sin_port = htons(port[1]);
	newsocket2 = socket(AF_INET, SOCK_STREAM, 0);
	
	if (newsocket2 == -1)
		printf("Error Creating Socket");
		
	kill2 = connect(newsocket2, (struct sockaddr*) &serverAddr, sizeof(serverAddr)); 
	
	if (kill2<0)
	{
		printf("Error Connecting to DFS2\n");
		newsocket2 = -1;
	}

 	serverAddr.sin_port = htons(port[2]);
	newsocket3 = socket(AF_INET, SOCK_STREAM, 0);
	
	if (newsocket3 == -1)
		printf("Error Creating Socket");
	
	kill3 = connect(newsocket3, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	
	if (kill3<0)
	{
		printf("Error Connecting to DFS3\n");
		newsocket3 = -1;
	}
	
	serverAddr.sin_port = htons(port[3]);
	newsocket4 = socket(AF_INET, SOCK_STREAM, 0);
	
	if (newsocket4 == -1)
		printf("Error Creating Socket");
			
	kill4 = connect(newsocket4, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	
	if (kill4<0)
	{
		printf("Error Connecting to DFS4\n");
		newsocket4 = -1;
	}
	
		
	int mod,size,sizeby4,modread;
	char in_buffer[Length]  = "\0";
	char filename[Length]   = "\0";
	char filename1 [Length] = "\0";
	char filename2 [Length] = "\0";
	char filename3 [Length] = "\0";
	char filename4 [Length] = "\0";
	char req_method[Length] = "\0";
	char sendbuff[Length]   = "\0";
	char listrecv[Length]   = "\0";
	FILE *fp;
	char cmd [Length]= "\0";
	
	

			
	//printf("Username:%s\nPassword:%s",username,password);
	
	printf("\n\n**************************** Distributed File Client (DFC) Running **************************\n\n");
	printf("DFC supports three option\n");
	printf("1. PUT/put Filename \t This option put the part of the file in to 4 server.\n\n");
	printf("2. GET/get Filename \t This option downloads the requested file from the servers.\n\n");
	printf("3. LIST/list \t \t This option displays the list of all the available files on the server.\n\n");
	
	

	fgets(in_buffer,sizeof in_buffer, stdin);

	sscanf(in_buffer, "%s %s",req_method,filename);
	//printf("Request Method:%s Filename: %s",req_method, filename);
	//int s = strcmp(req_method,"PUT")
	length = strlen(filename);


	//filename[strlen(filename)-1] = '\0'; 
	if (!strcmp(req_method,"PUT") || !strcmp(req_method,"put"))
	{
					
		mod = md5_modcalc(filename);    // Calculating mod 4 of MD5HASH value of the file.
		printf("Mod:%d\n",mod);
		fpmod = fopen(filemod,"a+");
		if (fp1 == NULL)
			printf("Error");
	
		sprintf(buffer,"%s%d\n",filename,mod);
		fprintf(fp1,buffer);
		fclose(fpmod);
				
		fp = fopen(filename, "r");
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		sizeby4 = size/4+1;
		//printf("\n%d",sizeby4);
	
		sprintf(cmd,"split -b %d -d -a 1 %s",sizeby4,filename);
		//printf("%s\n",cmd);
		system(cmd);
		int var =0;

		while (var<4)
		{
			sprintf(cmd,"cp x%d .%s.%d",var,filename,var+1);
			//printf("CMD:%s\n",cmd);
			system(cmd);
			sprintf(cmd,"rm x%d",var);
			system(cmd);
			var++;

		}

		sprintf(filename1, ".%s.1",filename);
		sprintf(filename2, ".%s.2",filename);
		sprintf(filename3, ".%s.3",filename);
		sprintf(filename4, ".%s.4",filename);	
		
		switch(mod) // Sending files to the server based on mod(MD5HASH,4) 
		{

			case 0:
				filewrite(newsocket1,filename1,filename2,username,password,req_method);
				filewrite(newsocket2,filename2,filename3,username,password,req_method);
				filewrite(newsocket3,filename3,filename4,username,password,req_method);
				filewrite(newsocket4,filename4,filename1,username,password,req_method);
				break;

			case 1:
				filewrite(newsocket1,filename4,filename1,username,password,req_method);
				filewrite(newsocket2,filename1,filename2,username,password,req_method);
				filewrite(newsocket3,filename2,filename3,username,password,req_method);
				filewrite(newsocket4,filename3,filename4,username,password,req_method);
				break;
	
			case 2:
				filewrite(newsocket1,filename3,filename4,username,password,req_method);
				filewrite(newsocket2,filename4,filename1,username,password,req_method);
				filewrite(newsocket3,filename1,filename2,username,password,req_method);
				filewrite(newsocket4,filename2,filename3,username,password,req_method);
				break;

			case 3:
				filewrite(newsocket1,filename2,filename3,username,password,req_method);
				filewrite(newsocket2,filename3,filename4,username,password,req_method);
				filewrite(newsocket3,filename4,filename1,username,password,req_method);
				filewrite(newsocket4,filename1,filename2,username,password,req_method);
				break;
	
		} // CASE END
		
	} // PUT END
	
	
	else if (!strcmp(req_method,"GET") || !strcmp(req_method,"get"))
	{
	
		sprintf(filename1, ".%s.1",filename);
		sprintf(filename2, ".%s.2",filename);
		sprintf(filename3, ".%s.3",filename);
		sprintf(filename4, ".%s.4",filename);	
		
		fpmod1 = fopen(filemod,"r");
		if (fpmod1==NULL)
		printf("ERROR in opening file");

		while(fgets(buffer,sizeof buffer,fpmod1)!=NULL)
		{
			req = strstr(buffer,filename);
			if (req!=NULL)
			{
				req = req +length;
				modread = atoi(req);
				//printf("%s\n",req);	
				//printf("Int:%d\n",modread);
				break;
			}
	
		}
			
		
		switch(modread) // Downloading the files from the server //
		{
			case 0:
				if (kill1==0)fileget(newsocket1,filename1,filename2,username,password,req_method);
				if (kill2==0)fileget(newsocket2,filename2,filename3,username,password,req_method);
				if (kill3==0)fileget(newsocket3,filename3,filename4,username,password,req_method);
				if (kill4==0)fileget(newsocket4,filename4,filename1,username,password,req_method);
				break;

			case 1:
				if (kill1==0)fileget(newsocket1,filename4,filename1,username,password,req_method);
				if (kill2==0)fileget(newsocket2,filename1,filename2,username,password,req_method);
				if (kill3==0)fileget(newsocket3,filename2,filename3,username,password,req_method);
				if (kill4==0)fileget(newsocket4,filename3,filename4,username,password,req_method);
				break;
	
			case 2:
				if (kill1==0)fileget(newsocket1,filename3,filename4,username,password,req_method);
				if (kill2==0)fileget(newsocket2,filename4,filename1,username,password,req_method);
				if (kill3==0)fileget(newsocket3,filename1,filename2,username,password,req_method);
				if (kill4==0)fileget(newsocket4,filename2,filename3,username,password,req_method);
				break;
			case 3:
				if (kill1==0)fileget(newsocket1,filename2,filename3,username,password,req_method);
				if (kill2==0)fileget(newsocket2,filename3,filename4,username,password,req_method);
				if (kill3==0)fileget(newsocket3,filename4,filename1,username,password,req_method);
				if (kill4==0)fileget(newsocket4,filename1,filename2,username,password,req_method);
				break;
		} // CASE END
		
	
		char temp1[Length] = "\0";
		char temp2[Length] = "\0";
		char temp3[Length] = "\0";
		char temp4[Length] = "\0";
		int modget;
		int del=0;
		
		strncpy(temp1,&filename1[1],strlen(filename1));
		strncpy(temp2,&filename2[1],strlen(filename2));
		strncpy(temp3,&filename3[1],strlen(filename3));
		strncpy(temp4,&filename4[1],strlen(filename4));
		
		fget1 = fopen(temp1,"r");
		fget2 = fopen(temp2,"r");
		fget3 = fopen(temp3,"r");
		fget4 = fopen(temp4,"r");
	
		if (fget1==NULL)
		{
			printf("File is Incomplete\n");
			close(newsocket1);
			close(newsocket2);
			close(newsocket3);
			close(newsocket4);
			exit(0);
		}
		else
		{
			fread(temp1,1,Length,fget1);
			fclose(fget1);
		}
	
		if (fget2==NULL)
		{
			printf("File is Incomplete\n");
			close(newsocket1);
			close(newsocket2);
			close(newsocket3);
			close(newsocket4);
			exit(0);
		}
		else
		{
			fread(temp2,1,Length,fget2);
			fclose(fget2);
		}
		
		if (fget3==NULL)
		{
			printf("File is Incomplete\n");
			close(newsocket1);
			close(newsocket2);
			close(newsocket3);
			close(newsocket4);
			exit(0);
		}
		else
		{
			fread(temp3,1,Length,fget3);
			fclose(fget3);
		}
		
		
		if (fget4==NULL)
		{
			printf("File is Incomplete\n");
			close(newsocket1);
			close(newsocket2);
			close(newsocket3);
			close(newsocket4);
			exit(0);
		}
		else
		{
			fread(temp4,1,Length,fget4);
			fclose(fget4);
		}
		
		strcat(temp1,temp2);
		strcat(temp1,temp3);
		strcat(temp1,temp4);
		
		
		fget1 = fopen(filename,"w");
		if (fget1 ==NULL)
		{ printf("ERROR IN OPENING THE GET FILE");}
		
		fwrite ( temp1,1,strlen(temp1),fget1);
		
		modget = md5_modcalc(filename);
		//printf("GETMOD:%d\n", modget);
		
		if (modget == modread)
			printf("Requested file is successfully downloaded from server....\n");
			printf("MD5CHECKSUM verification of the received file is successful...!!\n\n");
			
		while (del<4)
		{
			sprintf(cmd,"rm %s.%d",filename,del+1);
			system(cmd);
			del++;

		}
			
	} // GET END
		
			
	else if (!strcmp(req_method,"LIST") || !strcmp(req_method,"list"))
	{
	
		bzero(sendbuff,Length);
		char temp_filename[Length] = "\0";
		char temp_filesize[Length] = "\0";
		//char dirValues[Length]   = "\0";
		char dirvalues1[Length] = "\0";
		char dirvalues2[Length] = "\0";
		char dirvalues3[Length] = "\0";
		char dirvalues4[Length] = "\0";
		char array[50][50];
		char listunsorted[50][50];
		char listsorted[50][50];
		char listtoscan[50] = "\0";
		//char templist2[50];
		char tempbuff[Length] = "\0";
		char temp[50];
		char * token;
		int k =0, l = 0;
		int j,m;
		int complete = 0;
		char errormsg[] = "Invalid Username/Password, Please try again";
		strcpy(temp_filename,"HEY");
		strcpy(temp_filesize,"234");
		
		
		sprintf(sendbuff,"%s %s %s %s %s",temp_filename,temp_filesize,username,password,req_method);
		
		write(newsocket1,sendbuff,strlen(sendbuff)); // Sending Request to the server
		write(newsocket2,sendbuff,strlen(sendbuff));
		write(newsocket3,sendbuff,strlen(sendbuff));
		write(newsocket4,sendbuff,strlen(sendbuff));
		
		recv(newsocket1,listrecv,sizeof listrecv,0); // Receiving ACK
		recv(newsocket2,listrecv,sizeof listrecv,0);
		recv(newsocket3,listrecv,sizeof listrecv,0);
		recv(newsocket4,listrecv,sizeof listrecv,0);
				
		//printf("ACK RECEIVED:%s\n\n",listrecv);
	
		send(newsocket1,"ACK",3,0);	// Sending ACK
		send(newsocket2,"ACK",3,0);
		send(newsocket3,"ACK",3,0);
		send(newsocket4,"ACK",3,0);
		
		recv(newsocket1,dirvalues1,sizeof dirvalues1,0); // Receiving The directory list if Username and Password is correct
		recv(newsocket2,dirvalues2,sizeof dirvalues2,0);
		recv(newsocket3,dirvalues3,sizeof dirvalues3,0);
		recv(newsocket4,dirvalues4,sizeof dirvalues4,0);
		
		
if(!strcmp(dirvalues1, errormsg) || !strcmp(dirvalues2, errormsg) || !strcmp(dirvalues1, errormsg) || !strcmp(dirvalues1, errormsg))
	{
		printf("Server Replied with Invaid Username and Password\n\n");
		exit(1);
	}
		
		for (i=0;i<1;i++)
		{
			if (newsocket1 > 0)
			{
				strcpy(tempbuff,dirvalues1);
				break;
			}
			
			else if ( newsocket2 > 0)
			{
				strcpy (tempbuff,dirvalues2);
			  	break;
			}
		
			else if (newsocket3 > 0)
			{
				strcpy (tempbuff,dirvalues3);
			  	break;
			}
			 
			 else
				strcpy (tempbuff,dirvalues4);
	 		  	break;
	    }
		
		
		//printf("Copied:%s\n",tempbuff);
		
		strcat(dirvalues1,dirvalues2);
		strcat(dirvalues1,dirvalues3);
		strcat(dirvalues1,dirvalues4);
	
	
	/* Finding If Files are Downloadable from Server or Not, If not Printing File is incomplete */				
		token = strtok(tempbuff," ");
		
		while (token!= NULL)
		{
			//printf("%s\n", token);
			strncpy(array[k],&token[1],strlen(token));
			while (array[k][l] != '.')
			{	
				l++;
			}	
			strncpy(listunsorted[k],array[k],l);
			//printf("listunsorted:%s\n",listunsorted[k]);
			k++;
			token = strtok(NULL," ");
			l=0;
		}
		
		for (j=0; j<k-1;j++)
		{
			for (m =j+1;m<k;m++)
			{
				if (strcmp(listunsorted[j],listunsorted[m])>0)
				{
					strcpy(temp,listunsorted[j]);
					strcpy(listunsorted[j],listunsorted[m]);
					strcpy(listunsorted[m],temp);
				}
			}
		}
		
		m =1;
		for (j=0;j<k;j++)
		{
			if (strcmp(listunsorted[j],listunsorted[j+1]) == 0)
			{
				strcpy(listsorted[m-1],listunsorted[j]);
				//printf("%s\n",listsorted[m-1]);
				m++;
			}
		}
			
		printf("\nFile Present on the server\n");
		char message [100] = "\0";
		for (i=0;i<m-1;i++)
		{
			for (j =0;j<4;j++)
			{
				sprintf(listtoscan,".%s.txt.%d",listsorted[i],j+1);
				//printf(" %s\t%ld",listtoscan,strlen(listtoscan));
				if(strstr(dirvalues1,listtoscan)!=NULL)
				{
					complete++;
				}
			}
									
			if (complete == 4)
			sprintf(message,"%s.txt is Complete",listsorted[i]);
			else
			sprintf(message,"%s.txt is Incomplete",listsorted[i]);
			
			printf("%s\n\n", message);
			complete = 0;
		}
		
	} // END LIST


		close(newsocket1);
		close(newsocket2);
		close(newsocket3);
		close(newsocket4);

		return 0;

	
	
}
