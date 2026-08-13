#ifndef NSL_TRACELOG_H_
#define NSL_TRACELOG_H_

#include <stdarg.h>
#include <stdio.h>

#include "ThreadLock.h"
#include "GlobalInstance.h"

namespace nsl {

class LogSendThread;

extern bool bChangedDataForLog;

class TraceLog
{
public:
    TraceLog();
    virtual ~TraceLog();
    int init(char* ServiceName, char* ServiceIdentity);
    void sysLog(int flag, const char* msg, ...);
    void errorLog(const char* msg, ...);
    int get_mask(int bit);
    void set_mask(int bit, const char* yesno);
    int set_fd(int line);
    void get_time(char* todaystr, char* timestr);
    void sysNSL_LOG_date_ch();
    char* getBaseName(char* pName);
    int MakeDir(char* DirName);
    bool checkLogServer();
    int VsnPrintf(char* tmpbuf, int nSize, const char* msg, va_list ap);
    int SnPrintf(char* buf, int nSize, char* des, char* Data, char* Time, char* tmpbuf);
    void setMask();
    void setLogSendThread(LogSendThread* pLogSendThread);
    char* getServiceIdentity();

private:
    const char* mpServiceIdentity;
    char cur_date[20];
    char cur_time[20];
    char logfname[256];
    char errfname[256];
    char statfname[256];
    int logfd;
    int errfd;
    int statfd;
    long logmask;
    bool write_to_logserver;
    bool bNeedToLeaveErrorLogForOverSize;
    ThreadLock lockLog;
    LogSendThread* logSendThread_;
};

inline TraceLog* G_TraceLog()
{
    return GlobalInstance<TraceLog>::inst_ptr();
}

} // namespace nsl

#endif // NSL_TRACELOG_H_
