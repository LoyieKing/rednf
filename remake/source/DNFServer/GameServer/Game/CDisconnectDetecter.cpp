// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDisconnectDetecter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— disconnect_detecter::CDisconnectDetecter（断线检测器，
// G2-4 批次）。逐函数对照 docs/class_func_reports/disconnect_detecter__CDisconnectDetecter.md
// 与 ORIG 反汇编实现；数据流参照 Inter_Disconnect / Inter_DisconnectMonitorServer /
// Inter_HadesDisConnect / TimerDetectDisconnect 报告。
// 布局：+0x00 std::map<DetectedObjectKey,int> m_map（sizeof 0x18）
// DetectedObjectKey：+0 uid（CUser::GetUID）| +4 uniqueId（get_unique_id & 0xffff）
// map value = 最后收包时间（CSystemTime::getCurSec），超时 30s 判断线。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19disconnect_detecter19CDisconnectDetecter10_IsTimeoutEi`（ORIG 0x847260d）。
#include <iostream>

#include <map>
#include <utility>
#include "TimerQueue.h"

// [GI 调序 P327] CDisconnectDetecter 类声明须在 _IsTimeout 定义前可见，
// 故把该头 include 提前到 TU 顶部（原位置在 namespace 块之前）。
#include "CDisconnectDetecter.h"

TimerQueue* G_TimerQueue();  // ORIG W 0x80f647c

// ---------------------------------------------------------------------------
// 外部依赖最小声明（实现属其它 TU）
// ---------------------------------------------------------------------------
class CSystemTime
{
public:
    int getCurSec();   // 0x080cbc9e
};

namespace GlobalData
{
extern CSystemTime s_systemTime_;   // 0x0941f714
}

class CUser
{
public:
    unsigned int GetUID() const;      // 0x080c8c96
    int get_unique_id() const;        // 0x080da37c
    void OnDisconnect();              // 0x086804ce
};

class CGameManager
{
public:
    CUser* getUser(int slotId, int increId);   // 0x082947a4
};

CGameManager* G_CGameManager();       // 0x080cc18e

class TimerDetectDisconnect
{
public:
    static void registNextTimer();  // ORIG T 0x086374e0
};

// [GI 调序 P327] `_IsTimeout` 定义前移为本 TU 首个强符号 ⇒
// `_GLOBAL__I__ZN19disconnect_detecter19CDisconnectDetecter10_IsTimeoutEi`（ORIG 0x0847260d，
// 符号本体 ORIG T 0x084721f4）。只移函数定义，不动任何带动态初始化的对象。
namespace disconnect_detecter
{
#include "CDisconnectDetecter/_IsTimeout.inc"

}  // namespace disconnect_detecter

#include "CDisconnectDetecter/registNextTimer.inc"
{
    int interval = 10;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x6d, interval, 0, 0);
}

// cMyTrace 由 TimerQueue.h 间接引入的 GameTypes.h 声明（实现属独立 TU）。

namespace disconnect_detecter
{

// ============================================================================
// DetectedObjectKey 实现
// ============================================================================

#include "CDisconnectDetecter/DetectedObjectKey.inc"


bool DetectedObjectKey::operator<(const DetectedObjectKey& other) const
{
    if (m_uid < other.m_uid)
    {
        return true;
    }
    if (m_uid == other.m_uid)
    {
        if (m_uniqueId < other.m_uniqueId)
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// CDisconnectDetecter 实现
// ============================================================================

#include "CDisconnectDetecter/CDisconnectDetecter.inc"


#include "CDisconnectDetecter/dtor_CDisconnectDetecter.inc"


#include "CDisconnectDetecter/RegisterUser.inc"


#include "CDisconnectDetecter/UnregisterUser.inc"


#include "CDisconnectDetecter/RefreshRecvTime.inc"


#include "CDisconnectDetecter/DoDetectingDisconnect.inc"


#include "CDisconnectDetecter/OnDisconnect.inc"


#include "CDisconnectDetecter/_GetUser.inc"


CDisconnectDetecter* GetInstanceDisconnectDetecter()
{
    static CDisconnectDetecter instance;
    return &instance;
}

}  // namespace disconnect_detecter
