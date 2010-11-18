#ifndef _UPDATE_FILE_H_
#define _UPDATE_FILE_H_

typedef struct
{
	char sign[4];
	int total_length;
	short hw_version;
	short session_num;
	int reserved[5];
	int crc;
}UPDATE_FILE_HEAD, *PUPDATE_FILE_HEAD;

typedef struct
{
	int session_nand_page_offset;
	int session_nand_page_end;
	int session_length;
}SESSION_FILE_HEAD, *PSESSION_FILE_HEAD;

#define UPDATE_FILE_SIGN_STRING	"BFF"

#endif

