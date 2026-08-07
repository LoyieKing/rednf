#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "server.h"
#include "stun.h"
#include "udp.h"
#include "global_func.h"

int socketRealIp_;
int socketNat_;
int socketPortResctric_;

int main(int argc, char* argv[])
{
    pthread_t thread_id;
    int ret;

    puts("\n*****************************************************");
    puts("**             STUN Server v1.4.0.0                **");
    puts("**                                                 **");
    puts("**                                                 **");
    puts("**                            CopyRight 2010~2015. **");
    puts("**                       Young-san, Kim in Neople. **");
    puts("*****************************************************");

    make_dir("./pid/");
    make_dir("./log/");

    if (!regist_signals())
    {
        puts("Fail to Regist Signals, Server Process is Terminated.");
        return 0;
    }
    else if (!check_argv(argc, argv))
    {
        return 0;
    }
    else
    {
        printf("\nRealIP Port: %d, NAT Port: %d\n", 0x907, 0x908);

        socketRealIp_ = openPort(0x907, 0);
        if (socketRealIp_ == -1)
        {
            printf("[ERROR] Fail to Create Socket PORT(%d)\n", 0x907);
            return 0;
        }
        socketNat_ = openPort(0x908, 0);
        if (socketNat_ == -1)
        {
            printf("[ERROR] Fail to Create Socket PORT(%d)\n", 0x908);
            return 0;
        }
        socketPortResctric_ = openPort(0x909, 0);
        if (socketPortResctric_ == -1)
        {
            printf("[ERROR] Fail to Create Socket PORT(%d)\n", 0x909);
            return 0;
        }
        write_log("[[[[[ Start Udp Draft ]]]]]] v1.3.6.0", 0);

        ret = 0;
        ret = pthread_create(&thread_id, NULL, stunProcess1, NULL);
        ret = pthread_create(&thread_id, NULL, stunProcess2, NULL);
        ret = pthread_create(&thread_id, NULL, stunProcess3, NULL);

        while (true)
        {
            usleep(100000);
        }
    }
    return 0;
}
