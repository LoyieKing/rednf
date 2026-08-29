// CDoubleGoldCardEvent —— 双倍金币卡事件（CEventBase 叶事件）。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写）：C1 0x0811c30a / D1 0x0811c326 /
// D0 0x0811c356 / StartEvent() 0x0811c374 / StartEvent(Word_Param) 0x0811c3c6（thunk）/
// EndEvent 0x0811c3e0（均弱符号 W；vtable 0x08b520c0）。
// 注意：该 thunk ORIG 为「槽值→%edx、this→%eax」形态，本工具链 gcc4.4.7
// 对 `StartEvent();` 固定生成「槽值→%eax、this→%edx」反相（N7 负例：穷举
// 临时/引用/限定名均无效，语义等价，单 TU 对比判 NEAR 属豁免，勿再试）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CDOUBLEGOLDCARDEVENT_H_
#define DNF_GAME_CDOUBLEGOLDCARDEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CDoubleGoldCardEvent : public CEventBase
{
public:
    CDoubleGoldCardEvent();
    virtual ~CDoubleGoldCardEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CDOUBLEGOLDCARDEVENT_H_
