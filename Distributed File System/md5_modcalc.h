

int md5_modcalc(char *filename1)
{
   unsigned char c[MD5_DIGEST_LENGTH];
   char *filename = filename1;
   char buff[MD5_DIGEST_LENGTH]="\0";
   //char *buff;
   int i,digit,mod =0 ;
   //printf("Filename MD5_Hash:%s\n\n", filename);
  
   FILE *fp = fopen (filename,"rb");
   MD5_CTX mdContext;
   int bytes;
   unsigned char data[1024];
   
   if (fp == NULL)
   {
	printf("%s Cannot Open File\n", filename);
	exit(1);
	
   }
		
   MD5_Init(&mdContext);
   while ((bytes = fread(data,1,1024,fp)) != 0)
   MD5_Update (&mdContext, data, bytes);
   MD5_Final (c, &mdContext);
	for (i = 0; i< MD5_DIGEST_LENGTH; i++)
	{
		//printf("Hash Value: %02x",c[i]);
		sprintf(&buff[2*i],"%02x", c[i]);
	}
			
			
	//printf("Hash Value1:%s\n", buff);
				


	/* Convert Hex to integer  */ 
	for (i = 0; i<2*MD5_DIGEST_LENGTH; i++)
	{
		if (buff[i] >= '0' && buff[i] <= '9')
		{
			digit = buff[i] - '0';
			//printf("digit:%d Char:%c\n\n", digit, buff[i]);
		}
		else if (buff[i] >= 'a' && buff[i] <= 'f')
		{
			digit = buff[i] - 'a' +10;
			//printf("digit:%d Char:%c\n\n", digit, buff[i]);
		}
		else 
			digit = buff[i] - 'A' + 10;
			mod = ((mod*16 + digit) % 4);

	}
			
		//printf("Modulo:%d\n",mod); 


	fclose(fp);
	return (mod);
			
   
}
