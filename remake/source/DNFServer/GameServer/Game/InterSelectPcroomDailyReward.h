#ifndef GAME_INTERSELECT_PCROOM_DAILY_REWARD_H
#define GAME_INTERSELECT_PCROOM_DAILY_REWARD_H

// df_game_r 还原 —— InterSelectPcroomDailyReward 类。
// ORIG W ctor 0x08162170（继承 TaiwanInterfaceInterDispatcher，基类 code 0xd，
// 证据：global_constructors_keyed_to_TaiwanInternalDispatcher 报告 08162170 区
// movl $0xd,0x4(%esp); call TaiwanInterfaceInterDispatcher::C1）+ vtable/typeinfo。
// makeRequest（ORIG T 0x816179c）：第二参 typedef memberIdentificationNumber_t
// （__PRETTY_FUNCTION__ 实证），等价 unsigned int。dispatch_sig（ORIG T 0x81616ce）。

#include "TaiwanInterfaceInterDispatcher.h"

class CUser;

// ORIG Stream::GetInBuffer<Msg>（0x8162d44）enable_in/in_ptr 均为 $0x8 → sizeof(Msg)=8。
// 布局：+0x00 uint m_id（makeRequest 写 / dispatch_sig NumberToString 读），
// +0x04 char m_remainCount（makeRequest 写 / dispatch_sig 读 user+0x8ec32）。
typedef unsigned int memberIdentificationNumber_t;

class InterSelectPcroomDailyReward : public TaiwanInterfaceInterDispatcher
{
public:
    struct Msg
    {
        unsigned int m_id;        // +0x00
        char m_remainCount;       // +0x04
        char m_pad[3];            // +0x05
    };

    InterSelectPcroomDailyReward() throw();   // ORIG W 0x08162170
    virtual ~InterSelectPcroomDailyReward();
    static void SendPacket(CUser* user);      // ORIG T 0x081618a6
    virtual int dispatch_sig(CUser* user, char* data, int len);  // ORIG T 0x081616ce
    static void makeRequest(int uid, memberIdentificationNumber_t m_id, char remainCount);  // ORIG T 0x0816179c
};

#endif  // GAME_INTERSELECT_PCROOM_DAILY_REWARD_H
