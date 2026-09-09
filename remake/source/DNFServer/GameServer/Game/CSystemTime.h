#ifndef GAME_CSYSTEMTIME_H_
#define GAME_CSYSTEMTIME_H_

// ============================================================================
// df_game_r 还原 —— CSystemTime（0x14 布局）
// 参照 ORIG：getCurSec 0x080cbc9e / getCurTickCount 0x081458ac /
//            getCurDate 0x0823445e / update 0x082a68c8 /
//            ctor 0x082a6936 / dtor 0x082a6986
// 布局：+0 m_sec | +4 m_msec | +8 timeval | +0x10 m_lastSec
// ============================================================================

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

class CSystemTime
{
public:
    CSystemTime();
    ~CSystemTime();

    int getCurSec();
    unsigned int getCurTickCount();
    // ORIG W 0x0823445e（头内联发射）：static __thread buf 随 inline 函数以 W 形
    // 发射 `_ZZN11CSystemTime10getCurDateEvE3buf`（0xff 字节 TLS）——.cpp 定义会使
    // buf 落 .tbss 局部符号致仅ORIG 缺失（Main 按 nm 实证内联化）。
    char* getCurDate()
    {
        // ORIG nm -S 实证：_ZZN11CSystemTime10getCurDateEvE3buf = `00000200 000000ff W`
        // （TLS 基址 0x200、尺寸 0xff，非 0x100）。函数体走 sprintf（ORIG 0x082344a0
        // 同为无界写 sprintf），格式 "%04d-%02d-%02d" 写入固定 10 字符 + '\0' = 11 字节
        // ≪ 0xff ⇒ 收缩尺寸不引入新风险。
        static __thread char buf[0xff];
        struct tm t;
        struct tm* p = localtime_r((const time_t*)&m_sec, &t);
        sprintf(buf, "%04d-%02d-%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
        return buf;
    }
    void update();

    int m_sec;              // +0x00
    unsigned int m_msec;    // +0x04
    struct timeval m_tv;    // +0x08
    int m_lastSec;          // +0x10
};

#endif  // GAME_CSYSTEMTIME_H_
