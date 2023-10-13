#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<unistd.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

/*
 *Check operation type option -e or -d
 *Inputs:command line arguments
 *Return values : e_encode for -e and e_decode for -d
 */
OperationType check_operation_type(char *argv[])
{
	/*Comparing argument 1  with -e for selecting encode*/
	if(strcmp(argv[1],"-e") == 0)
	{
		/*return if -e is passed*/
		return e_encode;
	}
	else if(strcmp(argv[1],"-d") == 0)
	{
		/*return if -d is passed*/
		return e_decode;
	}
}

/*
 *Get File names of input and output files
Inputs :Command line arguments and struct variables to store names 
Outputs:source file name secret file and output file name
Return values:e_success and e_failure for errors
 */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	/*Checking .bmp file is passed or not */
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{
		/*store the file name in variable*/
		encInfo->src_image_fname = argv[2];

		/* Check Secret File passed or not in Command line args */
		if((strcmp(strstr(argv[3],"."),".txt") == 0))
		{


			/*Store the secret file name to Structure member */
			encInfo->secret_fname = argv[3];

			/*copy the extenstion to variable name*/
			strcpy(encInfo->extn_secret_file,".txt");
		}
		/*Check if third argument extn is .c*/
		else if(strcmp(strstr(argv[3],"."),".c") == 0)
		{
			/*store the file name*/
			encInfo->secret_fname = argv[3];

			/*copy and store in variable*/
			strcpy(encInfo->extn_secret_file,".c");

		}
		else if(strcmp(strstr(argv[3],"."),".h") ==0 )
		{
			encInfo->secret_fname = argv[3];

			strcpy(encInfo->extn_secret_file,".h");

		}
		else if(strcmp(strstr(argv[3],"."),".sh") == 0)
		{
			encInfo->secret_fname = argv[3];

			strcpy(encInfo->extn_secret_file,".sh");
		}
		/*if fourth argument is not passed  store by default name*/
		if(argv[4] != NULL)
			encInfo->stego_image_fname = argv[4];
		else
			encInfo->stego_image_fname = "stego.bmp";   //store default name
		return e_success;

	}
	else
		return e_failure;  // if failed
}

/*
 *Get the size of source file and secret file size
 *Inputs : Source file and secret file
 *Outputs : store size in magic_capacity and size secret file
 */
Status check_capacity(EncodeInfo *encInfo)
{
	/*function call for getting  image size*/
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

	/*function call for getting secret file size*/
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

	/* Checking image capacity greater than all encoding data*/
	if((encInfo->image_capacity ) > ((strlen(MAGIC_STRING) + 4 + 4 + 4 + encInfo->size_secret_file )* 8))
		return e_success;
	else
		return e_failure;

}

/*Function definition for finding the size*/
uint get_file_size(FILE *fptr)
{
	/*setting the position to end*/
	fseek(fptr, 0, SEEK_END);

	/*returning ftell position*/
	return ftell(fptr);
}

/*Function definition for copying the header*/
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
	/*declare the 54 bytes of arr*/
	char arr[54];

	/*setting the offset position to 0*/
	fseek(fptr_src_image,0,SEEK_SET);

	/*read the 32 bytes of data from source file*/
	fread(arr,54,1,fptr_src_image);

	/*store the 54 bytes i output file*/
	fwrite(arr,54,1,fptr_dest_image);
	return e_success;
}

/*Function definition for encoding magic string*/
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
	/*Function call for getting source file and storing  in output file*/
	encode_data_to_image(magic_string,2,encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
	return e_success;
}

/*Function definition for fetching 8 bytes and storing in output file*/
Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image,EncodeInfo *encInfo)
{
	/*loop run upto size times*/
	for(int i=0;i<size;i++)
	{
		/*fread will fetch 8 bytes from source file*/
		fread(encInfo->image_data,8,1,fptr_src_image);

		/*function call for getting bit from msb to lsb*/
		encode_byte_to_lsb(data[i],encInfo->image_data);

		/*storing 8 bytes of data to output file*/
		fwrite(encInfo->image_data,8,1,fptr_stego_image);
	}
}

/*
 *Storing secret file data to output file.
 *Inputs : secret file and  output file
 *Return values : e_succes if true
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	/*setting the position to 0th */
	fseek(encInfo->fptr_secret, 0 ,SEEK_SET);

	/*declare the array with secret file size*/
	char str[encInfo->size_secret_file];

	/*fetch the secret file size bytes from file and  store in output file*/
	fread(str , encInfo->size_secret_file,1,encInfo->fptr_secret);

	/*Function call for fetch and store in output file*/
	encode_data_to_image(str, strlen(str),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);

	return e_success;
}


/*Function definition for getting lsb bytes*/
Status encode_byte_to_lsb(char data,char *image_buffer)
{
	/*loop runs 8 times*/
	for(int i = 0; i < 8;i++)
	{
		/*Getting msb bit and shifting to lsb side*/
		image_buffer[i] = ((data & (1 << (7 - i))) >> (7-i)) | (image_buffer[i] & 0XFE);
	}
}

/* Function for getting bits to lsb 32 times*/
Status encode_size_to_lsb(int data,char *image_buffer)
{
	for(int i = 0; i < 32; i++)
	{
		/*getting msb bit and shifting to lsb side*/
		image_buffer[i] = ((data & (1 << (31 - i))) >> (31 - i)) | (image_buffer[i] & 0XFE);
	}

}

/*Function definition for encoding the extenstion of secret file*/
Status encode_secret_file_extn(char*file_extn,EncodeInfo *encInfo)
{
	/*function call for fetching the bytes and storing in output file*/
	encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
	return e_success;
}

/*Function definition for encoding the secret file size*/
Status encode_secret_file_size(int file_size,EncodeInfo *encInfo)
{
	/*declare the array of 32 bytes*/
	char arr[32];

	/*read 32 bytes from source file*/
	fread(arr,32,1,encInfo->fptr_src_image);

	/*function call for encode 32 bytes from lsb*/
	encode_size_to_lsb(file_size,arr);

	/*store 32 bytes from array to outputfile*/
	fwrite(arr,32,1,encInfo->fptr_stego_image);

	return e_success;
}

/*Function definition for encoding extn size  to output file */
Status encode_secret_file_extn_size(int extn_size,EncodeInfo *encInfo)
{
	/*declare the array*/
	char arr[32];

	/*read 32  bytes from from source file*/
	fread(arr,32,1,encInfo->fptr_src_image);

	/*function calling  for  size to lsb fetch 32 bytes and store in output file*/
	encode_size_to_lsb(extn_size,arr);

	/*write 32 bytes in output file*/
	fwrite(arr,32,1,encInfo->fptr_stego_image);

	return e_success;
}

/*Function definition  for copying the remaining data from source file and store in output file*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	/*declare the variable*/
	char ch;

	/*loop run upto get -1 value */
	while( (fread(&ch,1,1,fptr_src)) > 0)
	{
		/*store 1 byte each time in output file*/
		fwrite(&ch,1,1,fptr_dest);
	}
	return e_success;
}

/*
 *Checking all functions is success or failure
 *Inputs are encInfo structure
 *Outputs : e_success or e_failure
 */
Status do_encoding(EncodeInfo *encInfo)
{
	sleep(1);
	if(open_files(encInfo) == e_success)
	{
		printf("\nINFO : Open Files Success\n");
	}
	else
	{
		printf("ERROR : Open Files Failure\n");
		return e_failure;
	}
	sleep(1);
	if(check_capacity(encInfo) == e_success)
	{
		printf("\nINFO : Check Capacity Success\n");
	}
	else
	{
		printf("ERROR : Check Capacity Failure\n");
		return e_failure;
	}

	sleep(1);

	if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
		printf("\nINFO : Copied bmp header\n");
	else
	{
		printf("ERROR : Copied bmp header failure\n");
		return e_failure;
	}

	sleep(1);

	if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		printf("\nINFO : Encoding Magic String Success\n");
	else
	{
		printf("ERROR :  Encoding  Magic String is Failure\n");
		return e_failure;
	}

	sleep(1);



	if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
		printf("\nINFO : Encode secret file Extenstion size is Success\n");
	else
	{
		printf("ERROR : Encode secret file extenstion size is Failure\n");
		return e_failure;
	}
	sleep(1);
	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
	{
		printf("\nINFO : Encode Secret file Extenstion is Success\n");
	}
	else
	{
		printf("ERROR : Encode Secret file Extenstion is Failure\n");
		return e_failure;
	}

	if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
		printf("\nINFO : Encode Secret File Size is Success\n");
	else
	{
		printf("ERROR : Encode Secret File Size is Failure\n");
		return e_failure;
	}

	sleep(1);

	if(encode_secret_file_data(encInfo) == e_success)
		printf("\nINFO : Encode Secret File data is Success\n");
	else
	{
		printf("ERROR : Encode Secret File data is Failure\n");
		return e_failure;
	}

	sleep(1);

	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
		printf("\nINFO : Copy remaining data is Success\n");
	else
	{
		printf("ERROR : Copy remaining data is Failure\n");
		return e_failure;
	}
	return e_success;

}


