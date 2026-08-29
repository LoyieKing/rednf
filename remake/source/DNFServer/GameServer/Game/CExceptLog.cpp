// ============================================================================
// CExceptLog —— 异常日志类 ORIG 实现（Wave5 续批）。
// 证据：docs/class_func_reports/CExceptLog/ 全文（4 函数 + 全局 ios init）。
// C1/C2 同址（普通类无虚基），一个 ctor 实现；_GLOBAL__I_ 为 iostream 头
// 自动生成的 ios_base::Init 静态初始化。
// 注：fPrintTextFile 以 this 对象内存作 vsprintf 缓冲（ORIG 原行为）。
// ============================================================================
#include "CExceptLog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ORIG ctor @0x0846d736：memset(this,0,2000) → Set_Time_String(buf) →
// fPrintTextFile("Except.txt","%s\n%s\n",buf,str.c_str())。
CExceptLog::CExceptLog(std::string& str)
{
    memset((char*)this, 0, 2000);

    char buf[128];
    Set_Time_String(buf);
    fPrintTextFile("Except.txt", "%s\n%s\n", buf, str.c_str());
}

// ORIG @0x0846d640：fopen(name,"a") 成功后 fprintf+fclose。
void CExceptLog::PrintTextFile(char* name, char* content)
{
    FILE* stream = fopen(name, "a");
    if (stream != NULL)
    {
        fprintf(stream, content);
        fclose(stream);
    }
}

// ORIG @0x0846d682：vsprintf 写入 (char*)this，再 PrintTextFile(name,(char*)this)。
void CExceptLog::fPrintTextFile(char* name, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(reinterpret_cast<char*>(this), fmt, ap);
    va_end(ap);

    PrintTextFile(name, reinterpret_cast<char*>(this));
}

// ORIG @0x0846d6c2：time(&t) → localtime_r(&t,&tm) → sprintf 时间串。
void CExceptLog::Set_Time_String(char* buf)
{
    struct tm tm;
    time_t t;

    time(&t);
    localtime_r(&t, &tm);
    sprintf(buf, "%4d.%02d.%02d %02d:%02d:%02d",
            tm.tm_year + 0x76c, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}
