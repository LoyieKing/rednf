// df_game_r ORIG 对齐：CUnixTimer 继承 CDnFTimer（vtable@0x8ccf320），
// 虚覆盖 SetLastTime/GetTimeInterval，GetNowTime 非虚返回 double，
// 成员 double m_lastTime @ +0x4（ctor 仅设 vptr，无成员初始化，见 ORIG 0x085ed7f2）。
#ifndef DNF_GAME_CUNIXTIMER_H_
#define DNF_GAME_CUNIXTIMER_H_

#include "MySQL.h"

#pragma pack(push, 1)
class CUnixTimer : public CDnFTimer
{
public:
    CUnixTimer() throw();
    double GetNowTime();
    virtual void SetLastTime();
    virtual double GetTimeInterval();
    double m_lastTime;  // +4（ORIG：vptr@0 + double@4，总大小 0xc）
};
#pragma pack(pop)


#endif // DNF_GAME_CUNIXTIMER_H_
