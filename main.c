#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <poll.h>

int tty_config(struct termios *t);

int main() {

    int fd = open("/tmp/tty0", O_RDWR);

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
    // apply required config over copied config
    if (tty_config(&t) < 0) {
        fprintf(stderr, "could not conplete tty_config");
    }
    // set config 
    if (tcsetattr(fd, TCSANOW, &t) < 0) {
        perror("tcsetattr");
    }

    // writing to the serial device
    char *str = "Hello from C";
    write(fd, str, strlen(str));

    // polling for response
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;

    while(1) {
        int ret = poll(&p, 1, 3000);
        printf("poll returned: %d with revents: %d\n", ret, p.revents);
        
        if(ret < 0) {
            perror("polling failed\n");
            break;
        } else if (ret == 0) {
            printf("no device ready for I/O\n");
        }

        if(p.revents & POLLIN) {
            char write_buf[100];
            int n = read(fd, write_buf, 100);
            printf("message:\n");
            write(1, write_buf, n);
        }
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