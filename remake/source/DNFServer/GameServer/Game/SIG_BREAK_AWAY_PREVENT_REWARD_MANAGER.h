// ORIG W 0x0831d264 _ZN37SIG_BREAK_AWAY_PREVENT_REWARD_MANAGERC1Ev。
// 布局（ctor 反汇编实证）：3×movb(+0/+1/+2) + 5×movl(+4/+8/+c/+10/+14)，sizeof 0x18；
// SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER::m_rewards[30] 步进 0x18 交叉验证。
#ifndef DNF_GAME_SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER_H_
#define DNF_GAME_SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER_H_

class SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER
{
public:
    SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER()
        : m_field0(0), m_field1(0), m_field2(0),
          m_field4(0), m_field8(0), m_fieldc(0), m_field10(0), m_field14(0)
    {
    }

    unsigned char m_field0;   // +0x00
    unsigned char m_field1;   // +0x01
    unsigned char m_field2;   // +0x02
    int m_field4;             // +0x04
    int m_field8;             // +0x08
    int m_fieldc;             // +0x0c
    int m_field10;            // +0x10
    int m_field14;            // +0x14
};  // sizeof 0x18

#endif  // DNF_GAME_SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER_H_
