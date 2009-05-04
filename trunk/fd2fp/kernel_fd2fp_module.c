/*
** This file will generate a kernel fd2fp module.
** 
** Copy the module to board rootfs and load the module using load_fd2fp.sh.
*/

#include <linux/vfs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>      
#include <linux/netdevice.h> 
#include <linux/mount.h>

#include "kernel_fd2fp_module.h"

/* Some declaration about the mnodule */
/* ---------------------------------- */
#define FD2FP_NAME           "fd2fp"

#ifndef FD2FP_MAJOR
#define FD2FP_MAJOR          FD2FP_MAGIC_NUMBER
#endif
static unsigned int          majorNumber = FD2FP_MAJOR;

#define FD2FP_MAX_DEVICES    4
#define FD2FP_DEV_NAME_LEN   20

static struct cdev*          moduleCdev  = NULL;

MODULE_AUTHOR("Yan Leipo");
MODULE_DESCRIPTION("fd2fp kernel module");
MODULE_LICENSE("GPL");

typedef struct FD2FP_Device_s
{
  unsigned int  id;
  char          devName[FD2FP_DEV_NAME_LEN];
} FD2FP_Device_t;

static FD2FP_Device_t        fd2fpDevice[FD2FP_MAX_DEVICES];

/* ===============================================
   Name: 	Module file open operation
   Description: 
   =============================================== */
static int fd2fp_module_open (struct inode* inode, struct file* filp)
{
  int minor = MINOR(inode->i_rdev);

  printk( "module_open. Magic Number: 0x%x  MAJOR: 0x%x\n",FD2FP_MAGIC_NUMBER, FD2FP_MAJOR );
  filp->private_data = &fd2fpDevice[minor];
  return 0;
}

/* ===============================================
   Name: 	Module file ioctl operation 
   Description: 
   =============================================== */
static int fd2fp_module_ioctl(struct inode* inode, struct file* filp, unsigned int cmd, unsigned long arg)
{
  int               error    = 0;  /* No error */
  int               fd;
  struct file*      fp;
  struct inode*     node;
  FD2FP_Device_t*   fd2fpDev;
  int               major    = 0;
  int               minor    = 0;

  if ( _IOC_TYPE(cmd) != FD2FP_MAGIC_NUMBER )
  {
    printk( "Error : _IOC_TYPE(cmd)=%d != FD2FP_MAGIC_NUMBER=%d\n",  _IOC_TYPE(cmd),FD2FP_MAGIC_NUMBER );

    /* Not an ioctl for this module */
    error = -EINVAL;
    return 0;
  }

  switch( cmd )
  {
    case FD2FP_PASS_FD:
      if (__copy_from_user( &fd, (void*)arg, sizeof(int) ) != 0)
      {
           printk( "Unable to copy data from user\n" );
           return -EFAULT;
      }

      fp = fcheck( fd );
      if( fp != NULL )
      {
        fd2fpDev = fp->private_data;
        node     = fp->f_mapping->host;
        major    = imajor(node);
        minor    = iminor(node);
        printk( "fd: %d  (ID: %d  Name: %s) MAJOR: 0x%x MINOR: 0x%x\n", fd, fd2fpDev->id, fd2fpDev->devName, major, minor );
      }
      else
      {
        printk( "File pointer is NULL.\n" );
      }
      break;
    
    default:
      printk("Undefined command.\n");
      break;
  }
    
  return 0;
}

/* ===============================================
   Name: 	Module file release operation 
   Description: 
   =============================================== */
static int fd2fp_module_release(struct inode *inode, struct file *filp)
{
  printk( "module release\n" );

  filp->private_data = NULL;
  return 0;
}

struct file_operations fd2fp_module_fops =
{
  .owner   = THIS_MODULE,
  .llseek  = NULL,
  .read    = NULL,
  .write   = NULL,
  .ioctl   = fd2fp_module_ioctl,
  .poll    = NULL,
  .open    = fd2fp_module_open,
  .mmap    = NULL,
  .release = fd2fp_module_release,
};

/* ===============================================
   Name: 	Initialize the fd2fp devices 
   Description: 
   =============================================== */
static void fd2fp_device_reset(void)
{
  int i;
 
  for( i=0; i<FD2FP_MAX_DEVICES; i++ )
  {
    fd2fpDevice[i].id   = i;
    sprintf(fd2fpDevice[i].devName, "fd2fp%d\0", i);
  }
}

/* ===============================================
   Name: 	Module init operation 
   Description: 
   =============================================== */
int fd2fp_module_init(void)
{
  int error = 0; /* No error */
  dev_t baseDevNo;
  
  fd2fp_device_reset();

#if FD2FP_MAJOR

  error = register_chrdev(FD2FP_MAJOR, FD2FP_NAME, &fd2fp_module_fops);
  if(error < 0)
    return error;
	
#else

  /**
    * Register the major number. If major = 0 then a major number is auto
    * allocated. The allocated number is returned.
    * The major number can be seen in user space in '/proc/devices'
    */
  if (majorNumber == 0) 
  {
    if (alloc_chrdev_region(&baseDevNo, 0, FD2FP_MAX_DEVICES, FD2FP_NAME)) 
    {
      error = -EBUSY;
      printk("No major numbers by %s (pid %i)\n", current->comm, current->pid);
      goto fail;
    }
    majorNumber = MAJOR(baseDevNo);
  }
  else
  {
    baseDevNo = MKDEV(majorNumber, 0);

    if (register_chrdev_region(baseDevNo, FD2FP_MAX_DEVICES, FD2FP_NAME)) 
    {
      error = -EBUSY;
      printk( "No major numbers by %s (pid %i)\n", current->comm, current->pid);
      goto fail;
    }
  }

  if (NULL == (moduleCdev = cdev_alloc())) 
  {
    error = -EBUSY;
    printk( "No major numbers by %s (pid %i)\n", current->comm, current->pid);
    goto fail_reg;
  }

  moduleCdev->owner = THIS_MODULE;
  moduleCdev->ops   = &fd2fp_module_fops;
  kobject_set_name(&(moduleCdev->kobj), "dmx%d", 0);

  /*** Do the work ***/

  /* Appears in /var/log/syslog */
  printk("Load module %s [%d] by %s (pid %i)\n", FD2FP_NAME, majorNumber, current->comm, current->pid);

  /* Add the char device structure for this module */
  if (cdev_add(moduleCdev, baseDevNo, FD2FP_MAX_DEVICES)) 
  {

    error = -EBUSY;
    printk( "Failed to add a charactor device for %s by %s (pid %i)\n", FD2FP_NAME, current->comm, current->pid);
    goto fail_remap;
  }

  return (0);  /* If we get here then we have succeeded */

  /*** Clean up on error ***/
fail_remap:

  /* Remove the char device structure (has not been added) */
  kobject_put(&(moduleCdev->kobj));

fail_reg:

  /* Unregister the module */
  unregister_chrdev_region(MKDEV(majorNumber, 0), FD2FP_MAX_DEVICES);
	
fail : 
  return (error);    
	
#endif
   
  return 0;
}

/* ===============================================
   Name: 	Module exit operation
   Description: 
   =============================================== */
void fd2fp_module_exit(void)
{
  printk( "Unload module by %s (pid %i)\n",  current->comm, current->pid );
    
#if FD2FP_MAJOR
  unregister_chrdev(FD2FP_MAJOR, FD2FP_NAME);
#else
  /* Remove the char device structure (has been added) */
  cdev_del(moduleCdev);

  /* Unregister the module */
  unregister_chrdev_region(MKDEV(majorNumber, 0), FD2FP_MAX_DEVICES);
#endif
}

module_init(fd2fp_module_init);
module_exit(fd2fp_module_exit);


