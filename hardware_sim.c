#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

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
            char read_buf[100];
            int n = read(fd, read_buf, 100);
            printf("mssg received, sending response\n");
            write(fd, "ILU", 3);
        }
    }
}