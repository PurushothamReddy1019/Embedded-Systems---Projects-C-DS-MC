#include<stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include<string.h>
#include<stdlib.h>
#include <unistd.h>


/* Read and validate decode args from argv*/
dec_Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)  // validating the 2nd argument, checking the bmp file

		decInfo->d_src_fname = argv[2];  //<---- assigning the 2nd position to the source image file
	else
	{
		printf("ERROR: Please enter the .bmp file\n");
		return d_failure;
	}

	if(argv[3] != NULL)
	{

		strcpy(decInfo->d_out_fname,argv[3]);   ////<---- assigning the 3rd position to the decoded file

	}
	else
		strcpy(decInfo->d_out_fname,"output");

	return d_success;

}

/* 
 * Get File pointers for i/p and o/p files*/
dec_Status Open_files(DecodeInfo *decInfo)
{
	/*open the .bmp file*/
	decInfo->fptr_d_src_image = fopen(decInfo->d_src_fname,"r");
	/*Error handling*/
	if(decInfo->fptr_d_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR : Unable to open file %s\n",decInfo->d_src_fname);
		return d_failure;
	}
	return d_success;

}

/* Store Magic String*/
dec_Status decode_magic_string(DecodeInfo *decInfo)
{
	char magic_string[3] = {0};
	//int i = strlen(MAGIC_STRING);

	char buffer[8];
	/*Skip the 54 bytes*/
	fseek(decInfo->fptr_d_src_image, 54, SEEK_SET);

	for(int i=0; i<2; i++)
	{
		fread(buffer,1,8,decInfo -> fptr_d_src_image);
		
		magic_string[i] = decode_byte_from_lsb(magic_string[i],buffer);
	}
	magic_string[3] = '\0';

	if(strcmp(magic_string,MAGIC_STRING) == 0)
		return d_success;
	else
		return d_failure;
}


/* Decode a byte into LSB of image data array
 * Description: To decoding the secret data each bit of #(magic string) from 1 byte of LSb data
 */
dec_Status decode_byte_from_lsb(char data ,char *image_buffer)
{
	int bit = 7;
	unsigned char ch = 0x00;
	for (int i = 0; i < 8; i++)
	{
		ch = ((image_buffer[i] & 0x01) << bit--) | ch; //<---- image buffer and with 1 and it shld or with ch and stored in ch
	}
	return ch;
}


dec_Status decode_file_extn_size(DecodeInfo *decInfo) //<--- function definition for encode size
{
	char str[32];
	int size = 0;
	int bit = 31;

	fread(str, 32, 1, decInfo->fptr_d_src_image);   //<------ reading the image buffer

	for(int i=0;i<32;i++)
	{
		size = ((str[i] & 1) << bit-- ) | size;  //<----- image buffer ANDing with 1 then Oring with num and storing in the num
	}

	decInfo->extn_size = size;    //store  in size

	//checking if it is equal
	if(decInfo->extn_size == strlen(".txt"))
		return d_success;
	else if(decInfo->extn_size == strlen(".c"))
		return d_success;
	else if(decInfo->extn_size == strlen(".h"))
		return d_success;
	else if(decInfo->extn_size == strlen(".sh"))
		return d_success;
	else
		return d_failure;
}

/* Decode secret file extenstion 
 * Input: secret file size,stego image, decoded fiel
 * Return Value: e_success on file errors
 */
dec_Status decode_extn_secret_file(int size,DecodeInfo *decInfo)
{

	char extn_secret_file[size+1];
	char buffer[8];
	for(int i = 0;i<size;i++)
	{
		extn_secret_file[i] = 0;

		fread(buffer,1,8,decInfo->fptr_d_src_image);

		extn_secret_file[i] = decode_byte_from_lsb(extn_secret_file[i],buffer);
	}
	extn_secret_file[size] = '\0';
	strcat(decInfo->d_out_fname,extn_secret_file);

	decInfo->fptr_d_out_file =fopen(decInfo->d_out_fname,"w");
	if(decInfo->fptr_d_out_file == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR : unable to open file %s\n",decInfo->d_out_fname);
		return d_failure;
	}
	return d_success; //<---- returning success after dencoding the secret file extention

}


dec_Status decode_secret_file_size(DecodeInfo *decInfo)
{
	char str[32];
	int size =0;
	int bit = 31;
	fread(str,32,1,decInfo->fptr_d_src_image);

	for(int i=0;i<32;i++)
	{
		size = ((str[i] & 1)  << bit -- )| size;
	}
	decInfo-> file_size = size;
	return d_success;
}

/* Decode secret file data
 * Input: secret file size (structure), stego image, decoded file
 * Output: decoded secret file data
 * Return Value: e_success after decoded
 */
dec_Status decode_secret_file(int size,DecodeInfo *decInfo)
{
	char str[8];

	for(int i=0;i< size ;i++)
	{
		fread(str , 8 , 1, decInfo->fptr_d_src_image);
		decode_file(str,decInfo);
		fwrite(str,1,1,decInfo->fptr_d_out_file);
	}

	return d_success;
}

dec_Status decode_file(char *str,DecodeInfo *decInfo)
{
	char ch =0;
	int bit = 7;
	for(int i=0;i<8;i++)
	{
		ch = ((str[i] & 1) << bit--) | ch;
	}
	str[0] = ch;
	return d_success;
}


dec_Status do_decoding(DecodeInfo *decInfo)    //<---- function definition for decoding the process
{
	if(Open_files(decInfo) == d_success) //<---- function calling for open file & validating the condition  
		printf("\nINFO : Open the Source File is Success\n");

	else
	{
		printf("\nERROR : Open the Source File is Failure\n");
		return d_failure;
	}
	sleep(1);


	//function calling for decoding the magic string & validatong the condition
	if(decode_magic_string(decInfo) == d_success)
		printf("\nINFO : Decode is Magic String is Success\n");
	else
	{
		printf("\nERROR : Decode is Magic String is Failure\n");
		return d_failure;
	}
	sleep(1);

	//function calling for decoding the file extention size and file size
	if(decode_file_extn_size(decInfo) == d_success)
	{
		printf("\nINFO : Decode Secret File Extenstion Size is Success\n");
	}
	else
	{
		printf("\nERROR : Decode Secret File Extenstion Size is Failure\n");
		return d_failure;
	}
	sleep(1);

	//function calling for decoding the secret file extentiom & validating the condition
	if(decode_extn_secret_file(decInfo->extn_size,decInfo) == d_success)
	{

		printf("\nINFO : Decoding Extenstion File is Success\n");

	}
	else
	{
		printf("\nERROR : Decoding Extenstion File is Failure\n");
		return d_failure;
	}
	//function calling for decoding secret file size & validating the condition
	if(decode_secret_file_size(decInfo) == d_success)
		printf("\nINFO : Decoding Secret File size is Success\n");
	else
	{
		printf("\nERROR : Decoding Secret File Size is Failure\n");
		return d_failure;
	}
	sleep(1);

	//function calling for decoding the secret file data
	if(decode_secret_file(decInfo->file_size,decInfo) == d_success)
		printf("\nINFO : Decoding Secret File  is Success\n");
	else
	{
		printf("\nERROR : Decoding Secret File is Failure\n");
		return d_failure;
	}
	sleep(1);
	return d_success;
}



