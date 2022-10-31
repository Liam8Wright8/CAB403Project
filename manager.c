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
parking_data_t *shm; // Initilize Shared Memory Segment
int inloop,outloop,boomloop;
pthread_mutex_t hash=PTHREAD_MUTEX_INITIALIZER,incount=PTHREAD_MUTEX_INITIALIZER;
char *cars[5]; 
pthread_cond_t hashadd=PTHREAD_COND_INITIALIZER;


// Function Help from https://qnaplus.com/c-program-to-sleep-in-milliseconds/ Author: Srikanta
// Input microseconds
void threadSleep(long tms)
{
    usleep(tms * 1000);
}

bool check_plate(char* cars){
	// Get file pointer
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

// TESTING BOOMGATE COMMUNICATION WITH SIMULATOR ON EXIT 1
// CHANGE THE INDEX TO WHATEVER NUMBER YOU WANT TO TEST
void testBoomgate()

{
    // TEST BOOMGATE OPENING
    printf("RAISING BOOMGATE...\n");
    pthread_mutex_lock(&shm->exits[0].boomgate_mutex);
    shm->exits[0].boomgate = 'R';
    pthread_cond_broadcast(&shm->exits[0].boomgate_cond);
    pthread_mutex_unlock(&shm->exits[0].boomgate_mutex);
    printf("Current Status of BOOMGATE %c\n\n", shm->exits[0].boomgate);

    // CAR CAN ONLY DRIVE IN WHEN GATE IS OPEN
    printf("WAITING ON BOOMGATE TO BE OPENED...\n");
    while (shm->exits[0].boomgate == 'R')
    {
        pthread_cond_wait(&shm->exits[0].boomgate_cond, &shm->exits[0].boomgate_mutex);
    }
    if (shm->exits[0].boomgate == 'O')
    {
        printf("CAR DRIVING IN\n\n");
        threadSleep(10); // Ten Milliseconds for car to drive in
    }
    // TEST BOOMGATE CLOSING
    printf("LOWERING BOOMGATE...\n");
    pthread_mutex_lock(&shm->exits[0].boomgate_mutex);
    shm->exits[0].boomgate = 'L';
    pthread_cond_broadcast(&shm->exits[0].boomgate_cond);
    pthread_mutex_unlock(&shm->exits[0].boomgate_mutex);
    printf("BOOMGATE IS CLOSED\n");
    sleep(1);
    printf("Current Status of BOOMGATE %c\n", shm->exits[0].boomgate);
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

void *display_sign(void *arg){
	//parking_data_t *shm = (parking_data_t*)arg;
	for (;;)
    {
         // Display Entrances
         printf("----------------ENTRANCES--------------\n");
         for(int i=0; i<5; i++){
			 printf("LEVEL 1 LPR: %s     |     LEVEL 1 BG: %c\n", shm->entrys[i].lpr, shm->entrys[i].boomgate);
		 }    
         printf("---------------------------------------\n");
         // Display Exits
         printf("------------------EXITS----------------\n");
         for(int i=0; i<5; i++){
			 printf("LEVEL 1 LPR: %s     |     LEVEL 1 BG: %c\n", shm->exits[i].lpr, shm->exits[i].boomgate);
		 }
         printf("---------------------------------------\n");
         // Display Temperature
         printf("              ----TEMP---\n");
         for(int i=0; i<5; i++){
         printf("              LEVEL 1 : %dC\n", shm->levels[i].temp);
			 
		 }
         printf("---------------------------------------\n");        
         threadSleep(100); // Updates Every 'x' amount of milliseconds
         system("clear");
     } 
     return 0;
}
void *lpr_ins(){
	while(true){
	char* howdy=(char*)malloc(7*sizeof(char));
	howdy=generateNumberPlate();
	if(check_plate(howdy)){
		strcpy(shm->entrys[inloop].lpr,howdy);
		pthread_cond_signal(&hashadd);
		threadSleep(1000);
	}
	else{
		
		
	}
	free(howdy);
	inloop++;
	inloop=inloop%5;
	

	}
	return 0;
}
void *lpr_outs(){

	return 0;
}
	void *booms(void *arg){
		
	return 0;
}


int main()
{
	struct htable *hashtable=(htable_t*)malloc(sizeof(htable_t));
	
	pthread_t lpr_in,lpr_out, boom;//, hash_in, hash_out;
	
    parking_data_t parking; // Initilize parking segment
    
    // Map Parking Segment to Memory and retrive address.
    shm = read_shared_memory(&parking);
	int in, out, boom_ret;
	/*int run;
	pthread_t display;
	if((run=pthread_create(&display, NULL, display_sign, shm)) !=0){
		perror("Well shit");
	}*/
	htable_init(hashtable,(size_t)5);
	if((in = pthread_create(&lpr_in, NULL, lpr_ins, NULL))!=0){
		perror("lpr_in fail");
		};
	if((out = pthread_create(&lpr_out, NULL, lpr_outs, NULL))!=0){
		perror("lpr_out fail");
		};
	if((boom_ret = pthread_create(&boom, NULL, booms, NULL))!=0){
		perror("boom fail");
		};
	
	while(true){
		pthread_cond_wait(&hashadd, &hash);
			if((htable_find(hashtable, shm->entrys[inloop].lpr))==NULL){
				htable_add(hashtable, shm->entrys[inloop].lpr);
			}
		htable_print(hashtable);

	}

	
	
    // USED TO TEST BOOMGATES. RUN THIS WITH SIM RUNNING ALREADY
    // testBoomgate();

    if ((munmap(shm, SHMSZ)) == -1)
    {
        perror("munmap failed");
    }
    htable_destroy(hashtable);
	pthread_mutex_destroy(&hash);
	pthread_cond_destroy(&hashadd);
    close(fd);
    return 0;
}
