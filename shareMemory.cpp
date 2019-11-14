#include"shareMemory.h"

//create share memory
int createShareMemory(int partNum){

    int key = (partNum+1)*1000;

    int shmid = shmget((key_t)key, sizeof(struct ShareMemory), 0666|IPC_CREAT);

    //Share memory create fail!
    if(shmid == -1)
    	{
    		fprintf(stderr, "shmget failed\n");
    		exit(EXIT_FAILURE);
    	}
    return shmid;
}

void shareMemoryWrite(int shmid, RealRect aimRect){
    //Connect share memory to this process
    void* shm = shmat(shmid, (void*)0, 0);
    int running = 1;
    if(shm == (void*)-1)
    {
    	fprintf(stderr, "shmat failed\n");
    	exit(EXIT_FAILURE);
    }
    ShareMemory* sha = (struct ShareMemory*)shm;
    while (running)
    {
        sha->num = 0;
    	//if writing is 1,wait
    	int waitNum = 0;
    	while (sha->isWriting == 1)
    	{
    		//if wait time more than 500ms,it maybe deallock
    		if(waitNum>=10) break;
    		usleep(50);
    		waitNum +=1;
    		printf("shareMemory is busy,wait 50ms\n");
    	}

    	//begin write
    	sha->isWriting = 1;
    	//input data to share memory
    	for (int aimNum = 0; aimNum < aimRect.num; aimNum++)
    	{
    		sha->GdiArray[aimNum] = aimRect.GdiArray[aimNum];
            sha->num++;
    	}
    //write finshed
    running = 0;
    sha->isWriting = 0;
    }

    //detach share memory
    shmdt(shm);
    printf("memory write finshed!\n");
}

ShareMemory shareMemoryRead(int shmid){
	ShareMemory shareCopy;
	shareCopy.num = 0;
	int running = 1;
	//connect share memory to this process
	void* shm = shmat(shmid, (void*)0, 0);
	ShareMemory* sha = (struct ShareMemory*)shm;
	while (running)
		{
			//if writing is 1,wait
			int waitNum = 0;
			while (sha->isWriting == 1)
			{
				//if wait time more than 500ms,it maybe deallock
				if(waitNum>=10) break;
				usleep(50);
				waitNum +=1;
				printf("shareMemory is busy,wait 50ms\n");
			}
			//begin write
			sha->isWriting = 1;
			//input data to share memory
			for (int aimNum = 0; aimNum < sha->num; aimNum++)
			{
				shareCopy.GdiArray[aimNum] = sha->GdiArray[aimNum];
				shareCopy.num ++;
			}
		//read finshed
		running = 0;
		sha->isWriting = 0;
	}
    //detach share memory
    shmdt(shm);
	return shareCopy;
}

void destoryShareMemory(int shmid)
{
    //delete share memory
    if(shmctl(shmid, IPC_RMID, 0) == -1)
    {
    	fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    	exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}