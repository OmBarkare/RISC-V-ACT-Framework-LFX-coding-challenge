#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

int tty_config(struct termios *t);

int main() {
    int fd = open("/tmp/ttyHw", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("/tmp/ttyHw");
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


    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;

    while(1) {
        int ret = poll(&p, 1, 3000);

        if (ret < 0) {
            perror("poll error");
            break;
        } else if (ret == 0) {
            printf("no mssg from laptop\n");
            if (write(fd, "ARE U THERE", 11) < 0) {
                perror("write failed");
            }
        }

        if (p.revents & POLLIN) {
            char recv_buf[100];
            int n = read(fd, recv_buf, 99);
            if(n < 0) {
                perror("read failed");
                close(fd);
                return -1;
            }
            else if(n == 0) {
                printf("read EOF\n");
                break;
            }
            recv_buf[n] = '\0';
            
            if (strcmp(recv_buf, "ILU TOO") == 0) {
                if (write(fd, ":)", 2) < 0) {
                    perror("could not write");
                }
                printf("mssg received: %s,\nsending response: %s\n", recv_buf, ":)");
            } else if(strcmp(recv_buf, "I AM HERE") == 0) {
                if (write(fd, "OK", 2) < 0) {
                    perror("could not write");
                }
                printf("mssg received: %s,\nsending response: %s\n", recv_buf, "OK");
            } else {
                if (write(fd, "ILU", 3) < 0) {
                    perror("could not write");
                }
                printf("mssg received: %s,\nsending response: %s\n", recv_buf, "ILU");
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

    // override VMIN = 1 for non-blocking behaviour,
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