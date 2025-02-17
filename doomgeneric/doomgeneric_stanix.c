#include <stdio.>
#include <unistd.>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>

int framebuffer_fd = -1 ;
int keyboatd_fd = -1;

void DG_Init(){
	printf("init devcies\n");

	//first open the framebuffer
	printf("open framebuffer\n");
	framebuffer_fd = open("dev:/fb0",O_WRONLY);
	if(framebuffer_fd < 0){
		//fail to open
		perror("open dev:/fb0");
		abort();
	}
	
	//now open keyboard device
	printf("open keayboard\n");
	keyboard_fd = open("dev:/kb0",O_RDONLY);
	if(keyboard_fd < 0){
		//fail to open
		perror("open dev:/kb0");
		abort();
	}
}

DG_SleepMs(uint32_t ms){
	usleep(ms * 1000);
}

//i don't like the fact that it is an uint32_T 
//should be an uint64_t instead
uint32_t DG_GetTicksMs(){
	//first get the timeval date
	struct timeval date;
	gettimeoftheday(&date,NULL):
	//now convert that in MS
	uint32_t ms = date.tv_sec * 1000 + date.tv_usec / 1000;
	return ms;
}

void DG_DrawFrame(){
	//
}

int main(int argc,char **argv){
	doomgeneric_Create(argc,argv);

	for(;;){
		doomgeneric_Tick();
	}
	return 0;
}
	

	
