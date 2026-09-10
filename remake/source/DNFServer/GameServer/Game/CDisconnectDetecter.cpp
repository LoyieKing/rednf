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
{
    if (GlobalData::s_systemTime_.getCurSec() - time > 30)
    {
        return true;
    }
    return false;
}
}  // namespace disconnect_detecter

// ---- TimerDetectDisconnect::registNextTimer @ 0x086374e0 ----
// 断线检测器定时器：10 秒后触发 dispatch_sig(0x6d)，若 DoDetectingDisconnect
// 仍检测到断线用户则再次登记（自续期），见 TimerDetectDisconnect 报告。
// ORIG 以 int 局部（-0xc(%ebp)）承载 10 再传参，帧 sub $0x38。
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
    : m_uid(uid), m_uniqueId(uniqueId)
{
}

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
    : m_map()
{
}

#include "CDisconnectDetecter/dtor_CDisconnectDetecter.inc"
{
}

// [b28d-lane16 NEAR] ORIG 08472272：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "CDisconnectDetecter/RegisterUser.inc"
{
    int mapSize = m_map.size();
    long curSec = GlobalData::s_systemTime_.getCurSec();
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::pair<std::map<DetectedObjectKey, int>::iterator, bool> ret =
        m_map.insert(std::make_pair<DetectedObjectKey, long>(key, curSec));
    if (ret.second == false)
    {
        // [b28e-N5 · P221 全表达式临时链] 构造即调用。4.4 对「具名 cMyTrace 对象」给深槽
        //   (-0x48)，对「表达式临时」给浅槽 ⇒ 临时链使 trace 归位 ORIG 的 -0x1c，并连带把
        //   insert 返回槽归位 -0x48、ret.second 读取归位 -0x44（ORIG 0x08472317 段与
        //   our .o 逐槽对照实证；探针 /tmp/r5/n5/mgr/CDisconnectDetecter）。
        //   残留：curSec/key/make_pair/pair 四槽与 ORIG 呈镜像（4.4 把表达式临时放浅区、
        //   具名对象放深区；4.1 相反），属 N82附 结构性残差，试过「先声明后赋值」形 ⇒ 多发
        //   2×iterator C1Ev + 2 次拷贝（帧 0x54→0x64）反劣，已弃。
        //   读数（n1_v 口径）：NEAR 67/68，reloc 归一后行级 diff 11→9（净 −2）。
        cMyTrace(__PRETTY_FUNCTION__, 0x25, 0)("RegisterUser() wrong");
        return 0;
    }
    if (mapSize == 0)
    {
        TimerDetectDisconnect::registNextTimer();
    }
    return 1;
}

#include "CDisconnectDetecter/UnregisterUser.inc"
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        m_map.erase(it);
        return true;
    }
    return false;
}

#include "CDisconnectDetecter/RefreshRecvTime.inc"
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        it->second = GlobalData::s_systemTime_.getCurSec();
    }
    else
    {
        return false;
    }
    return true;
}

#include "CDisconnectDetecter/DoDetectingDisconnect.inc"
{
    for (std::map<DetectedObjectKey, int>::iterator it = m_map.begin();
         it != m_map.end(); )
    {
        if (_IsTimeout(it->second))
        {
            CUser* user = _GetUser(it->first.m_uid, it->first.m_uniqueId);
            if (user != 0)
            {
                OnDisconnect(user);
            }
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    return !m_map.empty();
}

#include "CDisconnectDetecter/OnDisconnect.inc"
{
    user->OnDisconnect();
}

#include "CDisconnectDetecter/_GetUser.inc"
{
    CUser* user = G_CGameManager()->getUser(uid, 2);
    if (user != 0)
    {
        if ((unsigned short)user->get_unique_id() != uniqueId)
        {
            return 0;
        }
    }
    return user;
}

CDisconnectDetecter* GetInstanceDisconnectDetecter()
{
    static CDisconnectDetecter instance;
    return &instance;
}

}  // namespace disconnect_detecter
