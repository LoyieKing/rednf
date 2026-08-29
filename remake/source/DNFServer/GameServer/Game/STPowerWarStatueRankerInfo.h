#ifndef DNF_GAME_STPOWERWARSTATUERANKERINFO_H_
#define DNF_GAME_STPOWERWARSTATUERANKERINFO_H_

#include <string.h>

// STPowerWarStatueRankerInfo：力战雕像排行条目（CPowerManager +0x28 起 3 条嵌入，
// entry 0x84）。ORIG 反汇编 0x08480f9e（C1/C2 同址弱符号 W）：
//   +0x00 byte=0；+0x04 dword=0；+0x08 memset(0x1e)；+0x26/+0x27/+0x28 byte=0；
//   +0x29 memset(0x28)；+0x58..+0x80 循环清零 11 dword（equipment[11]）；
//   +0x51..+0x53 padding、+0x54 guildId 保持未初始化（ORIG 未写）。
// 布局与 CPowerManager.h RankerEntry 交叉印证（GetRankingByCharacNo +0x2c /
// CheckCompleteDBLoadStatueInfo +0x28 flag / SendPowerWarUserStatueInfo 各偏移）。
struct STPowerWarStatueRankerInfo
{
public:
    unsigned char m_flag;       // +0x00（1=已加载）
    char m_pad01[3];            // +0x01..+0x03
    unsigned int m_characNo;    // +0x04
    char m_name[0x1e];          // +0x08（30B）
    char m_job;                 // +0x26
    char m_level;               // +0x27
    char m_grow;                // +0x28
    char m_guildName[0x28];     // +0x29（40B）
    int m_guildId;              // +0x54（0x51..0x53 padding；ORIG ctor 未初始化）
    int m_equipment[11];        // +0x58（0x80 起，11 槽）

    STPowerWarStatueRankerInfo()
        : m_flag(0), m_characNo(0), m_job(0), m_level(0), m_grow(0)
    {
        memset(m_name, 0, sizeof(m_name));
        memset(m_guildName, 0, sizeof(m_guildName));
        for (int i = 0; i <= 10; ++i)   // ORIG C1 0x08481005..0x0848102c 循环形态
            m_equipment[i] = 0;
    }
};

#endif  // DNF_GAME_STPOWERWARSTATUERANKERINFO_H_
