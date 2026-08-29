// CPremiumGoldCard —— 黄金卡充值事件。
// 布局（ORIG ctor 0x0811c432 实证）：仅 CEventBase 基区，无自有成员。
// 全部 6 方法为 ORIG W 弱符号（C1 0x0811c432 / D1 0x0811c44e / D0 0x0811c47e /
// StartEvent 0x0811c49c / StartEvent(Word_Param) 0x0811c4ee / EndEvent 0x0811c508，
// objdump 转写）。
#ifndef DNF_GAME_CPREMIUMGOLDCARD_H_
#define DNF_GAME_CPREMIUMGOLDCARD_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPremiumGoldCard : public CEventBase
{
public:
    CPremiumGoldCard();
    virtual ~CPremiumGoldCard();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPREMIUMGOLDCARD_H_
