#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

int tty_config(struct termios *t);

int main() {

    int fd = open("/tmp/ttyS0", O_RDWR);

    if(fd < 0) {
        perror("unable to open /tmp/tty0");
        return -1;
    }


    struct termios t;
    // copy current configuration to tty
    if (tcgetattr(fd, &t) < 0) {
        perror("tcgetattr");
        return -1;
    }
    if (tty_config(&t) < 0) {
        fprintf(stderr, "could not conplete tty_config");
    }

    if (tcsetattr(fd, TCSANOW, &t) < 0) {
        perror("tcsetattr");
    }
}


int tty_config(struct termios *t) {

    // disbale translation and flow control
    // disapble post processing
    // disable canonical mode, signals, echo, extensions
    // disable parity and set data size to 8 bit
    // set VMIN = 1, VTIME = 0
    cfmakeraw(t);

    // for non-blocking behaviour
    t->c_cc[VMIN] = 0;

    // clear stop bit to have 1 stob bit
    t->c_cflag &= ~(CSTOPB);

    // set baudrate
    if (cfsetispeed(t, B115200) < 0) {
        perror("cfsetispeed");
        return -1;
    }
    if (cfsetospeed(t, B115200) < 0) {
        perror("cfsetospeed");
        return -1;
    }

    // enable receiver and ignore modem control signals
    t->c_cflag |= (CREAD | CLOCAL);

    return 0;
}