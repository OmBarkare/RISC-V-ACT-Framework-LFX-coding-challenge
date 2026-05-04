#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("/tmp/ttyHw", O_RDWR);

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
            write(fd, "ARE U THERE", 11);
        }

        if (p.revents & POLLIN) {
            char recv_buf[100];
            int n = read(fd, recv_buf, 99);
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
                write(fd, "ILU", 3);
                printf("mssg received: %s,\nsending response: %s\n", recv_buf, "ILU");
            }
        }
    }
}