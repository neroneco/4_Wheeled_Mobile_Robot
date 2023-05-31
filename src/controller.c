#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <linux/input.h>
#include <linux/joystick.h>
#include <pthread.h>

#define YES 1
#define NO 0

int fd;
struct js_event js, power, X, Y, normal;
int new_data;
pthread_mutex_t lock;

void *read_from_controller(void *arg)
{
    int status = 0;
    while (1)
    {
        memset(&js, 0x0, sizeof(struct js_event));

        while ((status = read(fd, &js, sizeof(struct js_event))) != -1)
        {
            pthread_mutex_lock(&lock);
            if (js.number == 5 && js.type == 2)
            {
                memcpy(&power, &js, sizeof(struct js_event));
            }
            else if (js.number == 4 && js.type == 2)
            {
                memcpy(&X, &js, sizeof(struct js_event));
            }
            else if (js.number == 3 && js.type == 2)
            {
                memcpy(&Y, &js, sizeof(struct js_event));
            }
            else
            {
                memcpy(&normal, &js, sizeof(struct js_event));
            }
            new_data = YES;
            pthread_mutex_unlock(&lock);
        }
    }
}

int main(int argc, char **argv)
{
    assert((fd = open("/dev/input/js0", O_RDONLY)) >= 0);

    fcntl(fd, F_SETFL, O_NONBLOCK);
    fcntl(0, F_SETFL, O_NONBLOCK);

    char buf[10];

    new_data = NO;
    memset(&power, 0x0, sizeof(struct js_event));
    memset(&X, 0x0, sizeof(struct js_event));
    memset(&Y, 0x0, sizeof(struct js_event));
    memset(&normal, 0x0, sizeof(struct js_event));

    pthread_t tid;
    if (pthread_create(&tid, NULL, read_from_controller, NULL) != 0)
    {
        printf("Error while creating controller thread\n");
    }

    while (read(0, buf, 1) == -1)
    {
        if (new_data == YES)
        {
            new_data = NO;
            write(1, &power, sizeof(struct js_event));
            usleep(5000);
            write(1, &X, sizeof(struct js_event));
            usleep(5000);
            write(1, &Y, sizeof(struct js_event));
            usleep(5000);
            write(1, &normal, sizeof(struct js_event));
            usleep(5000);
        }
        usleep(100000);
    }
    pthread_cancel(tid);
    usleep(100000);
    close(fd);

    return 0;
}
