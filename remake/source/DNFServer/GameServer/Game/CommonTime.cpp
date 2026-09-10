// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CommonTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "CommonTime/SetCreateTime.inc"
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

#include "CommonTime/SetCreateTime_1.inc"
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

#include "CommonTime/SetCurTime.inc"
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

#include "CommonTime/clear.inc"
{
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_min = 0;
    m_wday = 0;
}

#include "CommonTime/GetCreateTime.inc"
{
    // ORIG 秒位固定输出 0
    sprintf(buf, "%4d-%2d-%2d %2d:%2d:%2d", (int)m_year + 2000, (int)m_month,
            (int)m_day, (int)m_hour, (int)m_min, 0);
}

#include "CommonTime/MakeTime.inc"
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
