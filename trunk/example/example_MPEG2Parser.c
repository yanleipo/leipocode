#include <stdio.h>
#include <fcntl.h>
#include "main.h"

#define TS_STREAM_NAME	"iptv_stream_3.ts"
#define VID_PID		111

#define CHUNK_SIZE	188*1024*6

void TS_PES_Parser(int offset, char* buff, int size)
{
    int j, ind, PTS, packet_pos, PTS_pos, adaption_field_length;
    int PID_Packet;
    
    printf("TS_PES_Parser: offset: %d\n", offset);

    for (ind=0, j=0; ind<size; ind+=188)
    {
        if (buff[j] != 0x47)
        {
            printf("ac_pdr_pvod_writeBuffer: packet aligned error.\n");
            continue;
        }
        
        PID_Packet = ((buff[j+1]&0x1F)<<8)|(buff[j+2]);
        
        if (PID_Packet == VID_PID && buff[j+3]&0x10==0x10)
        {
             packet_pos = 4;
             if (buff[j+3]&0x30==0x30)
             {
                adaption_field_length = buff[j+4];
                packet_pos += (1 + adaption_field_length);
             } 
             
             if (packet_pos >=188)
	     {
                printf("WARNING: ac_pdr_pvod_writeBuffer: packet_pos: %d  adaption_len: %d\n", packet_pos, adaption_field_length);
  	     }
             
             if (buff[packet_pos]==0 && buff[packet_pos+1]==0 && buff[packet_pos+2] == 1)
             {
                if (buff[packet_pos+3] != 0xBC && buff[packet_pos+3] != 0xBE &&
                    buff[packet_pos+3] != 0xBF && buff[packet_pos+3] != 0xF0 &&
                    buff[packet_pos+3] != 0xF1 && buff[packet_pos+3] != 0xF2 &&
                    buff[packet_pos+3] != 0xF8 && buff[packet_pos+3] != 0xFF &&
                    (buff[packet_pos+7]&0x80) == 0x80 )
                {
                    PTS_pos = packet_pos + 9;
                    printf("ac_pdr_pvod_writeBuffer: PTS found @ %d: %x %x %x %x %x\n", ind/188, buff[PTS_pos],buff[PTS_pos+1], buff[PTS_pos+2], buff[PTS_pos+3], buff[PTS_pos+4]);
                }
             }
        }
    }
}


int example_MPEG2Parser(void)
{
   int streamId;
   char* buff;
   int offset=0;
#ifdef EXAMPLE_MPEG2PARSER

   /* open stream */
   streamId = open(TS_STREAM_NAME, O_RDONLY);
   if (streamId < 0)
   {
	printf("Unable to open %s\n", TS_STREAM_NAME);
	return 0;
   }	
   lseek(streamId, 0, SEEK_SET);

   /* allocate memory for buffer */
   buff = malloc(sizeof(char)*CHUNK_SIZE);
   if (buff == NULL)
   {
	printf("Unable to allocate memory.\n");
	return 0;
   }

   while ( read(streamId, buff, CHUNK_SIZE) != 0)
   {
   //read(streamId, buff, CHUNK_SIZE);
	TS_PES_Parser(offset, buff, CHUNK_SIZE);
	offset += CHUNK_SIZE;
	lseek(streamId, offset, SEEK_SET);
   }

   free(buff);
   close(streamId);

   printf("example_MPEG2Parser done.\n");
#endif
   return 0;
}
