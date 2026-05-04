#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <poll.h>

int tty_config(struct termios *t);
int baud_from_speed(speed_t speed);

int main() {

    // open with O_NOCTTY so that this terminal does not
    // become the controlling terminal
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

    // check if config was set
    struct termios check;
    if (tcgetattr(fd, &check) < 0) {
        perror("tcgetaddr check");
        close(fd);
        return -1;
    }

    // print configuration by reading it from fd, to check if
    // it has been set correctly
    printf("UART CONFIG:\n");
    printf("baud rate input: %d,\nbaud rate output: %d,\n",
        baud_from_speed(cfgetispeed(&check)), baud_from_speed(cfgetospeed(&check)));
    printf("VMIN: %d,\n", check.c_cc[VMIN]);
    printf("VTIME: %d\n", check.c_cc[VTIME]);

    // writing to the serial device
    char *str = "HELLO";
    if (write(fd, str, strlen(str)) < 0) {
        perror("could not write to device");
        close(fd);
        return -1;
    };

    // setting poll up for readiness of fd
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;

    // polling loop
    while(1) {
        int ret = poll(&p, 1, 3000);
        #ifdef DEBUG
        printf("poll returned: %d with revents: %d\n", ret, p.revents);
        #endif
        
        if(ret < 0) {
            perror("polling failed");
            break;
        }
        else if (ret == 0) {
            printf("device not ready for I/O\n");
        }

        // check POLLHUP and POLLERR before POLLIN to confirm
        // fd was not marked ready because of deviec hang up or
        // error
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

            // if read returns with -1
            if (n_r < 0) {
                perror("could not read from device");
                close(fd);
                return -1;
            } // if EOF (0 bytes) is read
            else if (n_r == 0) {
                fprintf(stdout, "read EOF\n");
                break;
            }

            // terminate the buffer with null character for strcmp and printf
            recv_buf[n_r] = '\0';
            printf("message received: %s\n", recv_buf);

            // communication protocol
            if (strcmp(recv_buf, "PING") == 0) {
                printf("sending response: PONG");
                if (write(fd, "PONG", 4) < 0) {
                    perror("could not write to device");
                }
                printf("\n");
            }
            else if (strcmp(recv_buf, "OK") == 0) {
                printf("sending response: READY");
                if (write(fd, "READY", 5) < 0) {
                    perror("could not write to device");
                }
                printf("\n");
            }
        }
    }

    close(fd);
    return 0;
}


int tty_config(struct termios *t) {

    // disable canonical mode, signals, echo, extensions
    // disable parity and set data size to 8 bit
    // set VMIN = 1, VTIME = 0
    cfmakeraw(t);

    // override VMIN with 0 for non-blocking behaviour,
    // read() returns even if 0 bytes are read
    t->c_cc[VMIN] = 0;

    // clear stop bit flag to have only 1 terminating stop bit
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

int baud_from_speed(speed_t speed) {
    int baud;
    switch(speed) {
    case B1200:   baud = 1200;   break;
    case B9600:   baud = 9600;   break;
    case B57600:  baud = 57600;  break;
    case B115200: baud = 115200; break;
    case B230400: baud = 230400; break;
    default:      baud = -1;     break;
    }

    return baud;
}