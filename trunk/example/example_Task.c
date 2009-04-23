#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "main.h"

#include <fcntl.h>

#define FILE_ACCESS_MUTEX_LOCK
#define STREAM_NAME	"/root/pdr/media_vol/test.ts"

#define FILE_SIZE	(long long)1*1024*1024*1024
#define RECORD_BUF_SIZE 188*6*1024
#define READ_BUF_SIZE	188*3*1024

#define RECORD_TASK_DELAY	10
#define READ_TASK_DELAY		10

#ifdef EXAMPLE_TASK

typedef void            vfs_file_t;
static vfs_file_t *StreamFileRdHandle=NULL;
static vfs_file_t *StreamFileWtHandle=NULL;	

typedef pthread_t   Task_t;
typedef int task_flags_t;

long long actual_file_size=0;

static void Record_Task(void* NotUsed);
static Task_t *Rec_Task;
#define REC_TASK_PRIORITY	10

static void Read_Task(void* NotUsed);
static Task_t *Rd_Task;
#define RD_TASK_PRIORITY	10

#ifdef FILE_ACCESS_MUTEX_LOCK
static sem_t* File_Access;
/*
#define MutexCreate(__sem__) 	__sem__=semaphore_create_fifo(1)
#define MutexLock(__sem__)	semaphore_wait(__sem__)
#define MutexRelease(__sem__)	semaphore_signal(__sem__)
#define MutexDelete(__sem__)	semaphore_delete(__sem__)
*/
#define MutexCreate(__sem__) 	__sem__ = malloc(sizeof(sem_t)); sem_init(__sem__, 0, 1)
#define MutexLock(__sem__)	sem_wait(__sem__)
#define MutexRelease(__sem__)	sem_post(__sem__)
#define MutexDelete(__sem__)	sem_destroy(__sem__)
#endif

/* ==========================================
   Name:        Task_Create
   Description: 
   ========================================== */
Task_t *Task_Create(void *FuncP(void *), void *Param, size_t StackSize, int Priority, const char *Name, task_flags_t Flags)
{

    Task_t          Task, *TaskID;
    pthread_attr_t  Attr;

    pthread_attr_init(&Attr);
    if (Priority >= 0)
    {
        struct sched_param Sparam;

        if (Priority < sched_get_priority_min(SCHED_RR))
            Priority = sched_get_priority_min(SCHED_RR);

        if (Priority > sched_get_priority_max(SCHED_RR))
            Priority = sched_get_priority_max(SCHED_RR);

        Sparam.sched_priority = Priority;
        pthread_attr_setschedpolicy(&Attr, SCHED_RR);
        pthread_attr_setschedparam(&Attr, &Sparam);
    }

    if (pthread_create(&Task, &Attr, (void*)FuncP, Param) != 0)
    {
        return NULL;
    }

    TaskID = malloc(sizeof(Task_t));
    if (TaskID)
    {
        *TaskID = Task;
    }

    return TaskID;

}

void Task_Delay(int num)
{
    usleep(num*1000);
}


long getElapsedTime(const struct timeval *startTime,
                    const struct timeval *endTime)
{
    return (endTime->tv_sec*1000000+endTime->tv_usec)-
        (startTime->tv_sec*1000000+startTime->tv_usec);
}

vfs_file_t  *vfs_open(const char *pathname, int flags)
{
    int fd;

    fd = open64(pathname,flags);

    return (fd < 0) ? NULL : (vfs_file_t *)fd;
}

int check_buff(unsigned char* buf)
{
    printf("buf[0]: %d   buf[1]: %d   buf[READ_BUF_SIZE-2]: %d   buf[READ_BUF_SIZE-1]: %d\n", buf[0], buf[1], buf[READ_BUF_SIZE-2], buf[READ_BUF_SIZE-1]);
    if (buf[0]==0 && buf[1]==1 && buf[2]==2 && buf[READ_BUF_SIZE-2]==(READ_BUF_SIZE-2)%188 && buf[READ_BUF_SIZE-1]==(READ_BUF_SIZE-1)%188)
	return 1;
    else
	return 0;

}

void seek_check(void)
{
    long long pos;
    long long seek_pos[7]={0x4FFFFFFF, 0x7FFFFFFF, 0x80000000};
    int int_pos[3]={0x1, 0x7FFFFFFF, 0x80000011};
    int tmp_pos = (- 2 * RECORD_BUF_SIZE)/188*188;
    int i, BytesRead;
    unsigned char *buf;

    buf = (unsigned char*)malloc(RECORD_BUF_SIZE);

    seek_pos[3] = ((long long) ( int_pos[0]) << 32) + int_pos[1];
    seek_pos[4] = (unsigned long long) int_pos[1] *2;
    seek_pos[5] = ((unsigned long long) ((unsigned int)int_pos[2])) *2;

    if (StreamFileRdHandle== NULL)
	return;
    for (i=0; i<6; i++)
    {
	seek_pos[i] = seek_pos[i]/188 * 188;
	pos = lseek64(StreamFileRdHandle, (long long)seek_pos[i], SEEK_SET);
	printf("Seeking to %llx, postion set is %llx\n", seek_pos[i], pos);
	BytesRead = read(StreamFileRdHandle, buf, RECORD_BUF_SIZE);
	if (BytesRead != RECORD_BUF_SIZE)
	    printf("file read fail.\n");
	else
	{
	    if (check_buff(buf))
		printf("data read ok.\n");
	    else
		printf("data read error.\n");
	}
    }
    seek_pos[6] = (long long)tmp_pos;
    pos = lseek64(StreamFileRdHandle, seek_pos[6], SEEK_END);
    printf("tmp_pos: %d(0x%x)  seek_pos: %ld(0x%lx)\n", tmp_pos, tmp_pos, seek_pos[6], seek_pos[6]);
    printf("Seeking to %lld(0x%llx), postion set is %lld(0x%llx)\n", seek_pos[6], seek_pos[6], pos, pos);
}

/* ======================================
   Name: 	Record_Task
   Description: 
   ====================================== */
void Record_Task(void* NotUsed)
{
    int i=0, BytesWritten;
    unsigned int TotalSize=0;
    struct timeval startTime;
    struct timeval endTime;
    unsigned char *buf;

    buf = (unsigned char*)malloc(RECORD_BUF_SIZE);

    for (i=0; i<RECORD_BUF_SIZE; i++)
	buf[i] = i%188;

    i=0;    
    while(1)
    {
	#ifdef FILE_ACCESS_MUTEX_LOCK
	MutexLock(File_Access);
	#endif
	gettimeofday(&startTime, NULL);
	BytesWritten = write(StreamFileWtHandle, buf, RECORD_BUF_SIZE);
	if (BytesWritten != RECORD_BUF_SIZE)
	    printf("Unsufficient disk space!!!\n");
	TotalSize += BytesWritten;
	actual_file_size += BytesWritten;

	if (TotalSize >= FILE_SIZE)
	{
	    printf("Record Task Completes. BytesWritten is %u\n", TotalSize);
	    free(buf);
	    #ifdef FILE_ACCESS_MUTEX_LOCK
            MutexRelease(File_Access);
            #endif
	    return;
 	}
	gettimeofday(&endTime, NULL);
	#ifdef FILE_ACCESS_MUTEX_LOCK
	MutexRelease(File_Access);
	#endif
        printf("Record_Task: \ti: %5d  TimeStart: %ld TimeTaken: %6ld----->Rec\n", i,  (startTime.tv_sec*1000000+startTime.tv_usec)/1000, getElapsedTime(&startTime, &endTime)/1000);
	i++;
	Task_Delay(RECORD_TASK_DELAY);
    }
}

/* ============================================
   Name:	Read_Task
   Description: 
   ============================================ */
void Read_Task(void* NotUsed)
{
    int i=0, BytesRead;
    unsigned int TotalSize=0;
    struct timeval startTime;
    struct timeval endTime;
    unsigned char *buf;

    buf = (unsigned char*)malloc(READ_BUF_SIZE); 
    for(i=0; i<READ_BUF_SIZE; i++)
	buf[i]=0;

    i=0;
    while(1)
    {
	#ifdef FILE_ACCESS_MUTEX_LOCK
	MutexLock(File_Access);
	#endif
	gettimeofday(&startTime, NULL);
	BytesRead = read(StreamFileRdHandle, buf, READ_BUF_SIZE);
	if (BytesRead != READ_BUF_SIZE)
	{
	    printf("Specified data not read.\n");
	    printf("File Read Size: %d\n", TotalSize);
	    #ifdef FILE_ACCESS_MUTEX_LOCK
            MutexRelease(File_Access);
            #endif
	    return;
	}
	if (!check_buff(buf))
	{
	    printf("data error.\n");
	    #ifdef FILE_ACCESS_MUTEX_LOCK
            MutexRelease(File_Access);
            #endif
	    return;
	}

	TotalSize += BytesRead;
	if (TotalSize >= FILE_SIZE && TotalSize >= actual_file_size)
	{
	    printf("Read Task completes.  FileSize is %u\n", TotalSize);
	    free(buf);
	    #ifdef FILE_ACCESS_MUTEX_LOCK
            MutexRelease(File_Access);
            #endif
	    seek_check();
	    return;
	}
	gettimeofday(&endTime, NULL);
	#ifdef FILE_ACCESS_MUTEX_LOCK
	MutexRelease(File_Access);
	#endif
	printf("Read_Task:  \ti: %5d  TimeStart: %ld  TimeTaken: %6ld\n", i, (startTime.tv_sec*1000000+startTime.tv_usec)/1000, getElapsedTime(&startTime, &endTime)/1000);
	i++;
	Task_Delay(READ_TASK_DELAY);
    }
}


/* ============================================
   Name:	Dummy_Task
   Description: 
   ============================================ */
void Dummy_Task(void* NotUsed)
{
    int i=0;

    while(i<5000)
    {
	i++;
    	printf("i=%d\n", i);
	Task_Delay(READ_TASK_DELAY);
    }
}
#endif

int example_Task(void)
{
#ifdef EXAMPLE_TASK
    long s_pos = 0xF0F0F0F0;
    long long l_pos = 0xF0F0F0F0;
    printf("sizeof long is %d,  sizeof long long is: %d\n", sizeof(long), sizeof(long long));
    printf("s_pos: %u  l_pos: %lld\n", s_pos, l_pos);
    
    printf("Minimum RR task priority: %d\n",  sched_get_priority_min(SCHED_RR));
    printf("Maximum RR task priority: %d\n",  sched_get_priority_max(SCHED_RR));

    StreamFileWtHandle = vfs_open(STREAM_NAME, O_WRONLY | O_CREAT | O_LARGEFILE);
    if (StreamFileWtHandle == NULL)
    {
	printf("Unable to create stream.ts.\n");
        return 0;
    }
    StreamFileRdHandle = vfs_open(STREAM_NAME, O_RDONLY | O_LARGEFILE);
    if (StreamFileRdHandle == NULL)
    {
	printf("Unable to read stream.ts.\n");
        return 0;
    }

#ifdef FILE_ACCESS_MUTEX_LOCK
    MutexCreate(File_Access);
#endif
	
    Rec_Task = Task_Create(Record_Task, NULL, 4096, REC_TASK_PRIORITY, "RecordingTask", 0);
    if (Rec_Task == NULL)
	printf("Unable to create Rec Task.\n");
    else
	printf("Rec Task Created.\n");

    Task_Delay(2000);

    Rd_Task = Task_Create(Read_Task, NULL, 4096, RD_TASK_PRIORITY, "ReadingTask", 0);
    if (Rd_Task == NULL)
	printf("Unable to create Rd Task.\n");
    else
	printf("Rd Task Created.\n");

    Rd_Task = Task_Create(Dummy_Task, NULL, 4096, RD_TASK_PRIORITY, "DummyTask", 0); 
    Task_Delay(1000*10);
    printf("cancelling the task.\n");
    pthread_cancel(*Rd_Task);
    pthread_join(*Rd_Task, NULL);
    printf("cancelling task done. Rd_Task==NULL: %d\n", Rd_Task==NULL);
    Task_Delay(1000*10);

    //pthread_cancel(Rd_Task);
    //pthread_cancel(Rec_Task);
#ifdef FILE_ACCESS_MUTEX_LOCK
    MutexDelete(File_Access);
#endif

    seek_check();
    return 0;
#endif
}
