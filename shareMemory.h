#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#define MAX_NUM_RENDER  1024
#define MID_NUM_RENDER  256
#define MIN_NUM_RENDER  64

struct GdiCRect
{
	int left;
	int top;
	int right;
	int bottom;
};

//Haar detection result
struct RealRect{
	GdiCRect GdiArray[MIN_NUM_RENDER];
	int num;
};

//Struct of share memory
struct ShareMemory{
	GdiCRect GdiArray[MIN_NUM_RENDER];
	int num;
	int isWriting;//1 is writting
};

//Create share memory
int createShareMemory(int partNum);

//Write share memory
void shareMemoryWrite(int shmid, RealRect aimRect);

//Read share memory
ShareMemory shareMemoryRead(int shmid);

//Control share memory
void destoryShareMemory(int shmid);