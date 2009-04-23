#include <stdio.h>
#include <fcntl.h>
#include "main.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~ The following defs are extracted from STDVM ~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef signed char  S8;
typedef signed short S16;
typedef signed int   S32;
typedef long long S64;

typedef U16  STPTI_Pid_t;
#define STDVM_MAX_PIDS         (16)
#define STDVM_MAX_LENGTH_NAME  (255)
#define STDVMi_MAX_NAME_LENGTH          (STDVM_MAX_LENGTH_NAME+1)
#define READ_IDX_CHUNK 1024*1024

#define PTS_TIME  
typedef enum
{
    STDVM_STREAMTYPE_INVALID =  0, /* None     : Invalid type                         */
    STDVM_STREAMTYPE_MP1V    =  1, /* Video    : MPEG1                                */
    STDVM_STREAMTYPE_MP2V    =  2, /* Video    : MPEG2                                */
    STDVM_STREAMTYPE_MP4V    =  3, /* Video    : H264                                 */
    STDVM_STREAMTYPE_MP1A    =  4, /* Audio    : MPEG 1 Layer I                       */
    STDVM_STREAMTYPE_MP2A    =  5, /* Audio    : MPEG 1 Layer II                      */
    STDVM_STREAMTYPE_MP4A    =  6, /* Audio    : like HEAAC,Decoder LOAS / LATM - AAC */
    STDVM_STREAMTYPE_TTXT    =  7, /* Teletext : Teletext pid                         */
    STDVM_STREAMTYPE_SUBT    =  8, /* Subtitle : Subtitle pid                         */
    STDVM_STREAMTYPE_PCR     =  9, /* Synchro  : PCR pid                              */
    STDVM_STREAMTYPE_AC3     = 10, /* Audio    : AC3                                  */
    STDVM_STREAMTYPE_H264    = 11, /* Video    : H264                                 */
    STDVM_STREAMTYPE_MPEG4P2 = 12, /* Video    : MPEG4 Part II                        */
    STDVM_STREAMTYPE_VC1     = 13, /* Video    : Decode Simple/Main/Advanced profile  */
    STDVM_STREAMTYPE_AAC     = 14, /* Audio    : Decode ADTS - AAC                    */
    STDVM_STREAMTYPE_HEAAC   = 15, /* Audio    : Decoder LOAS / LATM - AAC            */
    STDVM_STREAMTYPE_WMA     = 16, /* Audio    : WMA,WMAPRO                           */
    STDVM_STREAMTYPE_DDPLUS  = 17, /* Audio    : DD+ Dolby digital                    */
    STDVM_STREAMTYPE_DTS     = 18, /* Audio    : DTS                                  */
    STDVM_STREAMTYPE_MMV     = 19, /* Video    : Not used for now, need STDEMUX       */
    STDVM_STREAMTYPE_MMA     = 20, /* Audio    : Not used for now, need STDEMUX       */
    STDVM_STREAMTYPE_OTHER   = 255,/* Misc     : Non identified pid                   */
    STDVM_STREAMTYPE_MP3,
    STDVM_STREAMTYPE_JPEG
} STDVM_StreamType_t;

typedef struct
{
    STDVM_StreamType_t  Type;
    STPTI_Pid_t         Pid;
} STDVM_StreamData_t;

typedef enum STDVMi_StreamFileType_e
{
    STREAM_FILE_TYPE_LINEAR,
    STREAM_FILE_TYPE_CIRCULAR,
    STREAM_FILE_TYPE_CIRCULAR_LINEAR
}STDVMi_StreamFileType_t;

typedef struct STDVMi_ProgramInfo_s
{
    U8                      NbOfPids;
    STDVM_StreamData_t      Pids[STDVM_MAX_PIDS];
    S64                     ProgStartPos;                   /* This PIDs start pos in the stream in Packets */
    S64                     ProgEndPos;                     /* This is PIDs end pos in the stream in Packets */
}STDVMi_ProgramInfo_t;


typedef struct STDVMi_StreamInfo_s
{
    U32                     Signature;

    U32                     Version;                        /* Version of STDVMi_StreamInfo_t */

    U32                     NbOfFiles;                      /* No of files recorded for this stream */

    char                    ChannelName[STDVMi_MAX_NAME_LENGTH];

    char                    Description[STDVMi_MAX_NAME_LENGTH];

    /* Circular File handling */
    STDVMi_StreamFileType_t StreamFileType;                 /* type of the stream file linear, circular ... */

    S64                     StreamStartPos;                 /* Stream start position for Circular files */

    S32                     FirstIndexPos;                  /* First index corresponding to record file data */

    /* Circular_Linear Stream handling  */
    S64                     CircularPartSize;               /* Size of the circular portion in the stream */

    S32                     CircularPartIndexEntries;       /* number of index entries in circular portion */

    /* Crop begining of Stream handling  */
    S64                     StreamStartPosAfterCrop;        /* Stream start position after crop beginning of the stream */

    S64                     StreamEndPosAfterCrop;          /* Stream end position after crop end of the stream */

    /* User Storage Record Date and Time */
    U32                     RecordDateTime;                 /* Record DateTime provided by user */

    U32                     NbOfPrograms;                   /* Number of programs recorded in this stream */

    STDVMi_ProgramInfo_t    ProgramInfo[1];                 /* info of the first program, grows as number of programs increases */

}STDVMi_StreamInfo_t;

typedef struct STDVMi_IndexInfo_s
{
    U32                     PacketTimeInMs;
    S64                     PacketPosition;
    U16                     Tag;
    U16                     Size;
    U32                     Data;

}STDVMi_IndexInfo_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void help(void)
{
    printf("\nUsage: ./run.exe INFO_FILE_NAME INDEX_FILE_NAME\n\n");
    exit(0);
}

int example_ParseIndex(void)
{
#ifdef EXAMPLE_PARSEINDEX
    int infofd, idxfd, outputfd;
    int BytesRead, IndexRead=0, IndexCnt=0, i;
    STDVMi_StreamInfo_t STDVM_Info;
    STDVMi_IndexInfo_t*  STDVM_Index;
    char str[255];

    if (inargc != 3)
	help();

    outputfd = open("Stream_Info.txt", O_WRONLY | O_CREAT);
    if (outputfd < 0)
	printf("Unable to create Stream_Info.txt.\n");

    if (strcmp(inargv[1], "NULL")!=0)
    {
	infofd = open(inargv[1], O_RDONLY);
	if (infofd < 0)
	    printf("Unable to open %s\n", inargv[1]);
	BytesRead = read(infofd, &STDVM_Info, sizeof(STDVMi_StreamInfo_t));
        if (BytesRead != sizeof(STDVMi_StreamInfo_t))
	    printf("Unable to read %s\n", inargv[1]);
	
	sprintf(str, "Signature: 0x%x\n", STDVM_Info.Signature);
	write(outputfd, str, strlen(str));
	sprintf(str, "Version: 0x%x\n", STDVM_Info.Signature);
    write(outputfd, str, strlen(str));
    sprintf(str, "NbOfFiles: %d\n", STDVM_Info.NbOfFiles);
    write(outputfd, str, strlen(str));
    sprintf(str, "ChannelName: %s\n", STDVM_Info.ChannelName);
    write(outputfd, str, strlen(str));
    sprintf(str, "Description: %s\n", STDVM_Info.Description);
    write(outputfd, str, strlen(str));
    sprintf(str, "StreamFileType: %d\n", STDVM_Info.StreamFileType);
    write(outputfd, str, strlen(str));
    sprintf(str, "StreamStartPos: 0x%llx\n", STDVM_Info.StreamStartPos);
    write(outputfd, str, strlen(str));
    sprintf(str, "FirstIndexPos: %d\n", STDVM_Info.FirstIndexPos);
    write(outputfd, str, strlen(str));
    sprintf(str, "CircularPartSize: 0x%llx\n", STDVM_Info.CircularPartSize);
    write(outputfd, str, strlen(str));
    sprintf(str, "CircularPartIndexEntries: %d\n", STDVM_Info.CircularPartIndexEntries);
    write(outputfd, str, strlen(str));
    sprintf(str, "StreamStartPosAfterCrop: 0x%llx\n", STDVM_Info.StreamStartPosAfterCrop);
    write(outputfd, str, strlen(str));
    sprintf(str, "StreamEndPosAfterCrop: 0x%llx\n", STDVM_Info.StreamEndPosAfterCrop);
    write(outputfd, str, strlen(str));
    sprintf(str, "RecordDateTime: %d\n", STDVM_Info.RecordDateTime);
    write(outputfd, str, strlen(str));
    sprintf(str, "NbOfPrograms: %d\n", STDVM_Info.NbOfPrograms);
    write(outputfd, str, strlen(str));
    sprintf(str, "STDVMi_ProgramInfo_t: NbOfPids: %d\n", STDVM_Info.ProgramInfo[0].NbOfPids);
    write(outputfd, str, strlen(str));
    for (i=0; i<STDVM_MAX_PIDS; i++)
    {
        sprintf(str, "STDVMi_ProgramInfo_t: PID(%d): Type: %d  Pid: %d\n", i,  STDVM_Info.ProgramInfo[0].Pids[i].Type, STDVM_Info.ProgramInfo[0].Pids[i].Pid);
        write(outputfd, str, strlen(str));
    }
    sprintf(str, "STDVMi_ProgramInfo_t: ProgStartPos: 0x%llx\n", STDVM_Info.ProgramInfo[0].ProgStartPos);
    write(outputfd, str, strlen(str));
    sprintf(str, "STDVMi_ProgramInfo_t: ProgEndPos: 0x%llx\n\n\n", STDVM_Info.ProgramInfo[0].ProgEndPos);
    write(outputfd, str, strlen(str));
    }

    
    if (strcmp(inargv[2], "NULL")!=0)
    {
	idxfd = open(inargv[2], O_RDONLY);
	if (idxfd < 0)
	    printf("Unable to open %s\n", inargv[2]);
        
    STDVM_Index = (STDVMi_IndexInfo_t*)malloc(READ_IDX_CHUNK*sizeof(STDVMi_IndexInfo_t));
    if (STDVM_Index == NULL)
    {        
        printf("Not sufficient memory.\n");
        close(idxfd);
        close(infofd);
        close(outputfd);
        exit(0);
    }
    memset(STDVM_Index, 0, READ_IDX_CHUNK*sizeof(STDVMi_IndexInfo_t));
        
	lseek(idxfd, 0, SEEK_END);
	IndexCnt = lseek(idxfd,0,SEEK_CUR)/sizeof(STDVMi_IndexInfo_t);
	IndexRead = 0;
    lseek(idxfd, 0, SEEK_SET);
    
    printf("Number of Index: %d\n", IndexCnt);
	while(IndexRead < IndexCnt)
	{
		BytesRead = read(idxfd, STDVM_Index, READ_IDX_CHUNK*sizeof(STDVMi_IndexInfo_t));
        IndexRead += BytesRead/sizeof(STDVMi_IndexInfo_t);
        
        for (i=0; i<BytesRead/sizeof(STDVMi_IndexInfo_t); i++)
        {
            sprintf(str, "Index(%8d): PacketTimeInMs: %8d   Size: %8d  PacketPosition: 0x%16llx\n", i, STDVM_Index[i].PacketTimeInMs, STDVM_Index[i].Size, STDVM_Index[i].PacketPosition);
            write(outputfd, str, strlen(str));
        }
        memset(STDVM_Index, 0, READ_IDX_CHUNK*sizeof(STDVMi_IndexInfo_t));
	}
    }	
    free(STDVM_Index);
    
    close(idxfd);
    close(infofd);
    close(outputfd);
#endif
    return 0;
}


