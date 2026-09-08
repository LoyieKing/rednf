// CSimpleLog 实现 — ORIG 符号对齐
// 报告: docs/class_func_reports/CSimpleLog/ 及 docs/class_func_reports/CSimpleLog__DateTime/
// 全部函数已完整阅读 ## 汇编 + ## 反编译 C。
#include "CSimpleLog.h"

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>

// _ZN10CSimpleLog8DateTimeC1Ev / C2Ev @ 0x0854fa02
CSimpleLog::DateTime::DateTime()
{
    time_t t = time((time_t*)0);
    struct tm tm_buf;
    struct tm* p = localtime_r(&t, &tm_buf); // P43: 经返回值访问字段
    m_year  = p->tm_year + 0x76c;
    m_month = p->tm_mon + 1;
    m_day   = p->tm_mday;
    m_hour  = p->tm_hour;
    m_min   = p->tm_min;
    m_sec   = p->tm_sec;
}

// _ZN10CSimpleLog8DateTime7DiffDayERS0_ @ 0x0854ff62 (W, 头内联; 此处 strong 定义)
bool CSimpleLog::DateTime::DiffDay(DateTime& rhs)
{
    return m_day != rhs.m_day;
}

// _ZN10CSimpleLogC1Ev / C2Ev @ 0x0854fa88
CSimpleLog::CSimpleLog() : m_fp(0)
{
}

// _ZN10CSimpleLogD1Ev / D2Ev @ 0x0854faa8 (非虚: ORIG 无 vtable)
CSimpleLog::~CSimpleLog()
{
    CloseFile();
}

// _ZN10CSimpleLog9CloseFileEv @ 0x0854fb06
bool CSimpleLog::CloseFile()
{
    if (m_fp != 0) {
        fclose(m_fp);
        m_fp = 0;
    }
    return 1;
}

// _ZN10CSimpleLog8OpenFileEPKc @ 0x0854fabc
bool CSimpleLog::OpenFile(char const* path)
{
    if (m_fp != 0) {
        return 0;
    }
    m_fp = fopen(path, "ab");
    // [b28c-lane17] ORIG 0x0854fabc 尾段 `test %eax; sete %al; test %al; je 成功块`
    // ⇒ P225 X3 while+break 物化形（裸 if 被 4.4 折成 jne；具名 bool 局部 spill 反劣）。
    while (m_fp == 0)
    {
        return 0;
        break;
    }
    return 1;
}

// _ZN10CSimpleLog13CreateLogFileERNS_8DateTimeE @ 0x0854fb32
bool CSimpleLog::CreateLogFile(DateTime& dt)
{
    char buf[1024] = {0};
    sprintf(buf, "%s%04d%02d%02d.%s", m_path, (int)dt.m_year, (int)dt.m_month, (int)dt.m_day);
    return OpenFile(buf);
}

// _ZN10CSimpleLog13CreateLogFileEPKc @ 0x0854fbba
void CSimpleLog::CreateLogFile(char const* path)
{
    DateTime dt;
    strncpy(m_path, path, 0xfe);
    CreateLogFile(dt);
}

// _ZN10CSimpleLog9_WriteLogEPKc @ 0x0854fbfc
bool CSimpleLog::_WriteLog(char const* msg)
{
    if (m_fp == 0) {
        return 0;
    }
    DateTime dt;
    if (dt.DiffDay(m_dt)) {
        CloseFile();
        if (!CreateLogFile(dt)) {
            return 0;
        }
    }
    // [b28c-lane17] P0：清除历史 register asm("eax") 绑定。ORIG 0x0854fc64 形为
    // `mov this; add $0x4; lea dt; cmp; je` + memcpy 段内二次 `lea dt`（P183 地址重物化），
    // 直接写表达式即复现。
    // 判词：清绑后残余 = ①4.1 把 `&m_dt` 留在 %eax 跨 je 存活（4.4 在 memcpy
    // 参数位重取 this+add，+2 条）；②`add $0x4,%eax` vs `lea 0x4(%eax),%edx`
    // 折叠差 ⇒ N18a + P75（跨版本地址物化时刻），语义（含自赋值防护）等价。
    if (&m_dt != &dt) {
        memcpy(&m_dt, &dt, 7);
    }
    fprintf(m_fp, "%s\r\n", msg);
    fflush(m_fp);
    return 1;
}

// _ZN10CSimpleLog8WriteLogEPKcz @ 0x0854fcb8
void CSimpleLog::WriteLog(char const* fmt, ...)
{
    char buf[4096];
    memset(buf, 0, 0x1000);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    _WriteLog(buf);
}
