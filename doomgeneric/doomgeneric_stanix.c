#include <stdio.>
#include <unistd.>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>

int framebuffer_fd = -1 ;
int keyboard_fd = -1;

//todo get this with ioctl
size_t framebuffer_width  = 640;
size_t framebuffer_height = 480:

//keep track of when the game started
uint64_t start_ms;

//convert an timeval to ms
uint64_t timeval2ms(struct timeval *time){
	return time->tv_sec * 1000 + time->tv_usec / 1000;
}

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

	//get the time
	struct timeval current_time;
	gettimeoftheday(&current_time,NULL):
	start_ms = timeval2ms(&current_time);
}

DG_SleepMs(uint32_t ms){
	usleep(ms * 1000);
}

//i don't like the fact that it is an uint32_T 
//should be an uint64_t instead
uint32_t DG_GetTicksMs(){
	//get the time
	struct timeval current_time;
	gettimeoftheday(&current_time,NULL):
	uint64_t ms = timeval2ms(&current_time) - start_ms;
	return (uint32_t)ms;
}

void DG_DrawFrame(){
	//copy to the framebuffer device
	for(size_t i = 0;i < DOOMGENERIC_RESY; i++){
		lseek(framebuffer_fd,i * framebuffer_width,SEEK_SET);
		write(framebuffer_fd, DG_ScreenBuffer + i * DOOMGENERIC_RESX ,DOOMGENERIC_RESX);
	}
}

int main(int argc,char **argv){
	doomgeneric_Create(argc,argv);

	for(;;){
		doomgeneric_Tick();
	}
	return 0;
}
	

	
