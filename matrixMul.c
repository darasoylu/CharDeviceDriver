#include <linux/init.h> // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/device.h> // Header to support the kernel Driver Model
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/fs.h> // Header for the Linux file system support
#include <asm/uaccess.h> // Required for the copy to user function

#define DEVICE_NAME "matrixMulDevice"
#define CLASS_NAME "matrixMulClass"

MODULE_LICENSE("GPL"); // The license type
MODULE_AUTHOR("Dara Soylu"); // The author
MODULE_DESCRIPTION("A simple Linux char driver"); // The description

static int M, N; // Stores the matrix's row and column
static int i = 0, j = 0, k = 0, l = 0, p = 0; // Required values for the code
static char temp[256]; // Stores the temp strings in code
static char message[256]; // Memory for the string that is passed from userspace
static int majorNumber; // Stores the device number 
static int numberOpens = 0; // Counts the number of times the device is opened
static struct class * matrixMulClass = NULL; // The device-driver class struct pointer
static struct device * matrixMulDevice = NULL; // The device-driver  struct pointer

// The prototype functions for the character driver
static int dev_open(struct inode * , struct file * );
static int dev_release(struct inode * , struct file * );
static ssize_t dev_read(struct file * , char * , size_t, loff_t * );
static ssize_t dev_write(struct file * , const char * , size_t, loff_t * );
static int intToString(char *);

// Integer degerleri stringe cevirmek icin intToString fonksiyonu
static int intToString(char *ptr){
    int t, res = 0;
    for (t = 0; ptr[t] != '\0'; ++t) {
        res = res * 10 + ptr[t] - '0';
      }
    return res;
}

static struct file_operations fops = {
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};

static int __init matrixMulDevice_init(void) {
  printk(KERN_INFO "matrixMulDevice: Initializing the device\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, & fops);
  if (majorNumber < 0) {
    printk(KERN_ALERT "matrixMulDevice failed to register a major number\n");
    return majorNumber;
  }
  printk(KERN_INFO "matrixMulDevice: Device registered\n");

  matrixMulClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(matrixMulClass)) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(matrixMulClass);
  }
  printk(KERN_INFO "matrixMulDevice: Device class registered correctly\n");

  matrixMulDevice = device_create(matrixMulClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(matrixMulDevice)) { 
    class_destroy(matrixMulClass); 
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(matrixMulDevice);
  }
  printk(KERN_INFO "matrixMulDevice: Device class created correctly\n");
  
  return 0;
}

static void __exit matrixMulDevice_exit(void) {
  device_destroy(matrixMulClass, MKDEV(majorNumber, 0)); // remove the device
  class_unregister(matrixMulClass); // unregister the device class
  class_destroy(matrixMulClass); // remove the device class
  unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
  printk(KERN_INFO "matrixMulDevice: Device unregistered!\n");
}

static int dev_open(struct inode * inodep, struct file * filep) {
  numberOpens++;
  printk(KERN_INFO "matrixMulDevice: Device has been opened %d time(s)\n", numberOpens);
  
  return 0;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset) {
  copy_to_user(buffer, message, len);
  
  return len;
}

static ssize_t dev_write(struct file * filep, const char * buffer, size_t len, loff_t * offset) {
  if (copy_from_user(message, buffer, len)) {
    return -EFAULT;
  }
  
  
  // "M N*matrix1*matrix2*" formatindaki message'i parse ederiz.
  // Once M ve N degerlerini aliriz.
  // M->satir sayisi N->sutun sayisi
  // M ve N degerlerini aldiktan sonra int matrix1[M][N] ve int matrix2[N][1] tanımlarız.
  // Bu matrixlere degerlerini sırasıyla message stringini parse ederek veririz.
  // matrix1 ve matrix2 matrislerini carpariz ve sonucu mulMatrix matrisinde tutariz.
  // mulMatrixin elemanlarini satir satir loga yazdırırız.

  while(1){
        if(message[i] == ' '){
       	    strncpy(temp,&message[0],i);
            temp[i] = '\0';
            M = intToString(temp); 
            break;
        }
       i++;
  }
  
    i++;
    j=i;
    memset(temp,0,256);

    while(1){
        if(message[j] == '*'){
            strncpy(temp,&message[i],j-i);
            temp[j-i] = '\0';
            N = intToString(temp);
            break;
        }
        j++;
    }
    
    memset(temp,0,256);
    
    int matrix1[M][N], matrix2[N][1];
    
    j++;
    p = j;
    while(1){
        if(message[p] == '*'){
            break;
        }
        if(message[p] == ' '){
            strncpy(temp,&message[j],p-j);
            if(k<M){
                if(l<N){
                    matrix1[k][l] = intToString(temp);
                    memset(temp,0,sizeof(temp));
                    j = p + 1;
                    l++;
                    if(l==N){
                        k++;
                        l=0;
                    }
                }
            }
        }
        p++;
        
    }
    
    j++;
    p = j;
    i=0;
    
    while(1){
        if(message[p] == '*'){
            break;
        }
        if(message[p] == ' '){
            strncpy(temp,&message[j],p-j);
            if(i<N){
            	matrix2[i][0] = intToString(temp);
                memset(temp,0,sizeof(temp));
                i++;
                j = p + 1; 
            }
        }
        p++;
        
    }
    
    int mul[M][1];
    
    for(i=0;i<M;i++)    
    {    
        mul[i][0]=0;
        for(j=0;j<N;j++)    
        {    
            mul[i][0]+=matrix1[i][j]*matrix2[j][0];
        }    
    }   
    
    for(i=0;i<M;i++){
        printk(KERN_INFO"MatrixMulDevice: Matrik Multiplication[%d]->%d\n", i, mul[i][0]);
    }

  return len;
}

static int dev_release(struct inode * inodep, struct file * filep) {
  printk(KERN_INFO "matrixMulDevice: Device closed\n");
  
  return 0;
}

module_init(matrixMulDevice_init);
module_exit(matrixMulDevice_exit);
