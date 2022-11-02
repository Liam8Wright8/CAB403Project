#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "resources/shared_mem.h"
#include "resources/generatePlate.h"
#include "resources/hashTable.h"


int fd;
int i=0,add=0;
int notFull=0;
parking_data_t *shm; // Initilize Shared Memory Segment
pthread_mutex_t addMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hashMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t addCond=PTHREAD_COND_INITIALIZER;
// Function Help from https://qnaplus.com/c-program-to-sleep-in-milliseconds/ Author: Srikanta
// Input microseconds
void threadSleep(long tms)
{
    usleep(tms * 1000);
}

bool check_plate(char* cars){
	// Get file pointer
    
    if(cars==NULL){
		return false;
	}
    
    FILE *plates = (FILE *)malloc(sizeof(FILE *));
	
    // Open file
    plates = fopen("./resources/plates.txt", "r");
    char* numplate=(char*)calloc(7,sizeof(char));
    
    if(plates == NULL){
        return false;
    }
	while(fgets(numplate, 7, plates)!=NULL){
		if(strcmp(numplate,cars)==0){
		fclose(plates);
		return true;
		}
	}
    fclose(plates);
    return false;
}

// Read Shared Memory segment on startup.
void *read_shared_memory(parking_data_t *shm)
{

    // Using share name, return already created Shared Memory Segment.
    int open;
    open = shm_open(SHARE_NAME, O_RDWR, 0666);

    if (open < 0)
    {
        printf("Failed to create memory segment");
    }
    fd = open;

    // Map memory segment to physical address
    shm = mmap(NULL, SHMSZ, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shm == MAP_FAILED)
    {
        printf("FAILED TO MAP shared memory segment.\n");
    }
    printf("Created shared memory segment.\n");
    printf("ADDRESS OF PARKING %p\n\n", shm);

    return shm;
}

void *addToHash(void *hashpointer){
	htable_t *hash=hashpointer;
	pthread_mutex_lock(&numMutex);
	int a=add;
	add++;
	pthread_mutex_unlock(&numMutex);
	while(has_room(hash)){
	printf("%d \n",a);
	pthread_cond_wait(&shm->entrys[a].LPR_cond,&shm->entrys[a].LPR_mutex);
	printf("plates continued %d\n",a);
	if(check_plate(shm->entrys[a].lpr)){
		htable_add(hash,shm->entrys[a].lpr);
	}
	*shm->entrys[a].lpr=0;
	}
	notFull++;
	return 0;
}

void *gen_plates(){
	sleep(1);
	while(notFull==0){
		char* randPlate=(char*)malloc(6*sizeof(char));
		randPlate=generateNumberPlate();
		strncpy(shm->entrys[i].lpr,randPlate,6);
		if(!pthread_cond_signal(&shm->entrys[i].LPR_cond)){
			printf("thread signal %d\n",i);
		}
		threadSleep(200);
		i++;
		i=i%5;
		free(randPlate);
	}
	
	return 0;
}

int main()
{
	struct htable *hashtable=(htable_t*)malloc(sizeof(htable_t));
	pthread_t entThreads[Num_Of_Entries], addThreads;
    parking_data_t parking; // Initilize parking segment
    // Map Parking Segment to Memory and retrive address.
    shm = read_shared_memory(&parking);
	for(int l=0;l<5;l++){
		*shm->entrys[l].lpr=0;
	}
	
	htable_init(hashtable,(size_t)Num_Of_Level);
	
	for(int m=0;m<Num_Of_Entries;m++){
		if(pthread_create(&entThreads[m],NULL,addToHash,hashtable)){
			perror("Entrance Threads Failed");
		}
	}
	if(pthread_create(&addThreads,NULL,gen_plates,NULL)){
		perror("Hash Threads Failed");
	}
	while(true){
			for(int j=0;j<5;j++){
			printf("LPR ENT %d: %s\n",j,shm->entrys[j].lpr);
			
		}
		htable_print(hashtable);
		sleep(1);
		system("clear");
		
	}
	
    // USED TO TEST BOOMGATES. RUN THIS WITH SIM RUNNING ALREADY
    // testBoomgate();

    if ((munmap(shm, SHMSZ)) == -1)
    {
        perror("munmap failed");
    }
    htable_destroy(hashtable);
	
    close(fd);
    return 0;
}
