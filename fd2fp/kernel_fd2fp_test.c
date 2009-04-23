/*
** This program is to test if data can be passed to kernel space through file descriptors.
**
** After loading the kernel module fd2fp_module.ko using load_fd2fp.sh, four devices will be created:
** /dev/test/fd2fp0, /dev/test/fd2fp1, /dev/test/fd2fp2, /dev/test/fd2fp3. Those devices will be opened
** in different processe, each giving a file descriptor. The file descriptors will be passed from one
** to another to check if the data associated with one device can be accessed by another device.
**
** Each file descriptor will have an ID field and device name field:
** fd2fpDev0: {ID: 0  Name: fd2fp0}
** fd2fpDev1: {ID: 1  Name: fd2fp1}
** fd2fpDev2: {ID: 2  Name: fd2fp2}
** fd2fpDev3: {ID: 3  Name: fd2fp3}
**
** Check the serial console of the board see if the data passed by file descriptor is correct.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "kernel_fd2fp_module.h"

#define TASK_PRIORITY	10
#define Task_Delay(x)   usleep(x*1000)
#define TASK_DELAY      100

typedef pthread_t   Task_t;

int   fd2fpDev1, fd2fpDev2, fd2fpDev3;

static Task_t *task1, *task2, *task3;
static void FD2FP_Task1(void* NotUsed);
static void FD2FP_Task2(void* NotUsed);
static void FD2FP_Task3(void* NotUsed);

/* ==========================================
   Name:        Task_Create
   Description: 
   ========================================== */
Task_t *Task_Create(void *FuncP(void *), void *Param, size_t StackSize, int Priority, const char *Name, int Flags)
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

/* ======================================
   Name: 	FD2FP_Task1
   Description: 
   ====================================== */
void FD2FP_Task1(void* NotUsed)
{
  fd2fpDev1 = open( "/dev/test/fd2fp1", O_RDWR );
  printf("fd2fpDev1: fd %d\n", fd2fpDev1);

  while(1)
  {
    Task_Delay(TASK_DELAY);
  }
}

/* ======================================
   Name: 	FD2FP_Task2
   Description: 
   ====================================== */
void FD2FP_Task2(void* NotUsed)
{
  fd2fpDev2 = open( "/dev/test/fd2fp2", O_RDWR );
  printf("fd2fpDev2: fd %d\n", fd2fpDev2);

  while(1)
  {
    Task_Delay(TASK_DELAY);
  }
}

/* ======================================
   Name: 	FD2FP_Task3
   Description: 
   ====================================== */
void FD2FP_Task3(void* NotUsed)
{
  fd2fpDev3 = open( "/dev/test/fd2fp3", O_RDWR );
  printf("fd2fpDev3: fd %d\n", fd2fpDev3);

  while(1)
  {
    Task_Delay(TASK_DELAY);
  }
}

/* ======================================
   Name: 	main
   Description: 
   ====================================== */
int main(void)
{
  int   fd2fpDev0;
  int   ret;

  fd2fpDev0 = open( "/dev/test/fd2fp0", O_RDWR );
  printf("fd2fpDev0: fd %d\n", fd2fpDev0);

  task1 = Task_Create(FD2FP_Task1, NULL, 4096, TASK_PRIORITY, "FD2FP_TASK1", 0); 
  task2 = Task_Create(FD2FP_Task2, NULL, 4096, TASK_PRIORITY, "FD2FP_TASK2", 0); 
  task3 = Task_Create(FD2FP_Task3, NULL, 4096, TASK_PRIORITY, "FD2FP_TASK3", 0); 

  if( fd2fpDev0>=0 && fd2fpDev1>=0 && fd2fpDev2>=0 && fd2fpDev3>=0 )
  {
    ret = ioctl(fd2fpDev0, FD2FP_PASS_FD, &fd2fpDev1);
    ret = ioctl(fd2fpDev0, FD2FP_PASS_FD, &fd2fpDev2);
    ret = ioctl(fd2fpDev0, FD2FP_PASS_FD, &fd2fpDev3);

    ret = ioctl(fd2fpDev1, FD2FP_PASS_FD, &fd2fpDev0);
    ret = ioctl(fd2fpDev2, FD2FP_PASS_FD, &fd2fpDev0);
    ret = ioctl(fd2fpDev3, FD2FP_PASS_FD, &fd2fpDev0);

    ret = ioctl(fd2fpDev1, FD2FP_PASS_FD, &fd2fpDev1);
    ret = ioctl(fd2fpDev1, FD2FP_PASS_FD, &fd2fpDev2);
    ret = ioctl(fd2fpDev1, FD2FP_PASS_FD, &fd2fpDev3);
  }
  else
  {
    printf("Failed to open fd2fp dev.\n");
    return 1;
  }

  ret = close(fd2fpDev3);
  ret |= close(fd2fpDev2);
  ret |= close(fd2fpDev1);
  ret |= close(fd2fpDev0);
  printf( "ret = %08X\n", ret);

  printf("cancelling the task.\n");
  pthread_cancel(*task1);
  pthread_join(*task1, NULL);
  pthread_cancel(*task2);
  pthread_join(*task2, NULL);
  pthread_cancel(*task3);
  pthread_join(*task3, NULL);

  return 0;
}
