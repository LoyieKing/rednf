// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CShutdowManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 定义 SendLastMsgDBQueue（ORIG _ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser）
// 及默认构造（GetInstanceShutdowManager 的 static 单例需要）。
// （注：上游单例 GetInstanceShutdowManager 由 CGameManager.cpp TU 提供。）

#include "CShutdowManager.h"
#include "ThreadInterface.h"

#include <cstdio>

#include "CUser.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

#include "CShutdowManager/CShutdowManager.inc"


#include "CShutdowManager/SendLastMsgDBQueue.inc"


#include "CShutdowManager/WaitUntilExitThread.inc"


CShutdowManager* GetInstanceShutdowManager()
{
    // [W16ASymNameSeg2 2026-09-11 符号名轴] ORIG 单例名 = `instance`（非 `s`）：
    //   _ZZ25GetInstanceShutdowManagervE8instance  @0x0943bc90（b,16B 对象）
    //   _ZGVZ25GetInstanceShutdowManagervE8instance @0x093fa760（V,guard）
    // ⇒ `s`→`instance`，两条 OUR-only 归零、四数不变（2/8/0/1/1）。
    static CShutdowManager instance;
    return &instance;
}
#include "CShutdowManager/Reset.inc"


#include "CShutdowManager/EnableShutdown.inc"


#include "CShutdowManager/OnAllUserLogout.inc"


#include "CShutdowManager/IncrementLastMsgResponseCount.inc"


#include "CShutdowManager/GetLastMsgCount.inc"


#include "CShutdowManager/GetLastMsgResponseCount.inc"

