#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
	//check argument count
	if(argc >= 3)
	{
		//check operation
		if(check_operation_type(argv) == e_encode)
		{
			EncodeInfo encInfo;
			if(read_and_validate_encode_args(argv, &encInfo) == e_success)
			{
				if(do_encoding(&encInfo) == e_success)
				{
					printf("|--Encoding Sucess--|");
				}
			}
		}
	

	else if(check_operation_type(argv) == e_decode)
	{
		printf("selected decoding \n");

		DecodeInfo decInfo;
		if(read_and_validate_decode_args(argv, &decInfo) == e_success)
		{
			if(do_decoding(&decInfo) == e_success)
			{
				printf("|--Decoding success--|");
			}
		}
	}
}

}



