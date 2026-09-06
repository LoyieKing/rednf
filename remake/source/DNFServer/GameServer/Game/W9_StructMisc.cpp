// ============================================================================
// W9_StructMisc —— Wave9 杂项 struct 生命周期符号（worklist
// /tmp/only_orig/{RecoveryInfo,stCeraShopBonusItemInfo_t,STPacketLog,
// TOD_UserState}.txt）。全部为 ORIG 真实符号；声明在各权威头。
// ============================================================================

#include "CDungeon.h"        // RecoveryInfo（ORIG D1 0x088f9346）
#include "CItemList.h"       // stCeraShopBonusItemInfo_t（ORIG D1 0x089363da）
#include "STPacketLog.h"     // ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030
#include "DataManagerInitDepends.h"   // TOD_UserState 权威定义

// ORIG _Z22CheckDailyScheduleTimeill（T 0x0846c0a8，定义 GameUtils.cpp）——
// C++ linkage，误用 extern "C" 会产生 C 符号导致链接缺口。
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);


// ---- RecoveryInfo（ORIG W 0x088f9346：push ebp / mov esp,ebp / pop ebp / ret，
//      即空体析构；vector<RecoveryInfo> 元素销毁触发）----
RecoveryInfo::~RecoveryInfo() {}

// ---- stCeraShopBonusItemInfo_t（ORIG W 0x089363da：空体析构）----
stCeraShopBonusItemInfo_t::~stCeraShopBonusItemInfo_t() {}

// ---- STPacketLog（ORIG C1 0x08481100：构造 string@+0x10；
//      D1 0x084810ea：析构 string@+0x10）----
STPacketLog::STPacketLog() : m_name() {}
STPacketLog::~STPacketLog() {}

// ---- STPacketLog::Compare（ORIG 0x08481030：cmp a->m_count, b->m_count；
//      jle → return false（a<=b），否则 true）----
bool STPacketLog::Compare(const STPacketLog* a, const STPacketLog* b)
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


TOD_UserState::~TOD_UserState() {}

// ORIG T 0x086438fc _ZN13TOD_UserState13setEnterLayerERK9TOD_Layer（0x12，
// LinkFinal2 核销）：仅写当日层 +0x14 = layer.m_layer（objdump 逐指令转写）。
void TOD_UserState::setEnterLayer(const TOD_Layer& layer)
{
    m_todayLayer = layer.m_layer;   // ORIG movzwl (%edx); mov %dx,0x14(%eax)
}

// ORIG T 0x08643624：enterCount=0 + CData::alter()
void TOD_UserState::ResetDaily()
{
    m_enterCount = 0;
    alter();
}

// ORIG T 0x086437ec：+8/+0xc/+0x10 清零 → resetLayer(+0x14) → 清
// lastEnterLayer(+0x16)/lastEnterTime(+0x18)/lastClearTime(+0x1c)。
void TOD_UserState::_reset()
{
    m_field8 = 0;
    m_firstEnterTime = 0;
    m_enterCount = 0;
    // ORIG 以 TOD_Layer(this+0x14)::resetLayer 调用（即 today 层视图重置），
    // 随后立即清 +0x16；此序与汇编一致。
    reinterpret_cast<TOD_Layer*>(&m_todayLayer)->resetLayer();
    m_lastEnterLayer = 0;
    m_lastEnterTime = 0;
    m_lastClearTime = 0;
}

// ORIG T 0x08643460：charac_expand::CData ctor 后写本类 vptr，再内嵌构造
// SIG_REQUEST_TOD_USER_STATE —— 后者对数据区 +0xc（即本类 m_todayLayer）
// 以 TOD_Layer(0) 初始化（ORIG SIG C1 0x08491f68 实证，其余字段不动）。
TOD_UserState::TOD_UserState()
{
    const TOD_Layer zeroLayer(0);
    *reinterpret_cast<TOD_Layer*>(&m_todayLayer) = zeroLayer;
}
// ORIG T 0x086435ca const：
//   pData+0xe←this+0x16 / pData+0xc←this+0x14 / pData+4←this+0xc /
//   pData+8←this+0x10 / pData+0x10←this+0x18 / pData+0x14←this+0x1c
//   （ORIG 不回写 sig+0 ← this+8）。
void TOD_UserState::getData(char* pData) const
{
    SIG_REQUEST_TOD_USER_STATE& out =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    out.m_firstEnterTime = m_firstEnterTime;// +0x04
    out.m_enterCount = m_enterCount;        // +0x08
    out.m_todayLayer = m_todayLayer;        // +0x0c
    out.m_lastEnterLayer = m_lastEnterLayer;// +0x0e
    out.m_lastEnterTime = m_lastEnterTime;  // +0x10
    out.m_lastClearTime = m_lastClearTime;  // +0x14
}

// ORIG T 0x086434fc：先虚调 _reset()，拷入 6 dword 数据区；
// 若当日跨天(按 serverEnvir.m_fatigueTime(+0x37c) 口径)再虚调 ResetDaily()；
// 有历史层且 lastEnterTime<0x4e275df1 时清层标记 + alter 脏标记。恒返 true。
bool TOD_UserState::loadData(CUser* pUser, char* pData)
{
    _reset();
    SIG_REQUEST_TOD_USER_STATE& in =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    m_field8 = in.m_characNo;               // ORIG *(this+8)=*(param_2)
    m_firstEnterTime = in.m_firstEnterTime;
    m_enterCount = in.m_enterCount;
    m_todayLayer = in.m_todayLayer.GetLayer();
    m_lastEnterLayer = in.m_lastEnterLayer;
    m_lastEnterTime = in.m_lastEnterTime;
    m_lastClearTime = in.m_lastClearTime;

    const long curSec = GlobalData::s_systemTime_.getCurSec();
    const long lastPlayTick = pUser->getCurCharacLastPlayTick();
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               lastPlayTick, curSec))
    {
        ResetDaily();
    }
    if (m_lastEnterLayer != 0)
    {
        if (static_cast<unsigned int>(m_lastEnterTime) < 0x4e275df1u)
        {
            m_lastEnterLayer = 0;
        }
        alter();
    }
    return true;
}

// ORIG T 0x08643642：等级低于 TowerOfDespairMgr 最小等级时直接返回 true；
// 否则经 StreamPool 取流组包：<<0x278、<<UID、GetInBuffer 填 0x18 记录、
// MsgQueueMgr::put(Q2)。异常安全由局部 CStreamGuard 析构承担。
bool TOD_UserState::_saveData(CUser* pUser)
{
    const int level = pUser->get_charac_level();
    TowerOfDespairMgr* mgr =
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    if (level < static_cast<int>(mgr->GetMinLv()))
    {
        return true;
    }

    Stream* stream = GlobalData::s_stream_pool->Acquire("TOD_UserState.cpp",
                                                        0x58);
    CStreamGuard guard(stream, true);
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）与 `guard->GetInBuffer`
    // （operator-> pt()）标识调用形态，勿改为直接成员调用。
    (*guard)->operator<<(0x278);
    (*guard)->operator<<(static_cast<int>(pUser->GetUID()));
    SIG_REQUEST_TOD_USER_STATE* sig =
        guard->GetInBuffer<SIG_REQUEST_TOD_USER_STATE>();
    bool ok = (sig != 0);
    if (ok)
    {
        memset(sig, 0, sizeof(*sig));
        sig->m_characNo = pUser->get_charac_no(-1);
        sig->m_todayLayer = m_todayLayer;           // ORIG save 顺序：sig+0xc=+0x14
        sig->m_lastEnterLayer = m_lastEnterLayer;   // sig+0xe=+0x16
        sig->m_firstEnterTime = m_firstEnterTime;   // sig+4=+0xc
        sig->m_enterCount = m_enterCount;           // sig+8=+0x10
        sig->m_lastEnterTime = m_lastEnterTime;     // sig+0x10=+0x18
        sig->m_lastClearTime = m_lastClearTime;     // sig+0x14=+0x1c
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(Q2)
    }
    return ok;
}
