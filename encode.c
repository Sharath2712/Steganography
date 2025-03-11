#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

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
uint get_file_size(FILE *fptr)
{
	fseek(fptr, 0, SEEK_END);
	return ftell(fptr);
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

OperationType check_operation_type (char *argv[]) //to check operation
{
	if(strcmp(argv[1], "-e") == 0)  //check operation is encode or decode
	{
		return e_encode;
	}
	else if(strcmp(argv[1], "-d") == 0)		
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(strstr(argv[2], ".bmp") != NULL)
	{
		encInfo -> src_image_fname = argv[2];
		if((strstr(argv[3], ".txt") != NULL) || (strstr(argv[3], ".c") != NULL) || (strstr(argv[3], ".sh") != NULL))
		{
			encInfo -> secret_fname = argv[3];
			strcpy(encInfo -> extn_secret_file, strstr(argv[3], "."));
			if(argv[4] == NULL)
			{
				encInfo -> stego_image_fname = "stego.bmp";
				return e_success;
			}
			else if(strstr(argv[4], ".bmp") != NULL)
			{
				encInfo -> stego_image_fname = argv[4];
				return e_success;
			}
		}
		return e_failure;
	}
	return e_failure;
}

Status check_capacity(EncodeInfo *encInfo)
{
	int value = (54 + ((strlen(MAGIC_STRING) + 4 + strlen(strstr(encInfo -> secret_fname, ".")) + 4 + get_file_size(encInfo -> fptr_secret)) *8));
	if(value < (get_image_size_for_bmp(encInfo -> fptr_src_image)))
	{
		return e_success;
	}
	return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	fseek(fptr_src_image, 0, SEEK_SET);
	char buffer[54];
	fread(buffer, 54, 1, fptr_src_image);
	fwrite(buffer, 54, 1, fptr_dest_image);
	return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char image_buffer[8];
	for(int i = 0;i < size;i++)
	{
		fread(image_buffer, 8, 1, fptr_src_image);
		encode_byte_to_lsb(data[i], image_buffer);
		fwrite(image_buffer, 8, 1, fptr_stego_image);
	}
	return e_success;
}
Status encode_byte_to_lsb(char size, char *image_buffer)
{
	int i;
	for(i = 0;i < 8;i++)
	{
		image_buffer[i] = image_buffer[i] & ~1 | (unsigned)((size & (1 << (7 - i))) >> (7 - i));
	}
}

Status encode_extn_size(int size, EncodeInfo *encInfo)
{
	char image_buffer[32];
	fread(image_buffer, 32, 1, encInfo -> fptr_src_image);
	encode_size_to_lsb(size, image_buffer);
	fwrite(image_buffer, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
	int i;
	for(i = 0;i < 32;i++)
	{
		image_buffer[i] = image_buffer[i] & ~1 | (unsigned)((size & (1 << (31 - i))) >> (31 - i));
	}
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(encInfo -> extn_secret_file, strlen(encInfo -> extn_secret_file), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	encode_extn_size(file_size, encInfo);
	return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	encode_data_to_image(encInfo -> secret_fname, strlen(encInfo -> secret_fname), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char buffer;
	while(fread (&buffer, 1, 1, fptr_src) == 1)
	{
		fwrite(&buffer, 1, 1, fptr_dest);
	}
	return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf("open files gets success \n");
		if(check_capacity(encInfo) == e_success)
	    {
			printf("check capacity gets sucess \n");
			if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)	
			{
				printf("copy bmp header gets success \n");
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("Encode magic string success \n");
					int ext_size = strlen(strstr(encInfo -> secret_fname, "."));
					if(encode_extn_size(ext_size, encInfo) == e_success)
					{
						printf("secret file extension encoded successfuly \n");
						if(encode_secret_file_extn(encInfo -> secret_fname, encInfo) == e_success)
						{
							printf("secret file extension successfuly \n");
							if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
							{
								printf("secret file size encoded successfuly \n");
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("secret file data encoded successfuly \n");
									if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
									{
										printf("Data copied successfully... \n");
										fclose(encInfo -> fptr_src_image);
										fclose(encInfo -> fptr_secret);
										fclose(encInfo -> fptr_stego_image);
										return e_success;
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
			return e_failure;
		}
		return e_failure;
	}
	return e_failure;
}
