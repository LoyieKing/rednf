#ifndef GAME_CEVENTSTAYTIME_H_
#define GAME_CEVENTSTAYTIME_H_

#include "CGuildServerProxy.h"

class CEventStayTime : public CEventBase
{
public:
    CEventStayTime();
    virtual ~CEventStayTime();
    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
    virtual void dailyresetData();
    void RegistNextTimer();
    bool IsFirstReset();
    // ORIG 静态 _ZN14CEventStayTime12kRewardItem_E（R 0x08b800a0，值 0x2920908d）：
    // 类静态成员（外部链接，无 L）。类内声明 + 类外定义（CEventStayTime.cpp）。
    static const int kRewardItem_;
    // ORIG 为非 const 成员（mangled Ev 无 K：getRewardStayTime 0x0816bebc、
    // getIntervalStayTime 0x0816bec0），勿加 const。
    unsigned int getRewardStayTime();
    unsigned int getIntervalStayTime();
    // ORIG 嵌套 functor（内联 ctor W 0x0863c002；operator() ORIG T 0x082add80，
    // 定义在 GameWorld.cpp）。ProcessAllUsers<RewardGoGoFighter> 显式实例化 0x082aebb2。
    class RewardGoGoFighter
    {
    public:
        // ORIG 内联 ctor W 0x0863c002（两 store：m_0/m_4；mangled C1Eil =
        // (int,long)，故首参为 int 而非 unsigned）。
        RewardGoGoFighter(int rewardStayTime, time_t baseSec)
            : m_0(rewardStayTime), m_4(static_cast<int>(baseSec))
        {
        }
        void operator()(CUser* user);

        int m_0;   // +0x00 rewardStayTime
        int m_4;   // +0x04 baseSec
    };
    // ORIG 布局：CEventBase 基区 sizeof 0x0c（vptr@0 + stADDINFO@4 + bool@8），
    // 首个 bool 成员复用基类尾 padding（C1/IsFirstReset 实证 +0x9），随后
    // m_rewardStayTime@0xc、m_intervalStayTime@0x10（EndEvent movl 0xc/0x10 实证）。
    // 声明序必须 bool 在前，否则 GCC 布局为 +0x14 与 ORIG 不符。
    bool m_firstReset;          // +0x09
    unsigned int m_rewardStayTime;    // +0x0c
    unsigned int m_intervalStayTime;  // +0x10
};

#endif
