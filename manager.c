#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>


#define SHMSZ 2920
#define Num_Of_Entries 5
#define Num_Of_Exits 5
#define Num_Of_Level 5

typedef struct {
	pthread_mutex_t *LPR_mutex, *boomgate_mutex,*info_mutex;
	pthread_cond_t *LPR_cond,*boomgate_cond,*info_cond;
	char *lpr[6];
	char *boomgate,*display;
}entry ;
typedef struct {
	pthread_mutex_t *LPR_mutex, *boomgate_mutex;
	pthread_cond_t *LPR_cond,*boomgate_cond;
	char *lpr[6];
	char *boomgate;
} exits;
typedef struct {
	pthread_mutex_t *LPR_mutex;
	pthread_cond_t *LPR_cond;
	char *lpr[6];
	char *alarm;
	int16_t *temp;
}level ;
	int m=0,n=0,o=0;
	level lev[Num_Of_Level];
    entry In[Num_Of_Entries];
	exits Out[Num_Of_Exits];
void *entry_init(void *shm){
	char *s;
	s=shm+288*m;								//Entry LPR Mutex
	In[m].LPR_mutex=(pthread_mutex_t*)s;
	s=shm+40+288*m;								//Entry LPR Condition Variable
	In[m].LPR_cond=(pthread_cond_t*)s;
	s=shm+(88+288*m);							//Entry LPR Values 
	for(int i=0;i<6;i++){
		In[m].lpr[i]=s;
		s++;
	}
	s=shm+96+288*m;								//Entry Boomgate Mutex
	In[m].boomgate_mutex=(pthread_mutex_t*)s;
	s=shm+136+288*m;							//Entry Boomgate Condition Variable
	In[m].boomgate_cond=(pthread_cond_t*)s;
	s=shm+184+288*m;							//Entry Boomgate status
	In[m].boomgate=s;
	s=shm+192+288*m;							//Entry Display Mutex
	In[m].info_mutex=(pthread_mutex_t*)s;
	s=shm+232+288*m;							//Entry Display Condition Variable
	In[m].info_cond=(pthread_cond_t*)s;
	s=shm+280+288*m;   							//Entry Display status
	In[m].display=s;
	printf("Success");
}
void *exit_init(void *shm){
	char *s;
	s=shm+1440+192*n;							//Exit LPR Mutex
	Out[n].LPR_mutex=(pthread_mutex_t*)s;
	s=shm+1440+40+192*n;						//Exit LPR Condition Variable
	Out[n].LPR_cond=(pthread_cond_t*)s;
	s=shm+1440+88+192*n;						//Exit LPR Values
	for(int i=0;i<6;i++){
		Out[n].lpr[i]=s;
		s++;
	}
	s=shm+1440+96+192*n;						//Exit Boomgate Mutex
	Out[n].boomgate_mutex=(pthread_mutex_t*)s;
	s=shm+1440+136+192*n;						//Exit Boomgate Condition Variable
	Out[n].boomgate_cond=(pthread_cond_t*)s;
	s=shm+1440+184+192*n;						//Exit Boomgate status
	Out[m].boomgate=s;	
	printf("Success");
}
void *level_init(void *shm){
	char *s;
	s=shm+2400+104*o;							//Level LPR Mutex
	lev[o].LPR_mutex=(pthread_mutex_t*)s;
	s=shm+2400+40+104*o;						//Level LPR Condition Variable
	lev[o].LPR_cond=(pthread_cond_t*)s;
	s=shm+2400+88+104*o;						//Level LPR values
	for(int i=0;i<6;i++){
		lev[o].lpr[i]=s;
		s++;
	}
	s=shm+2400+96+104*o;						//Level Temp Values
	lev[o].temp=(int16_t*)s;
	s=shm+2400+98+104*o;						//Level Alarm value
	lev[o].alarm=s;
	printf("Success");
}

int main()
{
	char c;
    int shm_fd;
    const char *key;
    char *shm, *s;
	pthread_t en[Num_Of_Entries],ex[Num_Of_Exits],le[Num_Of_Level];
	//struct exits Out;
    /*
     * We'll name our shared memory segment
     * "SHM_TEST".
     */
    key = "SHM_TEST";
    /*
     * Using *key="SHM_TEST" and both creating and setting to read and write
     * Read and write for owner, group (0660)
     * Fail if negative int is returned
     */
	if ((shm_fd = shm_open(key, O_CREAT | O_RDWR, 0660)) < 0)
    {
        perror("shm_open");
        exit(1);
    }
	/*
     * Configure the size of the shared memory segment to 2920 bytes
     */
    ftruncate(shm_fd, SHMSZ);
    
    if ((shm = mmap(0, SHMSZ, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == (char *)-1)
    {
        perror("mmap");
        exit(1);
    }
    for(int x=0;x<Num_Of_Entries;x++){
		pthread_create(&en[x],NULL,&entry_init,shm);
	}
	for(int y=0;y<Num_Of_Exits;y++){
		pthread_create(&ex[y],NULL,&exit_init,shm);
	}
	for(int z=0;z<Num_Of_Level;z++){
		pthread_create(&le[z],NULL,&level_init,shm);
	}
	return 0;
}
