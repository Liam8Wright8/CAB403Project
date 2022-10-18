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
#define Max_Per_Level 20

int max_capacity=Num_Of_Level*Max_Per_Level;
int m=0,n=0,o=0;
pthread_mutex_t mutex_m,mutex_n,mutex_o;
pthread_t en[Num_Of_Entries],b_en[Num_Of_Exits],sign[Num_Of_Level];		//Entry
pthread_t ex[Num_Of_Entries],b_ex[Num_Of_Exits];							//Exit
pthread_t le[Num_Of_Entries],sensor[Num_Of_Exits],temp_alarm[Num_Of_Level];	//Level

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
typedef struct{
	
}car_list;
	level lev[Num_Of_Level];
    entry In[Num_Of_Entries];
	exits Out[Num_Of_Exits];
void *entry_init(void *shm){
	pthread_mutex_lock(&mutex_m);
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
	printf("En %d\n ",m);
	m++;
	pthread_mutex_unlock(&mutex_m);
}
void *exit_init(void *shm){
	pthread_mutex_lock(&mutex_n);
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
	printf("Ex %d\n",n);	
	n++;
	pthread_mutex_unlock(&mutex_n);
}
void *level_init(void *shm){
	pthread_mutex_lock(&mutex_o);
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
	printf("Lev %d\n",o);
	o++;
	pthread_mutex_unlock(&mutex_o);
}
void init_threads(void *shm){
	for(int x=0;x<Num_Of_Entries;x++){
		if((pthread_create(&en[x],NULL,&entry_init,shm))!=0){
			perror("Entry thread failed creation");
		}
	}
	for(int y=0;y<Num_Of_Exits;y++){
		if((pthread_create(&ex[y],NULL,&exit_init,shm))!=0){
			perror("Exit thread failed creation");
		}
	}
	for(int z=0;z<Num_Of_Level;z++){
		if((pthread_create(&le[z],NULL,&level_init,shm))!=0){
			perror("Level thread failed creation");
		}
	}
	while((m+n+o)!=12){}
	for(int a=0;a<Num_Of_Entries;a++){
		if((pthread_create(&en[a],NULL,&entry_init,shm))!=0){
			perror("Entry thread failed creation");
		}
		if((pthread_create(&ex[a],NULL,&exit_init,shm))!=0){
			perror("Entry thread failed creation");
		}
		if((pthread_create(&le[a],NULL,&level_init,shm))!=0){
			perror("Entry thread failed creation");
		}
	}
	/* LPR*3
	 * Boom*2
	 * Display
	 * tempsensor
	 * alarm
	 * */
	
}
void read_boom(){
	//entrance and exit boomgate watch
}
void LPR(){
	//3 lprs 
}
void display_sign(){
	//only entrance. 
	//How full each level is, status of boomgates, allow in or not signs, temp sensor, alarm status, revenue
}
void level_alarms(){
	//read temp 
}

int main()
{
    int shm_fd;
    const char *key;
    char *shm, *s;
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
    init_threads(shm);
	
	if((munmap (shm, SHMSZ )) ==-1){
		perror("munmap failed");
	} 
	close(shm_fd);
	return 0;
}
