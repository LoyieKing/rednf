// CommonTime — 时间小结构（ORIG 0x822d0e6..0x822d201 / 0x86947d8..0x869498a；
// 权威参照 docs/class_func_reports/_lt_global_gt_.md 对应段 + 本报告）
// 布局（0x14）：
//   +0x00 m_year  两位年（tm_year - 100 = 历年 - 2000）
//   +0x01 m_month 1..12
//   +0x02 m_day
//   +0x03 m_hour
//   +0x04 m_min
//   +0x05 m_wday  ORIG 此处存 tm_wday（星期），非秒
//   +0x08 m_curTime（秒）
//   +0x0c m_createTime（秒）
// 全部方法在 ORIG 为 W（头内联/弱），本 TU 落地为强定义（体逐字节一致）。
// 注意：无用户 ctor/dtor（ORIG 无符号，勿补）。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

class CommonTime
{
public:
    void SetCreateTime(long t);
    void SetCreateTime(char* src);
    void SetCurTime();
    void clear();
    void GetCreateTime(char* buf) const;
    void MakeTime(tm& out) const;
    operator long() const;

    char m_year;   // +0x00
    char m_month;  // +0x01
    char m_day;    // +0x02
    char m_hour;   // +0x03
    char m_min;    // +0x04
    char m_wday;   // +0x05
    char m_pad[2]; // +0x06
    long m_curTime;      // +0x08
    long m_createTime;   // +0x0c
};

void CommonTime::SetCreateTime(long t)
{
    tm tmv;
    tm* ptm = localtime_r(&t, &tmv);
    m_year  = (char)(ptm->tm_year - 100);
    m_month = (char)(ptm->tm_mon + 1);
    m_day   = (char)ptm->tm_mday;
    m_hour  = (char)ptm->tm_hour;
    m_min   = (char)ptm->tm_min;
    m_wday  = (char)ptm->tm_wday;
    m_curTime = t;
    m_createTime = t;
}

void CommonTime::SetCreateTime(char* src)
{
    // 格式 "xxYYYY-MM-DD HH:MM"：跳过 2 字符前缀，按 2 字符 token + 1
    // 分隔符解析（ORIG 语义，只解析 5 段：年月日时分）
    char buf[3] = {0};
    src += 2;
    strncpy(buf, src, 2);
    m_year = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_month = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_day = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_hour = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_min = (char)atoi(buf);
}

void CommonTime::SetCurTime()
{
    long t;
    // ORIG 以 &t 作为 time() 的 seed 实参且不存返回值（t 保持未初始化，
    // 后续按字节序读取栈槽——逐字节对齐 ORIG，勿“修复”）
    time(&t);
    tm tmv;
    tm* ptm = localtime_r(&t, &tmv);
    m_year  = (char)(ptm->tm_year - 100);
    m_month = (char)(ptm->tm_mon + 1);
    m_day   = (char)ptm->tm_mday;
    m_hour  = (char)ptm->tm_hour;
    m_min   = (char)ptm->tm_min;
    m_wday  = (char)ptm->tm_wday;
    m_curTime = t;
    m_createTime = t;
}

void CommonTime::clear()
{
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_min = 0;
    m_wday = 0;
}

void CommonTime::GetCreateTime(char* buf) const
{
    // ORIG 秒位固定输出 0
    sprintf(buf, "%4d-%2d-%2d %2d:%2d:%2d", (int)m_year + 2000, (int)m_month,
            (int)m_day, (int)m_hour, (int)m_min, 0);
}

void CommonTime::MakeTime(tm& out) const
{
    out.tm_sec = 0;
    out.tm_min = m_min;
    out.tm_hour = m_hour;
    out.tm_mday = m_day;
    out.tm_mon = m_month - 1;
    out.tm_year = m_year + 100;
    out.tm_isdst = 0;
}

CommonTime::operator long() const
{
    tm tmv;
    MakeTime(tmv);
    return mktime(&tmv);
}
