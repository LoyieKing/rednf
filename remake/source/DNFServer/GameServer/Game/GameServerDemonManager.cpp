// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameServerDemonManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GameServerDemonManager.cpp — ORIG 全套方法实现（LinkClosureD 批次）
//
// 报告：docs/class_func_reports/GameServerDemonManager/（README/SetUserProhibitFlag/
// SetNoticeMessage/Notice10MinShutDownMessage/~GameServerDemonManager 全文已读）。
// 报告缺失符号（C1/GetNoticeMessage/IsProhibitConnectState/GameserverDemonManagerInstance）
// 以 nm -S 地址 + objdump -d 逐指令转写（见 §3.2 兜底规则）。
// 布局（ORIG C1 0x080f6330 / SetNoticeMessage 0x080f6280 实证，sizeof 0x101）：
//   +0x00 unsigned char m_prohibitFlag（SetUserProhibitFlag 写 byte；IsProhibitConnectState 读 byte）
//   +0x01 unsigned char m_noticeLen（SetNoticeMessage 写 byte；C1 清零）
//   +0x02 char m_notice[0xff]（C1 memset 0xff；SetNoticeMessage memcpy len）
// 单例：GameserverDemonManagerInstance() 函数局部 static（guard 0x093faba0 /
// instance 0x093fabc0，__cxa_atexit(~D1 0x080f621c)——P20 形态）。

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__Z30GameserverDemonManagerInstancev`（ORIG 0x80f6313）。
#include <iostream>

#include "GameServerDemonManager.h"
#include "TimerQueue.h"          // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x80f6313
// `_Z30GameserverDemonManagerInstancev`。
GameServerDemonManager* GameserverDemonManagerInstance()      // T 0x080f61b8
{
    static GameServerDemonManager instance;
    return &instance;
}

#include "GameServerDemonManager/GameServerDemonManager.inc"


#include "GameServerDemonManager/dtor_GameServerDemonManager.inc"


#include "GameServerDemonManager/SetUserProhibitFlag.inc"


#include "GameServerDemonManager/IsProhibitConnectState.inc"


#include "GameServerDemonManager/SetNoticeMessage.inc"


#include "GameServerDemonManager/GetNoticeMessage.inc"


#include "GameServerDemonManager/Notice10MinShutDownMessage.inc"


// [GI-ioinit L21 注] GameserverDemonManagerInstance 定义已前移至 ctor 之前（P327 键名）。
