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
    // [b28d-solo-CSimpleLog] _ZN10CSimpleLog9_WriteLogEPKc @0x0854fbfc
    // 判词：N 封顶（N18a + P75/P183 镜像：4.1 地址值跨分支保活 vs 4.4 恒重物化），语义等价。
    //   ORIG 4.1 形（memcpy 段 10 条，&m_dt 留 %eax 跨 je）：
    //     mov this,%eax; add $0x4,%eax; lea &dt,%edx; cmp %edx,%eax; je;
    //     movl $7,0x8(%esp); lea &dt,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call memcpy
    //   4.4.6/4.4.7 恒 12 条：比较段 `lea 0x4(%eax),%edx`（无 add 形、cmp 操作数序随分配反向）
    //     + memcpy 段重发 `mov this,%eax; lea 0x4(%eax),%edx`（+2 条 / +6 字节）。
    //   穷举证伪（/tmp/b28d_solo/probe{,2,3}.cpp，18 源形 + 4.4.6 后端）全部产同一 12 条形：
    //     A &m_dt!=&dt / B 反向 / C (void*)cast / D 双 cast / E 空 else / F while+break /
    //     G this->m_dt / H 正条件包裹 / I 三目 / J (char*)this+4 / K/L 混合 / M 三目语句 /
    //     N && 短路 / O 裸 register 指针 / P 裸 register 反向 / Q 具名指针 / R 裸 register 引用；
    //     Q 占槽（dt 槽平移 -0x14，帧劣化）、O/R 改用 %ebx 跨 call 并多 push/pop ebx（+2 条，反劣）。
    //   P248 三档类型旋钮不适用（纯地址比较，无 callee 返回类型/字段类型参与）；
    //   裸 register 第三条件（ORIG 目标寄存器为 callee-saved）不成立（ORIG 用 %eax）⇒ 不留 register。
    //   语义闭合证据：p167 调用点差集 7/7 CLEAR（C1/DiffDay/CloseFile/CreateLogFile/memcpy/fprintf/fflush）；
    //   自赋值防护与拷贝长度 7 与 ORIG 一致。
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
