#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    int fd = open("/dev/ttyS0", O_RDWR);

    if(fd < 0) {
        fprintf(stderr, "unable to open /dev/ttsy0");
    }

    struct termios t;

    // copy current configuration to tty
    tcgetattr(fd, &t);

    // disbale translation and flow control
    // disapble post processing
    // disable canonical mode, signals, echo, extensions
    // disable parity and set data size to 8 bit
    // set VMIN = 1, VTIME = 0
    cfmakeraw(&t);

    // for non-blocking behaviour
    t.c_cc[VMIN] = 0;

    // clear stop bit to have 1 stob bit
    t.c_cflag &= ~(CSTOPB);
}