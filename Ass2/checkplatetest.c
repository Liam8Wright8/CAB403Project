#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

bool check_plate(char* cars){
	// Get file pointer
    FILE *plates = (FILE *)malloc(sizeof(FILE *));
	
    // Open file
    plates = fopen("./resources/plates.txt", "r");
    char* numplate=(char*)calloc(7,sizeof(char));
    
    if(plates == NULL)
        return 1;
    
    while(fgets(numplate, 7, plates)){
        if(strcmp(numplate,cars)==0){
			fclose(plates);
			printf(numplate);
			return true;
		}
    }
    fclose(plates);
    return false;
}

int main(void){
	char* c="449XLO";
	check_plate(c);
	
	return 0;
}
