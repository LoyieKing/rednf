// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9_StructMisc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDungeon.h"        // RecoveryInfo（ORIG D1 0x088f9346）
#include "CItemList.h"       // stCeraShopBonusItemInfo_t（ORIG D1 0x089363da）
#include "STPacketLog.h"     // ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030
#include "DataManagerInitDepends.h"   // TOD_UserState 权威定义

// ORIG _Z22CheckDailyScheduleTimeill（T 0x0846c0a8，定义 GameUtils.cpp）——
// C++ linkage，误用 extern "C" 会产生 C 符号导致链接缺口。
#include "W9_StructMisc/CheckDailyScheduleTime.inc"
                            long lCurTime);

// [b28d-PoolMisc2 GI-q06 纯移位] TU 首符号需为 TOD_UserState 构造体外联定义
// （ORIG T 0x08643460）⇒ 定义块整体上移至本 TU 第一个发射位置（类声明来自上方
// DataManagerInitDepends.h，空体无额外依赖）。file-scope 对象声明序一字未动。
// ORIG T 0x8643460：charac_expand::CData ctor 后写本类 vptr，再 call
// SIG_REQUEST_TOD_USER_STATE::C1(this+0x8)（0x08491f68）——其构造对数据区
// +0xc（即内嵌 m_data.m_todayLayer）以 TOD_Layer(0) 初始化，其余字段不动。
// 以真实内嵌成员 m_data 建模 ⇒ 编译器发 call SIG::C1，与 ORIG 汇编同构。
#include "W9_StructMisc/TOD_UserState.inc"
{
}


// ---- RecoveryInfo（ORIG W 0x088f9346：push ebp / mov esp,ebp / pop ebp / ret，
//      即空体析构；vector<RecoveryInfo> 元素销毁触发）----

// ---- stCeraShopBonusItemInfo_t（ORIG W 0x089363da：空体析构）----
#include "W9_StructMisc/dtor_stCeraShopBonusItemInfo_t.inc"

// ---- STPacketLog（ORIG C1 0x08481100：构造 string@+0x10；
//      D1 0x084810ea：析构 string@+0x10）----
#include "W9_StructMisc/STPacketLog.inc"
#include "W9_StructMisc/dtor_STPacketLog.inc"

// ---- STPacketLog::Compare（ORIG 0x08481030：cmp a->m_count, b->m_count；
//      jle → return false（a<=b），否则 true）----
#include "W9_StructMisc/Compare.inc"
{
    if (a->m_count > b->m_count)
        return true;
    return false;
}

// ============================================================================
// TOD_UserState（ORIG T 0x08643460 / D1 0x086434ae / D0 0x086434de /
//   ResetDaily 0x08643624 / _saveData 0x08643642 / _reset 0x086437ec /
//   loadData 0x086434fc / getData 0x086435ca）
// ============================================================================
#include "CUser.h"                 // CUser::GetUID/get_charac_no/get_charac_level
#include "GlobalData.h"            // s_stream_pool/s_msgq_mgr/s_systemTime_/s_pTowerOfDespairMgr
#include "CEnvironment.h"          // G_CEnvironment()->m_serverEnvir.m_fatigueTime
#include "CStreamGuard.h"          // CStreamGuard/GetInBuffer
#include "MsgQueueMgr.h"


#include "W9_StructMisc/dtor_TOD_UserState.inc"

// ORIG T 0x086438fc _ZN13TOD_UserState13setEnterLayerERK9TOD_Layer（0x12，
// LinkFinal2 核销）：仅写当日层 +0x14 = layer.m_layer（objdump 逐指令转写）。
#include "W9_StructMisc/setEnterLayer.inc"
{
    m_data.m_todayLayer = layer;    // ORIG movzwl (%edx); mov %dx,0x14(%eax)
}

// ORIG T 0x08643624：enterCount=0 + CData::alter()
#include "W9_StructMisc/ResetDaily.inc"
{
    m_data.m_enterCount = 0;
    alter();
}

// ORIG T 0x086437ec：+8/+0xc/+0x10 清零 → resetLayer(+0x14) → 清
// lastEnterLayer(+0x16)/lastEnterTime(+0x18)/lastClearTime(+0x1c)。
#include "W9_StructMisc/_reset.inc"
{
    m_data.m_characNo = 0;
    m_data.m_firstEnterTime = 0;
    m_data.m_enterCount = 0;
    // ORIG 以 TOD_Layer(this+0x14)::resetLayer 调用（即 today 层视图重置），
    // 随后立即清 +0x16；此序与汇编一致。
    m_data.m_todayLayer.resetLayer();
    m_data.m_lastEnterLayer = 0;
    m_data.m_lastEnterTime = 0;
    m_data.m_lastClearTime = 0;
}

// ORIG T 0x086435ca const：
//   pData+0xe←this+0x16 / pData+0xc←this+0x14 / pData+4←this+0xc /
//   pData+8←this+0x10 / pData+0x10←this+0x18 / pData+0x14←this+0x1c
//   （ORIG 不回写 sig+0 ← this+8）。
#include "W9_StructMisc/getData.inc"
{
    SIG_REQUEST_TOD_USER_STATE& out =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    // [b28c-lane17] ORIG 0x086435ca 发射序 = +0x16→out+0xe、+0x14→out+0xc、
    // +0xc→out+4、+0x10→out+8、+0x18→out+0x10、+0x1c→out+0x14（-O0 逐句发射，
    // 书写序即机器码序）；原按偏移升序书写导致前两组语句块整体错位。
    out.m_lastEnterLayer = m_data.m_lastEnterLayer;      // src +0x16 → out +0x0e
    out.m_todayLayer = m_data.m_todayLayer;              // src +0x14 → out +0x0c
    out.m_firstEnterTime = m_data.m_firstEnterTime;      // src +0x0c → out +0x04
    out.m_enterCount = m_data.m_enterCount;              // src +0x10 → out +0x08
    out.m_lastEnterTime = m_data.m_lastEnterTime;        // +0x10
    out.m_lastClearTime = m_data.m_lastClearTime;        // +0x14
}

// ORIG T 0x086434fc：先虚调 _reset()，拷入 6 dword 数据区；
// 若当日跨天(按 serverEnvir.m_fatigueTime(+0x37c) 口径)再虚调 ResetDaily()；
// 有历史层且 lastEnterTime<0x4e275df1 时清层标记 + alter 脏标记。恒返 true。
// [b28d-23 N·N93] 槽/帧差（69/80）⇒ N。ORIG 0x086434fc
#include "W9_StructMisc/loadData.inc"
{
    _reset();
    SIG_REQUEST_TOD_USER_STATE& in =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    // [L8 2026-09-10 判据⑫/P83] ORIG 0x08643522 段是**结构整体赋值**（一次
    // `mov -0xc(%ebp),%edx` 后连发 6 条 `mov N(%edx),%ecx; mov %ecx,8+N(%eax)`，
    // N=0,4,8,0xc,0x10,0x14）⇒ 原码 `m_data = in;`（m_data 类型即
    // SIG_REQUEST_TOD_USER_STATE，sizeof 0x18，static_assert 已锁）。
    // 旧逐字段赋值（7 条）多 2 个具名临时槽（我方 -0x10/-0x14 vs ORIG 仅 -0xc）
    // 并多发 11 条指令（80 vs 69）。
    m_data = in;

    // [L8 2026-09-10 P0/P183] ORIG 0x0863545..0x086355a 段：curSec 结果升格 %esi、
    // lastPlayTick 升格 %ebx（跨后续 call 保活）后直接 `mov %esi,0x8(%esp)` /
    // `mov %ebx,0x4(%esp)` —— ORIG **无槽往返** ⇒ 原码无具名局部；我方两个
    // `const long` 局部落 -0x10/-0xc 槽并多 4 条 load/store。
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               pUser->getCurCharacLastPlayTick(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        ResetDaily();
    }
    if (m_data.m_lastEnterLayer != 0)
    {
        // [AxisB4 2026-09-10 轴B] ORIG 0x086435a3 `cmp $0x4e275df0,%eax; jg`
        // 为**有符号**序（Ghidra C：`*(int*)(this+0x18) < 0x4e275df1`）；
        // 旧文 `static_cast<unsigned int>(...) < 0x4e275df1u` 产 `ja` 属真语义分叉
        // （m_lastEnterTime 为负时判据相反）。向 ORIG 对齐去掉 unsigned cast。
        if (m_data.m_lastEnterTime < 0x4e275df1)
        {
            m_data.m_lastEnterLayer = 0;
        }
        alter();
    }
    return true;
}

// ORIG T 0x08643642：等级低于 TowerOfDespairMgr 最小等级时直接返回 true；
// 否则经 StreamPool 取流组包：<<0x278、<<UID、GetInBuffer 填 0x18 记录、
// MsgQueueMgr::put(Q2)。异常安全由局部 CStreamGuard 析构承担。
// [b28d-23 N·N93] ⇒ N。ORIG 0x08643642
#include "W9_StructMisc/_saveData.inc"
{
    TowerOfDespairMgr* mgr =
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    // [P3-#331 2026-09-11] P0/P183 删多余具名局部：ORIG 0x08643642 无 level 槽
    //（`mov %eax,%ebx` 直接保留调用结果于 callee-saved），我方 `const int level`
    // 多一槽（-0x18）并使帧 0x30 vs ORIG 0x20；比较改内联形态。
    if (pUser->get_charac_level() < static_cast<int>(mgr->GetMinLv()))
    {
        return true;
    }

    // [L8 2026-09-10 P259] ORIG 0x086436xx：Acquire 返回值直接经 %eax 传 ctor
    // （无 stream 具名局部）⇒ 我方 stream 槽（-0x14/-0x18）多 2 槽 + 4 条
    // load/store，并使帧 0x30 vs ORIG 0x20。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("TOD_UserState.cpp", 0x58), true);
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）与 `guard->GetInBuffer`
    // （operator-> pt()）标识调用形态，勿改为直接成员调用。
    (*guard)->operator<<(0x278);
    (*guard)->operator<<(static_cast<int>(pUser->GetUID()));
    SIG_REQUEST_TOD_USER_STATE* sig =
        guard->GetInBuffer<SIG_REQUEST_TOD_USER_STATE>();
    // [P3-#331 2026-09-11] P183 删多余具名局部：ORIG 0x086436f3 直接
    // `cmpl $0x0,-0xc(%ebp); jne <then>`（无 setne/mov 的 bool 槽）；
    // 我方 `bool ok = (sig != 0)` 多 1 槽 + 3 条（setne/movb/cmpb）。
    if (sig == 0)
    {
        return false;
    }
    memset(sig, 0, sizeof(*sig));
    sig->m_characNo = pUser->get_charac_no(-1);
    // [P3-#331 2026-09-11] ORIG 0x0864374e 先存 +0xe（this+0x16=m_lastEnterLayer）
    // 再 0x08643762 存 +0xc（this+0x14=m_todayLayer）；原顺序相反多 1 处交换。
    sig->m_lastEnterLayer = m_data.m_lastEnterLayer;   // sig+0xe = this+0x16
    sig->m_todayLayer = m_data.m_todayLayer;           // sig+0xc = this+0x14
    sig->m_firstEnterTime = m_data.m_firstEnterTime;   // sig+4=+0xc
    sig->m_enterCount = m_data.m_enterCount;           // sig+8=+0x10
    sig->m_lastEnterTime = m_data.m_lastEnterTime;     // sig+0x10=+0x18
    sig->m_lastClearTime = m_data.m_lastClearTime;     // sig+0x14=+0x1c
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(Q2)
    return true;
}
