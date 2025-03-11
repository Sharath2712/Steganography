#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "common.h"
#include "types.h"
#include<stdlib.h>
#include<unistd.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	if(strstr (argv[2], ".bmp") != NULL)
	{
		decInfo -> stego_img_fname = argv[2];
		return e_success;
	}
	return e_failure;
}

Status open_file(DecodeInfo *decInfo)
{
	decInfo -> fptr_stego_img = fopen(decInfo -> stego_img_fname, "r");
	if(decInfo -> fptr_stego_img == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_img_fname);
		return e_failure;
	}
	fseek(decInfo -> fptr_stego_img, 54, SEEK_SET);
	return e_success;
}

char decode_image_to_data(FILE *fptr_stego_img)
{
	char image_buffer[8], sum = 0;
	fread(image_buffer, 8, 1, fptr_stego_img);
	for(int i = 0; i <= 7; i++)
	{
		sum = sum + ((image_buffer[i] & 1) << 7 - i);
	}
	return sum;
}
int decode_img_to_size(FILE *fptr_stego_img)
{
	char image_buffer[32], sum = 0;
	fread(image_buffer, 32, 1, fptr_stego_img);
	for(int i = 0; i <= 31; i++)
	{
		sum = sum +((image_buffer[i] & 1) << 31 - i);
	}
	return sum;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
	char magic_string[3];
	for(int i = 0;i < 2;i++)
	{
		magic_string[i] = decode_image_to_data(decInfo -> fptr_stego_img);
	}
	if(!strcmp(magic_string, MAGIC_STRING))
	{
		return e_success;
	}
	else
	{
			return e_failure;
	}
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	decInfo -> extn_size = decode_img_to_size(decInfo -> fptr_stego_img);
	return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	int i = 0;
	for(i = 0;i < decInfo -> extn_size;i++)
	{
		decInfo -> extn_secret_file[i] = decode_image_to_data(decInfo -> fptr_stego_img);
	}
	decInfo -> extn_secret_file[i] = '\0';

	char fname[100] = "output";
	strcat(fname, decInfo -> extn_secret_file);
	decInfo -> output_fname = fname;
	decInfo -> fptr_output = fopen(decInfo -> output_fname, "w");
	return e_success;

}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
	decInfo -> size_secret_file = decode_img_to_size(decInfo -> fptr_stego_img);
	return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char ch;
	int size_secret_file = 8;
	for(int i = 0; i < decInfo -> size_secret_file; i++)
	{
		ch = decode_image_to_data(decInfo -> fptr_stego_img);
		putc(ch, decInfo -> fptr_output);
	}
	return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
	if(open_file(decInfo) == e_success)
	{
		sleep(1);
		printf("Open file gets success \n");
		if(decode_magic_string(decInfo) == e_success)
		{
			sleep(1);
			printf("Decode magic string gets success \n");
			if(decode_secret_file_extn_size(decInfo) == e_success)
			{
				sleep(1);
				printf("Secret file extention size decodded success \n");
				if(decode_secret_file_extn(decInfo) == e_success)
				{
					sleep(1);
					printf("Secret file extention success \n");
					if(decode_secret_file_size(decInfo) == e_success)
					{
						sleep(1);
						printf("Secret file size decodded success \n");
						if(decode_secret_file_data(decInfo) == e_success)
						{
							sleep(1);
							printf("Secret file data decodded success \n");
							return e_success;
							fclose(decInfo->fptr_output);
						}
						return e_failure;
					}
					return e_failure;
				}
				return e_failure;
			}
			return e_failure;
		}
		return e_failure;
	}
	return e_failure;
}



