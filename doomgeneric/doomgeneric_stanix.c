#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "doomgeneric.h"
#include "doomkeys.h"
#include "input.h"

int framebuffer_fd = -1 ;
int keyboard_fd = -1;

//todo get this with ioctl
size_t framebuffer_width  = 640;
size_t framebuffer_height = 480;

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
	gettimeofday(&current_time,NULL);
	start_ms = timeval2ms(&current_time);
}

void DG_SleepMs(uint32_t ms){
	usleep(ms * 1000);
}

//i don't like the fact that it is an uint32_T 
//should be an uint64_t instead
uint32_t DG_GetTicksMs(){
	//get the time
	struct timeval current_time;
	gettimeofday(&current_time,NULL);
	uint64_t ms = timeval2ms(&current_time) - start_ms;
	return (uint32_t)ms;
}

int DG_GetKey(int *pressed, unsigned char *key){
	char c = 0;
	ssize_t rsize = read(keyboard_fd,&c,1);

	//nothing to read = no key
	if(rsize < 1){
		return 0;
	}

	*pressed = c >> 8;
	switch (c & 0b01111111){
	case 'w':
		*key = KEY_UPARROW;
		break;
	case 'a':
		*key = KEY_LEFTARROW;
		break;
	case 's':
		*key = KEY_DOWNARROW;
		break;
	case 'd':
		*key = KEY_RIGHTARROW;
		break;
	case ' ':
		*key = KEY_USE;
		break;
	case 0x11:
		*key = KEY_FIRE;
		break;
	case 0x0F:
		*key = KEY_RSHIFT;
		break;
	case '\n':
		*key = KEY_ENTER;
		break;
	default:
		break;
	}

	return 1;
}

void DG_DrawFrame(){
	//copy to the framebuffer device
	for(size_t i = 0;i < DOOMGENERIC_RESY; i++){
		lseek(framebuffer_fd,i * framebuffer_width,SEEK_SET);
		write(framebuffer_fd, DG_ScreenBuffer + i * DOOMGENERIC_RESX ,DOOMGENERIC_RESX);
	}
}

void DG_SetWindowTitle(const char *title){
	//now window no problem
	(void)title;
}

void __tlibc_init(void);

int main(int argc,char **argv){
	__tlibc_init();
	doomgeneric_Create(argc,argv);

	for(;;){
		doomgeneric_Tick();
	}
	return 0;
}
	
//this functions don't exist on stanix so stub here

int system(const char *cmd){
    return 0;
}

int remove(const char *path){
    return -ENOENT;
}

int rename(const char *src,const char *dest){
	return -ENOSYS;
}

int sscanf(const char *buf,const char *fmt,...){
	return -1;
}