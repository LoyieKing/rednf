// CCollectArchieveEventLog —— 收集图鉴事件日志（CEventBase 叶事件）。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写）：C1 0x08116b48 / D1 0x08116b64 /
// D0 0x08116b94 / StartEvent() 0x08116bb2 / StartEvent(Word_Param) 0x08116c04（thunk）/
// EndEvent 0x08116c1e（均弱符号 W；vtable 0x08b4f4a0）。
// StartEvent(Word_Param) 经 vtable slot 2 虚调 StartEvent()（0x08116c1a ff d2）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CCOLLECTARCHIEVEEVENTLOG_H_
#define DNF_GAME_CCOLLECTARCHIEVEEVENTLOG_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CCollectArchieveEventLog : public CEventBase
{
public:
    CCollectArchieveEventLog();
    virtual ~CCollectArchieveEventLog();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CCOLLECTARCHIEVEEVENTLOG_H_
