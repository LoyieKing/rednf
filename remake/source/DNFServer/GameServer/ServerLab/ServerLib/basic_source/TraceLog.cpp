// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TraceLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "Script.h"
#include "TraceLog.h"
#include "LogSendThread.h"

extern "C" int __xstat(int ver, const char* path, struct stat* buf);

extern "C" __attribute__((weak)) int stat(const char* __path, struct stat* __statbuf)
{
    return __xstat(3, __path, __statbuf);
}

namespace nsl {

bool bChangedDataForLog = false;

#include "TraceLog/TraceLog.inc"
    : logmask(0)
{
    write_to_logserver = false;
    bNeedToLeaveErrorLogForOverSize = true;
}

#include "TraceLog/dtor_TraceLog.inc"
{
    close(logfd);
    close(errfd);
    close(statfd);
}

#include "TraceLog/init.inc"
{
    mpServiceIdentity = ServiceIdentity;
    memset(logfname, 0, 0x100);
    memset(errfname, 0, 0x100);
    memset(statfname, 0, 0x100);
    sprintf(logfname, "%s/Info_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    sprintf(errfname, "%s/Error_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    sprintf(statfname, "%s/Stat_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    if (access(logfname, 0) == 0)
    {
        char backup_name[1024];
        struct stat st;
        sprintf(backup_name, "%s/old_log", G_Script()->findCharValue(0, 1));
        if (stat(backup_name, &st) < 0)
        {
            if (MakeDir(backup_name) < 0)
            {
                return -1;
            }
        }
        time_t log_now = time(NULL);
        tm* tm_now = localtime(&log_now);
        sprintf(backup_name, "%s/old_log/Info_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(logfname, backup_name);
        sprintf(backup_name, "%s/old_log/Error_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(errfname, backup_name);
        sprintf(backup_name, "%s/old_log/Stat_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(statfname, backup_name);
    }
    setMask();
    if (set_fd(99) < 0)
    {
        return -1;
    }
    return 0;
}

#include "TraceLog/sysNSL_LOG_date_ch.inc"
{
    // ORIG DWARF 声明序（decl 220/221/222/223/225）：数组→orifname→st→size/rst/i→slock
    char logdname[4096];
    char renfname[4096];
    char* orifname;
    struct stat st;
    int size;
    int rst;
    int i;
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockLog);
    time_t now;
    tm* tm_now;
    int ret;
    // ORIG: 赋值在条件内（mov rst; shr $0x1f; test; je）
    if ((rst = stat(logfname, &st)) < 0)
    {
        errorLog("sysNSL_LOG_date_ch stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, logfname);
        exit(1);
    }
    // 原始：access 失败分支在成功分支之前（callset 顺序）
    if (access(logfname, 0) != 0)
    {
        if (bNeedToLeaveErrorLogForOverSize != false)
        {
            errorLog("sysNSL_LOG_date_ch access fail : errmsg[%s(%d)] : logfilename[%s]", strerror(errno), errno, logfname);
            bNeedToLeaveErrorLogForOverSize = false;
        }
    }
    else
    {
        if (0x1388000 < st.st_size)
        {
            bChangedDataForLog = true;
        }
        // 原始：if (!bChangedDataForLog) 跳过；否则旋转日志
        if (bChangedDataForLog == false)
        {
        }
        else
        {
            bChangedDataForLog = false;
            sprintf(logdname, "%s/old_log", G_Script()->findCharValue(0, 1));
            if ((rst = stat(logdname, &st)) < 0)
            {
                if (errno != ENOENT)
                {
                    errorLog("sysNSL_LOG_date_ch stat() errmsg[%s(%d)]", strerror(errno), errno);
                    return;
                }
                if (MakeDir(logdname) < 0)
                {
                    errorLog("sysNSL_LOG_date_ch mkdir() errmsg[%s(%d)]", strerror(errno), errno);
                    return;
                }
            }
            // 原始：close 结果存局部再与 0 比较
            ret = close(logfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), logfd);
            }
            ret = close(errfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), errfd);
            }
            ret = close(statfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), statfd);
            }
            for (i = 0; i < 3; i = i + 1)
            {
                if (i == 0)
                {
                    now = time(NULL);
                    tm_now = localtime(&now);
                }
                if (i == 0)
                {
                    orifname = logfname;
                    sprintf(renfname, "%s/Info_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                else if (i == 1)
                {
                    orifname = errfname;
                    sprintf(renfname, "%s/Error_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                else
                {
                    orifname = statfname;
                    sprintf(renfname, "%s/Stat_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                if (rename(orifname, renfname) < 0)
                {
                    set_fd(0x148);
                    errorLog("sysNSL_LOG_date_ch rename() errmsg[%s(%d)]", strerror(errno), errno);
                }
            }
            set_fd(0x14e);
        }
    }
}


#include "TraceLog/sysLog.inc"
{
    bool other_file_log = false;
    if (flag == 9)
    {
        other_file_log = true;
    }
    else if (get_mask(flag) == 0)
    {
        return;
    }
    va_list ap;
    va_start(ap, msg);
    char tmpbuf[2048];
    // 原始：VsnPrintf 写到 tmpbuf 基址（非 tmpbuf+1）
    int len = VsnPrintf(tmpbuf, 0x800, msg, ap);
    va_end(ap);
    get_time(cur_date, cur_time);
    // 原始：while (tmpbuf[len-1] == '\n') { tmpbuf[len-1]=0; len--; }
    while (tmpbuf[len - 1] == '\n')
    {
        tmpbuf[len - 1] = '\0';
        len = len - 1;
    }
    char buf[2088];
    len = SnPrintf(buf, 0x2028, (char*)"%s %s : %s\n", cur_date, cur_time, tmpbuf);
    if (len < 0)
    {
        return;
    }
    // 原始：if (write_to_logserver) SendLogMsg 在前，else 本地写
    if (write_to_logserver != false)
    {
        logSendThread_->SendLogMsg(buf, len);
    }
    else
    {
        sysNSL_LOG_date_ch();
        if (logfd < 1)
        {
            puts("syslog fd error");
        }
        else if (other_file_log != false)
        {
            write(statfd, buf, len);
        }
        else
        {
            if (flag == 7)
            {
                write(errfd, buf, len);
            }
            write(logfd, buf, len);
        }
    }
}

#include "TraceLog/errorLog.inc"
{
    va_list ap;
    va_start(ap, msg);
    char tmpbuf[2048];
    // 原始：同 sysLog，VsnPrintf(tmpbuf, ...) + while (tmpbuf[len-1]=='\n')
    int len = VsnPrintf(tmpbuf, 0x800, msg, ap);
    va_end(ap);
    get_time(cur_date, cur_time);
    while (tmpbuf[len - 1] == '\n')
    {
        tmpbuf[len - 1] = '\0';
        len = len - 1;
    }
    char buf[2088];
    len = SnPrintf(buf, 0x2028, (char*)"%s %s : %s\n", cur_date, cur_time, tmpbuf);
    write(errfd, buf, len);
}

#include "TraceLog/get_mask.inc"
{
    // ORIG：logmask 先载 edx，1<<bit 走 ebx/esi（callee-saved）——
    // 操作数顺序 logmask & (1<<bit) 才能复现（反过来是 edx/ecx 直算，16 条）。
    if ((logmask & (1 << bit)) > 0)
    {
        return 1;
    }
    return 0;
}

#include "TraceLog/set_mask.inc"
{
    // ORIG：外层早退（if NULL return）→ else-if 链尾有 jmp EPI + nop。
    if (yesno == NULL)
    {
        return;
    }
    if (strcmp(yesno, "yes") == 0)
    {
        logmask = logmask | (1 << bit);
    }
    else if (strcmp(yesno, "no") == 0)
    {
        logmask = logmask & ~(1 << bit);
    }
}

#include "TraceLog/set_fd.inc"
{
    FILE* fptr;
    // 原始：fopen 赋值在条件内 → cmpl/sete/test/je
    if ((fptr = fopen(logfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", logfname, strerror(errno));
        return -1;
    }
    logfd = fileno(fptr);
    if ((fptr = fopen(errfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", errfname, strerror(errno));
        return -1;
    }
    errfd = fileno(fptr);
    if ((fptr = fopen(statfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", statfname, strerror(errno));
        return -1;
    }
    statfd = fileno(fptr);
    return 1;
}

#include "TraceLog/get_time.inc"
{
    // 原始：time(&tval) 非 tval=time(NULL)
    time_t tval;
    time(&tval);
    tm* t = localtime(&tval);
    if (todaystr != NULL)
    {
        sprintf(todaystr, "%04d%02d%02d", t->tm_year + 0x76c, t->tm_mon + 1, t->tm_mday);
    }
    if (timestr != NULL)
    {
        sprintf(timestr, "%02d%02d%02d", t->tm_hour, t->tm_min, t->tm_sec);
    }
}

#include "TraceLog/getBaseName.inc"
{
    return basename(pName);
}

#include "TraceLog/MakeDir.inc"
{
    return mkdir(DirName, 0x1c0);
}

#include "TraceLog/checkLogServer.inc"
{
    write_to_logserver = false;
    return write_to_logserver;
}

#include "TraceLog/VsnPrintf.inc"
{
    return vsnprintf(tmpbuf, nSize, msg, ap);
}

#include "TraceLog/SnPrintf.inc"
{
    return snprintf(buf, 0x2028, des, cur_date, cur_time, tmpbuf);
}

#include "TraceLog/setMask.inc"
{
    set_mask(0, G_Script()->findCharValue(0, 0xe));
    set_mask(1, G_Script()->findCharValue(0, 0xf));
    set_mask(2, G_Script()->findCharValue(0, 0x10));
    set_mask(3, G_Script()->findCharValue(0, 0x11));
    set_mask(4, G_Script()->findCharValue(0, 0x12));
    set_mask(5, G_Script()->findCharValue(0, 0x13));
    set_mask(6, G_Script()->findCharValue(0, 0x14));
    set_mask(7, G_Script()->findCharValue(0, 0x15));
    set_mask(8, G_Script()->findCharValue(0, 0x16));
}

#include "TraceLog/setLogSendThread.inc"
{
    logSendThread_ = pLogSendThread;
}

#include "TraceLog/getServiceIdentity.inc"
{
    return (char*)mpServiceIdentity;
}

} // namespace nsl
