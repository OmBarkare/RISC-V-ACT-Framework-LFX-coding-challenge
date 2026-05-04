#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <poll.h>

int tty_config(struct termios *t);

int main() {

    const char *file_path = "/tmp/tty0";
    int fd = open(file_path, O_RDWR | O_NOCTTY);

    if(fd < 0) {
        fprintf(stderr, "failed to open serial device\n");
        perror(file_path);
        return -1;
    }


    struct termios t;
    // copy current configuration to tty
    if (tcgetattr(fd, &t) < 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }
    // apply required config over copied config
    if (tty_config(&t) < 0) {
        fprintf(stderr, "tty_config failed\n");
        close(fd);
        return -1;
    }
    // set config 
    if (tcsetattr(fd, TCSANOW, &t) < 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    // writing to the serial device
    char *str = "Hello from main";
    if (write(fd, str, strlen(str)) < 0) {
        perror("could not write to device");
        close(fd);
        return -1;
    };

    // polling for response
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;

    while(1) {
        int ret = poll(&p, 1, 3000);
        printf("poll returned: %d with revents: %d\n", ret, p.revents);
        
        if(ret < 0) {
            perror("polling failed");
            break;
        } else if (ret == 0) {
            printf("device not ready for I/O\n");
        }

        if(p.revents & POLLHUP) {
            fprintf(stderr, "device disconnected\n");
            close(fd);
            return -1;
        }

        if(p.revents & POLLERR) {
            fprintf(stderr, "device error\n");
            close(fd);
            return -1;
        }

        if(p.revents & POLLIN) {
            char recv_buf[100];
            int n_r = read(fd, recv_buf, 99);

            if (n_r < 0) {
                perror("could not read from device");
                close(fd);
                return -1;
            } else if (n_r == 0) {
                fprintf(stdout, "read EOF\n");
                break;
            }

            recv_buf[n_r] = '\0';
            printf("message: %s\n", recv_buf);

            if (strcmp(recv_buf, "ILU") == 0) {
                if (write(fd, "ILU TOO", 7) < 0) {
                    perror("could not write to device");
                }
            } else if (strcmp(recv_buf, "ARE U THERE") == 0) {
                if (write(fd, "I AM HERE", 9) < 0) {
                    perror("could not write to device");
                }
            }
        }
    }

    close(fd);
    return 0;
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