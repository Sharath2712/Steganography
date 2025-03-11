#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // it contanis user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
	/* Stego Image Info	*/
	char *stego_img_fname;	//To store the input stego image file
	FILE *fptr_stego_img;	//Stego image filr pointer

	/* Decode output file info */
	char *output_fname;	//To store the output decode file
	FILE *fptr_output;	//output file pointer

	/* Secret file info	*/
	char extn_secret_file[MAX_FILE_SUFFIX];	//To store secret file extention
	int extn_size;			//size of secret file extention
	long size_secret_file;	//size of secret file

} DecodeInfo;

char fun(FILE *fptr_stego_image);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get file pointers for i/p and o/p file */
Status open_file(DecodeInfo *decInfo);

/* Store MAGIC STRING */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file ectention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file ectention */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does not real decoding */
char decode_image_to_data( FILE *fptr_stego_img);

/* Find the size of the image */
Status decode_image_to_size(int data, DecodeInfo *decInfo);

/* Decode a byte into lsb of image data array */
Status decode_byte_to_lsb(char data, char *image_buffer);

/* Decode a size into lsb of image data array */
Status decode_size_to_lsb(int size, char *image_buffer);


#endif
