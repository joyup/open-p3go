#include <stdio.h>
#include <stdlib.h>
#include "update_file.h"

#define BUF_SIZE (1024*1024)
#define MODULE_COUNT	2
struct 
{
	char module_name[256];
	int nand_page_offset;
	int nand_page_end;
}module_info[MODULE_COUNT] = 
{
	{"loader.bin", 128, 2175},
	{"homebrew.bin", 10240, 17279},
};

static unsigned int*  pcrc_table = NULL;
void CRCFini(void)
{
    if(NULL != pcrc_table){
        free(pcrc_table);
        pcrc_table = NULL;
    }
}
int CRCInit(void)
{
    CRCFini();
    pcrc_table = (unsigned int*)malloc(sizeof(unsigned int)*8*256);
    if(NULL == pcrc_table){
        return 0;
    }

    int I, J;
    unsigned int C;
    for (I=0;I<256;I++)
    {
        for (C=I,J=0;J<8;J++)
            C=(C & 1) ? (C>>1)^0xEDB88320L : (C>>1);
        pcrc_table[I]=C;
    }
    return 1;
}

unsigned int CalcCRC32(unsigned int StartCRC,unsigned char *Addr,unsigned int Size)
{
    unsigned int I;
    for (I=0; I<Size; I++)
        StartCRC = pcrc_table[(unsigned char)StartCRC ^ Addr[I]] ^ (StartCRC >> 8);
    return(StartCRC);
}

int main(int argc,char *argv[])
{
	int i;
	unsigned int *crc_tables = 0;
	unsigned char *buf = 0;
	unsigned int size;
	unsigned int filesize=0;
	UPDATE_FILE_HEAD file_header;
	SESSION_FILE_HEAD session_header;
	FILE *fp_session;
	unsigned int start_crc;
	unsigned int module_coount = 0;

#if 0
	if(argc < 3)
	{	
		printf("update_file.exe filename [filename] [...]!\n");
		return 0;
	}
#endif
	
	FILE *outfp = fopen("update.bin","wb+");
	if(outfp == NULL)
	{
		printf("%s file open fail!\n","update.bin");
		goto error;
	}
	fwrite((unsigned char *)&file_header, sizeof(UPDATE_FILE_HEAD), 1, outfp);
	filesize += sizeof(UPDATE_FILE_HEAD);
	
	buf = (char *)malloc(BUF_SIZE);
	if(buf == NULL) 
		goto error;

	if(CRCInit() == 0)
		goto error;

	memset(buf,0,BUF_SIZE);
	for(i = 0; i < MODULE_COUNT; i++)
	{
		printf("module_info[%d] = %s\n",i,module_info[i].module_name);
		fp_session = fopen(module_info[i].module_name, "rb");
		if(fp_session == NULL)
		{
			printf("%s file open fail!\n",module_info[i].module_name);
			continue;
		}
		
		fseek(fp_session,0,SEEK_END);
		session_header.session_length = ftell(fp_session)+4;	//add 4bytes crc
		session_header.session_nand_page_offset = module_info[i].nand_page_offset;
		session_header.session_nand_page_end = module_info[i].nand_page_end;
		fwrite((unsigned char *)&session_header, sizeof(session_header), 1, outfp);
		filesize += sizeof(SESSION_FILE_HEAD);

		start_crc = 0;
		fseek(fp_session,0,SEEK_SET);
		do
		{
			size = fread(buf,1,BUF_SIZE,fp_session);
			if(size > 0)
			{
				fwrite(buf,1,size,outfp);
				start_crc = CalcCRC32(start_crc,buf,size);
			}
			filesize +=size;

		}while(size >= BUF_SIZE);
		fclose(fp_session);
		fwrite(&start_crc,1,sizeof(unsigned int),outfp);
		filesize += 4;

		module_coount++;
	}

	fseek(outfp,0,SEEK_SET);
	//写入文件头
	memset((unsigned char *)&file_header, 0, sizeof(UPDATE_FILE_HEAD));
	memcpy(file_header.sign, UPDATE_FILE_SIGN_STRING, 4);
	file_header.hw_version = 1;
	file_header.session_num = module_coount;
	file_header.total_length = filesize;
	fwrite((unsigned char *)&file_header, sizeof(UPDATE_FILE_HEAD), 1, outfp);
	
	fclose(outfp);
	if(buf) free(buf);
	CRCFini();		
	return 0;

error:
	if(buf) free(buf);
	return 1;
}

