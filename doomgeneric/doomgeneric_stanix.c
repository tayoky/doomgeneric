#include <errno.h>
#include <fcntl.h>
#include <input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "doomgeneric.h"
#include "doomkeys.h"

#define ESC '\033'

int framebuffer_fd = -1;
int keyboard_fd = -1;

// todo get this with ioctl
size_t framebuffer_width = 640;
size_t framebuffer_height = 480;

// keep track of when the game started
uint64_t start_ms;

// convert an timeval to ms
uint64_t timeval2ms(struct timeval *time) {
    return time->tv_sec * 1000 + time->tv_usec / 1000;
}

void DG_Init() {
    printf("init devcies\n");

    // first open the framebuffer
    printf("open framebuffer\n");
    framebuffer_fd = open("dev:/fb/00", O_WRONLY);
    if (framebuffer_fd < 0) {
        // fail to open
        perror("open dev:/fb/00");
        abort();
    }

    // now open keyboard device
    printf("open keyboard\n");
    keyboard_fd = open("dev:/kb0", O_RDONLY);
    if (keyboard_fd < 0) {
        // fail to open
        perror("open dev:/kb0");
        abort();
    }

    // get the time
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    start_ms = timeval2ms(&current_time);
}

void DG_SleepMs(uint32_t ms) { usleep(ms * 1000); }

// i don't like the fact that it is an uint32_T
// should be an uint64_t instead
uint32_t DG_GetTicksMs() {
    // get the time
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    uint64_t ms = timeval2ms(&current_time) - start_ms;
    return (uint32_t)ms;
}

#define CHECK(str) !memcmp(str, &c[2], 4)

int DG_GetKey(int *pressed, unsigned char *key) {
    struct input_event event;
    if (read(keyboard_fd, &event, sizeof(event)) < 1) {
        // no key to read
        return 0;
    }

    // ignore not key event
    if (event.ie_type != IE_KEY_EVENT) {
        return 0;
    }

    *pressed = event.ie_key.flags & KEY_PRESS;
    if (event.ie_key.flags & KEY_GRAPH) {
        switch () {
            case 'w':
                *key = KEY_UPARROW;
                return 1;
            case 'a':
                *key = KEY_LEFTARROW;
                return 1;
            case 's':
                *key = KEY_DOWNARROW;
                return 1;
            case 'd':
                *key = KEY_RIGHTARROW;
                return 1;
            case ' ':
                *key = KEY_USE;
                return 1;
            case '\n':
                *key = KEY_ENTER;
                break;
        }
    }

    // check for special keys
    switch (event.ie_key.scancode) {
        case 0x02:
            *key = KEY_ESCAPE;
            return 1;
        case 0x1E:
            *key = KEY_RSHIFT;
            return 1;
        case 0x2B:
            *key = KEY_RSHIFT;
            return 1;
    }

    return 0;
}

void DG_DrawFrame() {
    // copy to the framebuffer device
    for (size_t i = 0; i < DOOMGENERIC_RESY; i++) {
        lseek(framebuffer_fd, i * framebuffer_width, SEEK_SET);
        write(framebuffer_fd, DG_ScreenBuffer + i * DOOMGENERIC_RESX,
              DOOMGENERIC_RESX);
    }
}

void DG_SetWindowTitle(const char *title) {
    // no window no problem
    (void)title;
}

void __tlibc_init(void);

int main(int argc, char **argv) {
    printf("launching doom...\n");
    printf("argc : %d\n", argc);
    __tlibc_init();

    doomgeneric_Create(argc, argv);

    for (;;) {
        doomgeneric_Tick();
    }
    return 0;
}

// this functions don't exist on stanix so stub here

int system(const char *cmd) { return 0; }

int remove(const char *path) { return -ENOENT; }

int rename(const char *src, const char *dest) { return -ENOSYS; }

int sscanf(const char *buf, const char *fmt, ...) { return -1; }
