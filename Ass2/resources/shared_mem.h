#include <stdio.h>
#ifndef SHARED_MEM
#define SHARED_MEM

//////////////////Accessing Shared Memory Segments//////////////////////////////////////

/////////////////USING THE SRUCTURES TO GET VALUE AND SET//////////////////////////////

// shm->entry[EntryNum].variable     Where EntryNum is 0-4 for Entry 1-5 respectively
// shm->exit[ExitNum].variable     Where Exit Num is 0-4 for Exit 1-5 respectively
// shm->level[LevelNum].variable     Where LevelNum is 0-4 for Level 1-5 respectively

/////////////////USING THE SRUCTURES TO GET ADDRESS//////////////////////////////

// &shm->entry[EntryNum].variable     Where EntryNum is 0-4 for Entry 1-5 respectively
// &shm->exit[ExitNum].variable     Where Exit Num is 0-4 for Exit 1-5 respectively
// &shm->level[LevelNum].variable     Where LevelNum is 0-4 for Level 1-5 respectively

////////////////USING ADDRESSES DIRECTLY TO GET ADDRESS///////////////////////////////

// char *addr = (CastTypeHere *)shm + offset * NUM              WHERE NUM is Entry/Exit/Level NUM 0-4

/*
*	//////////////////ENTRY Offsets/////////////////
	///USE THESE VALUES AND REPLACE offset placeholder above

	288 - Mutex_t LPR
	40 + 288 - Mutex Condition LPR
	88 + 288 - LPR Values
	96 + 288 - Mutex_t Boomgate
	136 + 288 - Mutex Condition Boomgate
	184 + 288 - Status BoomGate
	192 + 288 pthread_mutex_t for information sign
	232 + 288 pthread_cond_t for information sign
	280 + 288 character display for information sign

	//////////////////EXIT Offsets//////////////////
	1440 + 192 pthread_mutex_t for LPR
	1480 + 40 + 192 pthread_cond_t for LPR
	1528 + 88 + 192 license plate reading for LPR
	1536 + 96 + 192 pthread_mutex_t for boom gate
	1576 + 136 + 192 pthread_cond_t for boom gate
	1624 + 184 + 192 status character for boom gate

	///////////////////Level Offsets/////////////////

	2400 + 104 pthread_mutex_t for LPR
	2440 + 40 + 104 pthread_cond_t for LPR
	2488 + 88 + 104 license plate reading
	2496 + 96 + 104 temperature sensor
	2498 + 98 + 104 fire alarm
*/

#include <sys/types.h>
#include <pthread.h>

#define SHARE_NAME "PARKING" // Shared Memory Name
#define SHMSZ 2920			 // Size of Shared Memory to be allocated
#define Num_Of_Entries 5	 // Number of entriess
#define Num_Of_Exits 5		 // Number of Exits
#define Num_Of_Level 5		 // Num of Car Park Levels
#define Max_Per_Level 20	 // Number of Max Car Parks

/*
 * Structs Types for Entries, Exits and Levels
 */
typedef struct entrys
{
	pthread_mutex_t LPR_mutex;
	pthread_cond_t LPR_cond;
	char lpr[6];
	char padd[2];
	pthread_mutex_t boomgate_mutex;
	pthread_cond_t boomgate_cond;
	char boomgate;
	pthread_mutex_t info_mutex;
	pthread_cond_t info_cond;
	char display;
} entrys_t;

typedef struct exits
{
	pthread_mutex_t LPR_mutex;
	pthread_cond_t LPR_cond;
	char lpr[6];
	char padd[2];
	pthread_mutex_t boomgate_mutex;
	pthread_cond_t boomgate_cond;
	char boomgate;
} exits_t;

typedef struct levels
{
	pthread_mutex_t LPR_mutex;
	pthread_cond_t LPR_cond;
	char lpr[6];
	char padd[2];
	int16_t temp;
	char alarm;
	char padding[5];
} levels_t;

/**
 * Parking Data Block
 */
typedef struct shared_data
{

	// Entry Struct
	entrys_t entrys[Num_Of_Entries];

	// Exit Struct
	exits_t exits[Num_Of_Exits];

	// Level Struct
	levels_t levels[Num_Of_Level];

} parking_data_t;

// Set Up Mutex/Condition Variables By Default
void setDefaultValues(parking_data_t *shm)
{

	pthread_mutexattr_t mta;
	pthread_condattr_t cta;
	pthread_mutexattr_setpshared(&mta, PTHREAD_PROCESS_SHARED);
	pthread_condattr_setpshared(&cta, PTHREAD_PROCESS_SHARED);

	// Entries
	for (int i = 0; i < Num_Of_Entries; i++)
	{
		// BoomGate
		pthread_mutex_init(&shm->entrys[i].boomgate_mutex, &mta);
		pthread_cond_init(&shm->entrys[i].boomgate_cond, &cta);
		shm->entrys[i].boomgate = 'C'; // Set default gate to closed.

		// LPR
		pthread_mutex_init(&shm->entrys[i].LPR_mutex, &mta);
		pthread_cond_init(&shm->entrys[i].LPR_cond, &cta);

		// Information Sign
		pthread_mutex_init(&shm->entrys[i].info_mutex, &mta);
		pthread_cond_init(&shm->entrys[i].info_cond, &cta);
	}

	// Exits
	for (int i = 0; i < Num_Of_Exits; i++)
	{
		// BoomGate
		pthread_mutex_init(&shm->exits[i].boomgate_mutex, &mta);
		pthread_cond_init(&shm->exits[i].boomgate_cond, &cta);
		shm->exits[i].boomgate = 'C'; // Set default gate to closed.

		// LPR
		pthread_mutex_init(&shm->exits[i].LPR_mutex, &mta);
		pthread_cond_init(&shm->exits[i].LPR_cond, &cta);
	}

	// Level
	for (int i = 0; i < Num_Of_Level; i++)
	{
		// LPR
		pthread_mutex_init(&shm->levels[i].LPR_mutex, &mta);
		pthread_cond_init(&shm->levels[i].LPR_cond, &cta);
	}
	printf("All mutexes created.\n");
}

#endif
