#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
// df_dbmw_r - DNFDBServerMain (ORIG DNFDBServerMain.cpp)

#include <signal.h>
#include <cstdio>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int main(int argc, char** argv)
{
    CApplication app;
    try
    {
        app.Create(argc, argv);
        if (strcmp(argv[2], "stop") != 0)
            app.Process();
        app.Clear();
    }
    catch (...)
    {
        app.Clear();
    }
    return 1;
}
