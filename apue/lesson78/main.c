#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void int_handler(int s)
{
    write(1, "!", 1);
}

int main()
{
    int i = 0;
    int j = 0;
    sigset_t set, oldset, saveset;
    signal(SIGINT, int_handler);
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, &saveset);

    sigprocmask(SIG_BLOCK,&set,&oldset);

    for (j = 0; j < 1000; j++)
    {
        for (i = 0; i < 5; i++)
        {
            // 向终端打印*
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        sigsuspend(&oldset);  // 解除对某个信号的阻塞后立刻去等待这个信号的到来
        /*
        sigset_t tmpset;
        sigprocmask(SIG_SETMASK, &oldset, NULL);
        pause();
        sigprocmask(SIG_SETMASK,&tmpset,NULL);
        */
    }

    sigprocmask(SIG_SETMASK, &saveset, NULL);
    exit(0);
}

