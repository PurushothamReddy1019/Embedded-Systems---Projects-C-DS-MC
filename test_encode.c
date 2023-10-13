#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
#include<unistd.h>

/*Passing arguments through command line arguments*/
int main(int argc, char *argv[])
{
	/* Declare the Structure Variable name*/

	EncodeInfo encInfo;

	if(argc > 3)
	{

		/*Function call for checking operation option */

		if((check_operation_type(argv)) == e_encode)
		{
			printf("Selected Encoding\n");


			if(argc >=4 && argc <=5)
			{
				/*argument count should be greater than or eq to 4 and less than  or equal to 5*/

				/* Function call for reading and validating the passed files on command line*/
				if(read_and_validate_encode_args(argv,&encInfo) == e_success)
				{


					printf("\nINFO : Read and validate Encode is Success\n");


					printf("-------->ENCODING STARTED<-----------\r\n");

					/*Function call for checking the all the functions are success or failure */
					if(do_encoding(&encInfo) == e_success)
					{
						/*Encoding is done*/
						printf("\n#### ENCODING COMPLETED #####\n");
					}
					else
					{
						printf("ERROR : Encoding is Failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("Read and validate is Failure\n");
					printf("Usage : Pass the .bmp file like beautiful.bmp\n");
					return e_failure;
				}


			}
			else
			{
				printf("Please Pass the Arguments\n");
			}
		}

		/*Function call for checking operation option is "-d"*/
		else if((check_operation_type(argv)) == e_decode)
		{
			/* Decode Structure is declared */
			DecodeInfo decInfo;
			if(argc >=3 && argc <=4)
			{

				printf("Selected Decoding\n");


				/*Function call for decode part read and validate arguments */
				if(read_and_validate_decode_args(argv,&decInfo) == d_success)
				{

					printf("\nINFO : Read and Validate Decode is  Success\n");


					printf("\n-->DECODING IS STARTED........\n");

					/*Function call for  checking decode functions are success or failure */
					if(do_decoding(&decInfo) == d_success)
					{

						/*if true then Decoding is done successfully*/
						printf("\n##### Decoding is Completed #####\n");
					}
					else
					{
						/*decoding is fail*/
						printf("ERROR: Decoding is Failure\n");
						return d_failure;
					}
				}
				else
				{

					printf("ERROR : Read and Validate decode is Failure\n");

					d_failure;
				}
			}
			else 
			{
				printf("Please pass the Arguments\n");
			}

		}
	}
	else
	{
		printf("Usage : Kindly pass for\nEncoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nDecoding: ./a.out -d stego.bmp decode.txt\n");
	}



	return 0;
}


