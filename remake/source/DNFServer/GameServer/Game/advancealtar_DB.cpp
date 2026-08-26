// df_game_r Game/ advancealtar DBAndExchangeServerProc + APSystem CDBAndExchangeServerProc
// 独立注册 TU（2026-08-23）。实现：
//   advancealtar::DBAndExchangeServerProc::getFromMemory          ORIG T 0x0813394c
//   advancealtar::CharacAdvanceAltarManager::getCharacAdvanceAltarDbData ORIG T 0x08131710
//   advancealtar::CharacAdvanceAltarManager::isUpdated           ORIG T 0x081316a0
//   advancealtar::AdvanceAltarData::{get,getSlot,getBuyItem}     ORIG T 0x088996d8 / ...
//   advancealtar::DB_Update::makeRequest                         ORIG T 0x0813eb38
//   APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory ORIG T 0x08121616（纯空函数）
#include <cstring>
#include "CUser.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "advancealtar.h"
#include "PacketTypes.h"  // MSG_CHARAC_SELECT 完整类型（requestLoadFromDBAndES 读 m_slot）

namespace advancealtar
{

// ---- AdvanceAltarData 数据模型（advancealtar__AdvanceAltarData 报告）----
AdvanceAltarData::AdvanceAltarData()
    : m_stages(), m_buyItems1(), m_buyItems2(), m_buyItems3(),
      m_slots1(), m_slots2(), m_rewards(), m_update(0)
{
}

AdvanceAltarData::~AdvanceAltarData()
{
}

// ORIG T 0x08899b4e：把 map<uchar,_Slot> 拷入定长槽数组（槽 0 长度 10）。
void AdvanceAltarData::getSlot(const std::map<unsigned char,_Slot>& m, _Slot* out, int count) const
{
    int local_10 = 0;
    for (std::map<unsigned char,_Slot>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        const _Slot& slot = it->second;
        if (slot.m_int1 == 0)               // 源槽 m_int1==0 → 跳过
        {
            ++it;
        }
        else if (out[local_10].m_int1 == 0)  // 目标槽 m_int1==0 才写入
        {
            std::memcpy(&out[local_10], &slot, 10);
            ++it;
            ++local_10;
        }
        else
        {
            ++local_10;
        }
    }
}

// ORIG T 0x08899a3a：把 map<int,_BuyItem> 拷入定长购买项数组（每项 12）。
void AdvanceAltarData::getBuyItem(const std::map<int,_BuyItem>& m, _BuyItem* out, int count) const
{
    int local_10 = 0;
    for (std::map<int,_BuyItem>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        const _BuyItem& bi = it->second;
        if (bi.m_field4 < 1)                 // 源 m_field4 < 1 → 跳过
        {
            ++it;
        }
        else if (out[local_10].m_field4 < 1)  // 目标 m_field4 < 1 才写入
        {
            std::memcpy(&out[local_10], &bi, 0xc);
            ++it;
            ++local_10;
        }
        else
        {
            ++local_10;
        }
    }
}

// ORIG T 0x088996d8：把 AdvanceAltarData 序列化到 _AdvanceAltarDbData（0x518）。
void AdvanceAltarData::get(_AdvanceAltarDbData& out) const
{
    out.m_update = m_update;                          // +0x517 = this[0xcc]
    std::memcpy(&out, this, 0x21);                    // 前 0x21（RidableBase）

    int local_10 = 0;
    for (std::map<int,_Stage>::const_iterator it = m_stages.begin();
         it != m_stages.end() && local_10 <= 0x31; )
    {
        const _Stage& st = it->second;
        if (st.m_field0 < 1)                          // value.m_field0 < 1 → 跳过
        {
            ++it;
        }
        else
        {
            std::memcpy(reinterpret_cast<char*>(&out) + (local_10 + 4) * 8 + 1, &st, 8);
            ++it;
            ++local_10;
        }
    }

    getSlot(m_slots1, reinterpret_cast<_Slot*>(reinterpret_cast<char*>(&out) + 0x1b1), 0xc);
    getSlot(m_slots2, reinterpret_cast<_Slot*>(reinterpret_cast<char*>(&out) + 0x1b1), 0xc);
    getBuyItem(m_buyItems1, reinterpret_cast<_BuyItem*>(reinterpret_cast<char*>(&out) + 0x229), 0x32);
    getBuyItem(m_buyItems2, reinterpret_cast<_BuyItem*>(reinterpret_cast<char*>(&out) + 0x229), 0x32);
    getBuyItem(m_buyItems3, reinterpret_cast<_BuyItem*>(reinterpret_cast<char*>(&out) + 0x229), 0x32);

    local_10 = 0;
    for (std::map<int,_Reward>::const_iterator it = m_rewards.begin();
         it != m_rewards.end() && local_10 <= 0x1d; )
    {
        const _Reward& rw = it->second;
        if (rw.m_field0 < 1)
        {
            ++it;
        }
        else
        {
            std::memcpy(reinterpret_cast<char*>(&out) + local_10 * 5 + 0x481, &rw, 5);
            ++it;
            ++local_10;
        }
    }
}

// ---- CharacAdvanceAltarManager（CUser.h 声明）----
// ORIG T 0x08131710：把管理器内单个 AdvanceAltarData 拷到 _CharacAdvanceAltarDbData.m_db。
void CharacAdvanceAltarManager::getCharacAdvanceAltarDbData(_CharacAdvanceAltarDbData& out) const
{
    // ORIG 循环 local_10 <= 0（一次）
    for (int local_10 = 0; local_10 < 1; ++local_10)
    {
        m_data.get(*reinterpret_cast<_AdvanceAltarDbData*>(
            reinterpret_cast<char*>(&out) + local_10 * 0x518 + 4));
    }
}

// ORIG T 0x08899244（objdump 全文核对）：memset 前 0x21（_RidableBase）→
// m_update=0 → 7 个 map clear（stages/buyItems1..3/slots1/slots2/rewards）。
void AdvanceAltarData::reset()
{
    std::memset(this, 0, 0x21);
    m_update = 0;
    m_stages.clear();
    m_buyItems1.clear();
    m_buyItems2.clear();
    m_buyItems3.clear();
    m_slots1.clear();
    m_slots2.clear();
    m_rewards.clear();
}

// ORIG T 0x081316a0：检查首个 AdvanceAltarData 的更新标志（manager+0xd0 == m_data.m_update）。
bool CharacAdvanceAltarManager::isUpdated() const
{
    int local_8 = 0;
    for (;;)
    {
        if (0 < local_8)
            return false;
        if (reinterpret_cast<const unsigned char*>(this)[local_8 * 0xd0 + 0xd0] != 0)
            return true;
        ++local_8;
    }
}

// ORIG T 0x813159a：槽位坐骑有效时把免费票补为配置值（CDataManager+0x6364
// = m_altarConfig.m_0）并置 m_data.m_update=1。
void CharacAdvanceAltarManager::refillTicketFree()
{
    for (int i = 0; i < 1; ++i)
    {
        if (isValidRidableId(static_cast<advancealtar::RidableId::T>(m_data.m_ridableId)))
        {
            m_data.m_ticket = G_CDataManager()->m_altarConfig.m_0;
            m_data.m_update = 1;
        }
    }
}

// ORIG T 0x08131660（docs/class_func_reports/advancealtar__CharacAdvanceAltarManager/
// reset.md 全文）：m_field0=0；循环 local_10<=0 一次 reset +i*0xd0+4 处 AdvanceAltarData。
void CharacAdvanceAltarManager::reset()
{
    m_field0 = 0;
    for (int local_c = 0; local_c <= 0; ++local_c)
    {
        // 单实例布局：this+local_c*0xd0+4 即 m_data
        (*reinterpret_cast<AdvanceAltarData*>(
             reinterpret_cast<char*>(this) + local_c * 0xd0 + 4))
            .reset();
    }
}

// ----------------------------------------------------------------------------
// +0x00 承载 AdvanceAltarData* current（setCurrentAdvanceAltarData 0x08131254
// 实证：命中槽位写 this+4+i*0xd0，无匹配置 0；reset 置 0）。权威头以 int
// m_field0 承载，以下三个 const 访问器按 ORIG 语义经指针读取。
// ----------------------------------------------------------------------------

// ORIG T 0x081312d0（getCurrentRidableId.md 全文）：current 空 → 0；
// 否则 *current（AdvanceAltarData::m_ridableId，_RidableBase+0x00）。
int CharacAdvanceAltarManager::getCurrentRidableId() const
{
    const AdvanceAltarData* cur =
        reinterpret_cast<const AdvanceAltarData*>(static_cast<unsigned long>(m_field0));
    if (cur == 0)
    {
        return 0;
    }
    return cur->m_ridableId;
}

// ORIG T 0x081312ec（isAvailableTciket.md 全文）：current 空 → 0；
// 否则 *(int*)(cur+4) + *(int*)(cur+8) > 0（免费票 + 现持票）。
bool CharacAdvanceAltarManager::isAvailableTciket() const
{
    const AdvanceAltarData* cur =
        reinterpret_cast<const AdvanceAltarData*>(static_cast<unsigned long>(m_field0));
    if (cur == 0)
    {
        return false;
    }
    const int freeTickets =
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(cur) + 4);
    const int heldTickets =
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(cur) + 8);
    return (freeTickets + heldTickets) > 0;
}

// ORIG T 0x0813131a（isEnterStage.md 全文）：current 空 → 0；在
// current->m_stages(+0x24, map<int,_Stage>) find(stageId) 命中 → 1。
bool CharacAdvanceAltarManager::isEnterStage(int stageId) const
{
    const AdvanceAltarData* cur =
        reinterpret_cast<const AdvanceAltarData*>(static_cast<unsigned long>(m_field0));
    if (cur == 0)
    {
        return false;
    }
    return cur->m_stages.find(stageId) != cur->m_stages.end();
}

// ---- getTowerInfo（ORIG T 0x081328ea，getTowerInfo.md 全文）----
// current(+0x00) 空→0；current->m_buyItems3(+0x6c) find(towerIdx) 未命中→0；
// G_CDataManager()+0x631c（m_altarShop）.getBuyUpgradeDataVec(*current,2) 空→0；
// 向量中 m_nItemId==towerIdx 项未命中→0；getFieldDataPoint(买项.m_short8,
// FT_INDEX=1) 空→0；买项计数 m_short8==0→0；否则 *out1=FieldData+4(m_value)、
// *out2=(int)m_short8，返回 1。
bool CharacAdvanceAltarManager::getTowerInfo(int towerIdx, int& outValue,
                                             int& outCount) const
{
    const AdvanceAltarData* cur =
        reinterpret_cast<const AdvanceAltarData*>(static_cast<unsigned long>(m_field0));
    if (cur == 0)
    {
        return false;
    }
    const std::map<int, _BuyItem>& buys = cur->m_buyItems3;
    std::map<int, _BuyItem>::const_iterator it = buys.find(towerIdx);
    if (it != buys.end())
    {
        std::vector<BuyUpgradeData>* vec =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(
                cur->m_ridableId, AdvanceAltarShopType::T_2);
        if (vec == 0)
        {
            return false;
        }
        BuyUpgradeData* data = 0;
        for (std::vector<BuyUpgradeData>::iterator vi = vec->begin();
             vi != vec->end(); vi++)
        {
            if (vi->m_nItemId == towerIdx)
            {
                data = &*vi;
                break;
            }
        }
        if (data == 0)
        {
            return false;
        }
        const FieldData* point =
            data->getFieldDataPoint((int)it->second.m_short8, FieldType::FT_INDEX);
        if (point == 0)
        {
            return false;
        }
        if (it->second.m_short8 == 0)
        {
            return false;
        }
        outValue = point->m_value;
        outCount = (int)it->second.m_short8;
        return true;
    }
    return false;
}

// ---- DB_Update（advancealtar__DB_Update 报告）----
// ORIG T 0x0813eb38：opcode 0x1a | uid | memcpy 0x51c → DB_Q。
void DB_Update::makeRequest(int uid, const _CharacAdvanceAltarDbData& data)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1d9);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x1a);
    guard.operator*()->operator<<(uid);
    _CharacAdvanceAltarDbData* sig =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memcpy(sig, &data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DBAndExchangeServerProc（advancealtar__DBAndExchangeServerProc 报告）----
// ORIG T 0x0813394c：characNo→data.m_characNo；拷 AdvanceAltarData；isUpdated 则 DB_Update。
void DBAndExchangeServerProc::getFromMemory(const CUser& user, _CharacAdvanceAltarDbData& data)
{
    data.m_characNo = (unsigned int)user.getCurCharacNo();
    user.m_advanceAltar.getCharacAdvanceAltarDbData(data);
    if (user.m_advanceAltar.isUpdated())
        DB_Update::makeRequest((int)user.GetUID(), data);
}

// ---- requestLoadFromDBAndES（advancealtar__DBAndExchangeServerProc 报告全文）----
// ORIG T 0x08133784：reset 管理器 → Acquire stream(opcode 0x19) → uid →
// DisableCharacInfo(10) →（ValidLastLogin 时补 channelNo/characNo）→
// GetInBuffer<_CharacAdvanceAltarDbData> 清零 0x51c 后写 m_characNo →
// ValidLastLogin ? put 队列5 : put DB_Q(2) → 返回 1。
bool DBAndExchangeServerProc::requestLoadFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg)
{
    user.m_advanceAltar.reset();  // ORIG CharacAdvanceAltarManager::reset @0x08131660

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localglobal/global_AdvanceAltar.cpp", 0xc3a);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x19);
    guard.operator*()->operator<<(static_cast<int>(user.GetUID()));
    user.DisableCharacInfo(CACHE_ADVANCE_ALTAR);  // ORIG push 0xa（枚举名 Prod_DispatcherHelpers 落地）

    if (user.IsValidLastLoginCharac())
    {
        guard.operator*()->operator<<(user.GetLastLoginChannelNo());
        guard.operator*()->operator<<(user.GetLastLoginCharacNo());
    }

    _CharacAdvanceAltarDbData* data =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memset(data, 0, 0x51c);
    data->m_characNo = static_cast<unsigned int>(user.get_charac_no(static_cast<int>(msg.m_slot)));

    if (!user.IsValidLastLoginCharac())
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    else
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);  // 第 6 槽（ORIG 无枚举名）
    return true;
}

} // namespace advancealtar

namespace APSystem
{

// ORIG T 0x08121616（纯空函数）：行动力从内存缓存加载——本版本 ORIG 体为空。
void CDBAndExchangeServerProc::GetActionPointFromMemory(const CUser&, _SIG_LOAD_ACTION_POINT&)
{
}

} // namespace APSystem

// ---- CUser 跨类叶子助手（Prod_GlobalAdv；全局作用域，置于文件尾避免命名空间混淆）----

// ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION：
// movb $0,(this+0x8d0fd+section) —— m_pad8d0fd[section] 清零。
void CUser::DisableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 0;
}
