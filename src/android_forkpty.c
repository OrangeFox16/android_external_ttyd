#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>

int ttyd_android_forkpty(int *amaster, char *name,
                         const struct termios *termp,
                         const struct winsize *winp) {
    int master = -1;
    int slave = -1;
    char *slave_name = NULL;
    pid_t pid;

    if (amaster == NULL) {
        errno = EINVAL;
        return -1;
    }

    master = posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0) {
        return -1;
    }

    if (grantpt(master) < 0 || unlockpt(master) < 0) {
        close(master);
        return -1;
    }

    slave_name = ptsname(master);
    if (slave_name == NULL) {
        close(master);
        return -1;
    }

    if (name != NULL) {
        strcpy(name, slave_name);
    }

    slave = open(slave_name, O_RDWR | O_NOCTTY);
    if (slave < 0) {
        close(master);
        return -1;
    }

    if (termp != NULL) {
        tcsetattr(slave, TCSANOW, termp);
    }

    if (winp != NULL) {
        ioctl(slave, TIOCSWINSZ, winp);
    }

    pid = fork();
    if (pid < 0) {
        close(slave);
        close(master);
        return -1;
    }

    if (pid == 0) {
        close(master);

        setsid();
        ioctl(slave, TIOCSCTTY, 0);

        dup2(slave, STDIN_FILENO);
        dup2(slave, STDOUT_FILENO);
        dup2(slave, STDERR_FILENO);

        if (slave > STDERR_FILENO) {
            close(slave);
        }

        return 0;
    }

    close(slave);
    *amaster = master;
    return pid;
}
