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

class CSystemTime
{
public:
    CSystemTime();
    ~CSystemTime();

    int getCurSec();
    unsigned int getCurTickCount();
    char* getCurDate();
    void update();

    int m_sec;              // +0x00
    unsigned int m_msec;    // +0x04
    struct timeval m_tv;    // +0x08
    int m_lastSec;          // +0x10
};

#endif  // GAME_CSYSTEMTIME_H_
