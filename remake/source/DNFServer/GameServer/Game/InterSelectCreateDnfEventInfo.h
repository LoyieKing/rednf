#ifndef GAME_INTERSELECTCREATEDNFEVENTINFO_H_
#define GAME_INTERSELECTCREATEDNFEVENTINFO_H_

// df_game_r 还原 —— InterSelectCreateDnfEventInfo（台湾内部事件分发器）。
// ORIG：继承 TaiwanInterfaceInterDispatcher（C1 0x816201a 调基类 ctor code=9
// 后设 vtable 0x8b7aca8；D1 0x816203e 设 vptr 后调基类 D1）。
// vtable @0x8b7aca0：3 虚函数槽 = [~D1 @+0x08, ~D0 @+0x0c, dispatch_sig @+0x10]
// （dispatch_sig 覆盖基类纯虚槽，见 ORIG _ZTV29InterSelectCreateDnfEventInfo）。
// 方法报告：
//   makeRequest 0x8160ef6  （docs/class_func_reports/InterSelectCreateDnfEventInfo/makeRequest.md）
//   dispatch_sig 0x8160d5e  （docs/class_func_reports/InterSelectCreateDnfEventInfo/dispatch_sig.md）
//   Reward::Reward(int,bool) 0x8160c4a / Reward::operator() 0x8160c6e
//   （docs/class_func_reports/InterSelectCreateDnfEventInfo__Reward/）
// Msg 布局：3×int（makeRequest 写 +0/+4/+8；dispatch_sig 按 +0/+4/+8 读）。

class CUser;

#include "TaiwanInterfaceInterDispatcher.h"   // 基类权威声明（virtual dtor 由 OrigTaiwanDisp 落）

class InterSelectCreateDnfEventInfo : public TaiwanInterfaceInterDispatcher
{
public:
    struct Msg
    {
        int m_rate;       // +0x00（makeRequest param_1）
        int m_hour;       // +0x04（makeRequest param_2）
        int m_eventDay;   // +0x08（makeRequest param_3）
    };

    class Reward
    {
    public:
        Reward(int rate, bool flag);   // ORIG 0x8160c4a：short+char 布局
        void operator()(CUser* user);  // ORIG 0x8160c6e

        unsigned short m_rate;   // +0x00
        bool m_flag;             // +0x02
    };

    InterSelectCreateDnfEventInfo();             // ORIG 0x816201a
    virtual ~InterSelectCreateDnfEventInfo();    // ORIG D1 0x816203e / D0 0x816206e
    virtual int dispatch_sig(CUser* user, char* data, int len);  // ORIG 0x8160d5e
    static void makeRequest(int hour, int rate, int eventDay);   // ORIG 0x8160ef6
};

#endif  // GAME_INTERSELECTCREATEDNFEVENTINFO_H_
