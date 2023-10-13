#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include "types.h"


#define MAX_BUF_SIZE 8


typedef struct decodeInfo
{
	/* Source File members*/
	char *d_src_fname;
	FILE *fptr_d_src_image;

	char image_data[MAX_BUF_SIZE];
	int extn_size;


	int file_size;

	/*Output FIle members*/
	char d_out_fname[20];
	FILE *fptr_d_out_file;
}DecodeInfo;

/*Decoding Function Prototypes*/


/*read and validate decode args from arguments*/
dec_Status read_and_validate_decode_args(char *arg[], DecodeInfo *decInfo);

/*Decodeing Process*/
dec_Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
dec_Status Open_files(DecodeInfo *decInfo);

/* Store Magic String */
dec_Status decode_magic_string(DecodeInfo *decInfo);

/* Decode extension size and file size */
dec_Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode a byte ifrom LSB of image data array */
dec_Status decode_byte_from_lsb(char data , char *image_buffer);

/* Decode secret file extenstion */
dec_Status decode_extn_secret_file(int size,DecodeInfo *decInfo);

dec_Status decode_secret_file_size(DecodeInfo *decinfo);

/* Decode secret file data*/
dec_Status decode_secret_file(int size,DecodeInfo *decInfo);

dec_Status decode_file(char *str, DecodeInfo *decInfo);

#endif
