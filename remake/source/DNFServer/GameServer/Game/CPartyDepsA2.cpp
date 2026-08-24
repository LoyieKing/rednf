// ============================================================================
// CPartyDepsA2.cpp —— CParty 第一批依赖函数（PartyDepsA2，2026-08-24）
//
// 全部函数逐份完整读取 docs/class_func_reports/CParty/<Func>.md（## 汇编 +
// ## 反编译 C）后转写；依赖叶子函数按其自身报告转写：
//   ConditionLevelChkDungeon/isEventDungeon.md、isApplyLevel.md
//   TowerOfDespairMgr/{Get_CDungeonByLayer,Get_DungeonIndexByLayer,
//                      Get_APCIndexByLayer,GetBaseLayerIndex,SendMessageToCUser}.md
//   TOD_UserState/getTodayEnterLayer.md、TOD_Layer/TOD_Layer.md
//   （TOD_Layer::GetLayer 无独立报告，按 nm 0x0822ec40 直读 m_layer 实现，
//     标记"报告缺失"。）
// 定义仍缺失（blocked，声明已在权威头）：
//   TowerOfDespairMgr::CheckEnterable   ORIG T 0x086440ee（待 TOD 批次）
//   CUser::MakeTagCharacInfo            ORIG T 0x08690776（待 CUser 批次）
//   CUser::Send(PacketGuard&)           ORIG T 0x086485ba（声明 CUser.h，定义待批次）
// ============================================================================
#include "CParty.h"

#include <algorithm>
#include <stdio.h>
#include <vector>

#include "CDungeon.h"               // get_index/get_min_level/get_standard_level
#include "CDataManager.h"           // G_CDataManager/find_dungeon/m_conditionDungeon
#include "DataManagerInitDepends.h" // TowerOfDespairMgr/TOD_Layer/TOD_UserState
#include "TimerQueue.h"             // G_TimerQueue/TimerQueue::InsertTimer
#include "GlobalData.h"             // GlobalData::s_pTowerOfDespairMgr
#include "CWorldMap.h"              // IsInHellDungeon/m_field1c/m_field34
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "CInventory.h"             // check_item_exist/delete_item 完整类型
#include "cMyTraceNoop.h"

namespace
{

// ORIG：user+0xcac 为当前 CDungeon*。CUser 该偏移区未建模（无具名字段/访问器），
// 按 DisPatcherCore.cpp 对 party+0xcac 的只读视图同口径处理【推断：布局口径】。
struct CUserCurDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

}  // namespace

// ORIG 全局 _Z33getHellPartyEntranceItemNeedCounti（T 0x08891be6），
// 定义在 CParty.cpp；此处仅声明供 UseSealDoorItems 调用。
int getHellPartyEntranceItemNeedCount(int minLevel);

// ORIG T 0x0822ec40 _ZNK9TOD_Layer8GetLayerEv：返回首 ushort。
unsigned short TOD_Layer::GetLayer() const
{
    return m_layer;
}

// ORIG T 0x085fe7b4 _ZN9TOD_LayerC1Et（报告 TOD_Layer/TOD_Layer.md）：
// m_layer=v；>100 时 cMyTrace 告警并回卷为 1。
TOD_Layer::TOD_Layer(unsigned short layer)
{
    m_layer = layer;
    if (100 < m_layer)
    {
        cMyTrace trace("TOD_Layer::TOD_Layer(short unsigned int)", 0x10, 5);
        trace("@TOD: invalid layer setting : %d", static_cast<unsigned int>(layer));
        m_layer = 1;
    }
}

// ORIG T 0x086439c8 _ZNK9TOD_LayerplEi（报告缺失，按 nm 反汇编实现）：
// 构造 ret(m_layer+n)；ret.GetLayer()>100 → resetLayer()；sret 返回。
TOD_Layer TOD_Layer::operator+(int n) const
{
    TOD_Layer ret(static_cast<unsigned short>(m_layer + n));
    if (ret.GetLayer() > 100)
    {
        ret.resetLayer();
    }
    return ret;
}

// ============================================================================
// ConditionLevelChkDungeon::isEventDungeon —— ORIG T 0x089139d4
//   （报告 ConditionLevelChkDungeon/isEventDungeon.md）
//   m_unableDungeons 为空或不含 dungeonIdx → true。
// ============================================================================
bool ConditionLevelChkDungeon::isEventDungeon(int dungeonIdx)
{
    if (m_unableDungeons.empty())
    {
        return true;
    }
    std::vector<int>::iterator it = m_unableDungeons.begin();
    std::vector<int>::iterator endIt = m_unableDungeons.end();
    while (it != endIt)
    {
        if (!(*it == dungeonIdx))
        {
            ++it;
            continue;
        }
        return false;
    }
    return true;
}

// ============================================================================
// ConditionLevelChkDungeon::isApplyLevel —— ORIG T 0x08913a62
//   （报告 ConditionLevelChkDungeon/isApplyLevel.md）：m_applyLevel <= level。
// ============================================================================
bool ConditionLevelChkDungeon::isApplyLevel(int level)
{
    if (!(m_applyLevel > level))
    {
        return true;
    }
    return false;
}

// ============================================================================
// TOD_UserState::getTodayEnterLayer —— ORIG T 0x0864383e
//   （报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
//   *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1；
//   构造回卷语义见 TOD_Layer/TOD_Layer.md 与 operator+ 尾部 resetLayer）。
// ============================================================================
TOD_Layer TOD_UserState::getTodayEnterLayer() const
{
    // ORIG：return *(const TOD_Layer*)(this+0x14) + 1
    //   （经 TOD_Layer::operator+(int)，sret 链与 ORIG 一致）。
    const TOD_Layer* today =
        reinterpret_cast<const TOD_Layer*>(
            reinterpret_cast<const char*>(this) + 0x14);
    return *today + 1;
}

// ============================================================================
// TowerOfDespairMgr::Get_CDungeonByLayer —— ORIG T 0x08643d14
//   （报告 TowerOfDespairMgr/Get_CDungeonByLayer.md）
// ============================================================================
CDungeon* TowerOfDespairMgr::Get_CDungeonByLayer(unsigned int layer)
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    if (m_layerDungeons[layer] == 0)
    {
        m_layerDungeons[layer] =
            G_CDataManager()->find_dungeon(
                static_cast<int>(m_dungeonIndexOfLayer[layer]));
    }
    return m_layerDungeons[layer];
}

// ============================================================================
// TowerOfDespairMgr::Get_DungeonIndexByLayer —— ORIG T 0x08643d76
//   （报告 TowerOfDespairMgr/Get_DungeonIndexByLayer.md）
// ============================================================================
unsigned int TowerOfDespairMgr::Get_DungeonIndexByLayer(unsigned int layer)
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    return m_dungeonIndexOfLayer[layer];
}

// ============================================================================
// TowerOfDespairMgr::Get_APCIndexByLayer —— ORIG T 0x08643d9c
//   （报告 TowerOfDespairMgr/Get_APCIndexByLayer.md）：
//   层号在 m_npcLayers 中 → 返回层号；否则 -1；>100 → 0。
// ============================================================================
int TowerOfDespairMgr::Get_APCIndexByLayer(unsigned int layer)
{
    if (100 < layer)
    {
        return 0;
    }
    std::vector<unsigned int>::iterator findIt =
        std::find(m_npcLayers.begin(), m_npcLayers.end(), layer);
    if (findIt != m_npcLayers.end())
    {
        return static_cast<int>(layer);
    }
    return -1;
}

// ============================================================================
// TowerOfDespairMgr::GetBaseLayerIndex —— ORIG T 0x086440a0
//   （报告 TowerOfDespairMgr/GetBaseLayerIndex.md）：m_field358 缓存基础层。
// ============================================================================
int TowerOfDespairMgr::GetBaseLayerIndex()
{
    if (m_field358 == 0)
    {
        CDungeon* base = Get_CDungeonByLayer(1);
        if (base != 0)
        {
            m_field358 = base->get_index();
        }
    }
    return m_field358;
}

// ============================================================================
// TowerOfDespairMgr::SendMessageToCUser —— ORIG T 0x08644366
//   （报告 TowerOfDespairMgr/SendMessageToCUser.md）：仅 GM 可见文本通知包
//   （header(0,0xc)/byte0/short0/byte0/int len/str/finalize(true)→Send）。
//   CUser::Send 定义 blocked（ORIG T 0x086485ba 待批次）。
// ============================================================================
void TowerOfDespairMgr::SendMessageToCUser(CUser* user, char* msg)
{
    if (user->isGMUser())
    {
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(msg)));
        packet.put_str(msg, static_cast<int>(strlen(msg)));
        packet.finalize(true);
        user->Send(packet);
    }
}

// ============================================================================
// IsExistCharacter —— ORIG T 0x085b624e（报告 CParty/IsExistCharacter.md）
// ============================================================================
bool CParty::IsExistCharacter(int characNo)
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        if (characNo == m_member[local_10].m_pUser->getCurCharacNo())
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// FindUserByCharId —— ORIG T 0x085a2140（报告 CParty/FindUserByCharId.md）
//   返回命中槽的 CUser*（头文件签名 int；调用方按真值消费）。
// ============================================================================
int CParty::FindUserByCharId(unsigned int characNo)
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        if (static_cast<unsigned int>(
                m_member[local_10].m_pUser->get_charac_no(-1)) == characNo)
        {
            return reinterpret_cast<int>(m_member[local_10].m_pUser);
        }
    }
    return 0;
}

// ============================================================================
// BloodHandleDieUser —— ORIG T 0x085b8e5c（报告 CParty/BloodHandleDieUser.md）
// ============================================================================
void CParty::BloodHandleDieUser(CUser* user)
{
    int local_10 = 0;
    for (local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!checkValidUser(local_10))
            continue;
        if (get_user(local_10) == user)
        {
            if (m_liveFlag[local_10] != 1)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                return;
            }
            set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);
            break;
        }
    }

    if (check_allmember_die())
    {
        if (m_partyType != 1)
        {
            int key = gen_timer_key((TIMER_MESSAGE)0x22);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x22,
                                        10, key, (unsigned int)0x22);
        }
    }
}

// ============================================================================
// CheckClearQuestHellParty —— ORIG T 0x085a7a72（报告 CParty/CheckClearQuestHellParty.md）
//   out 清空后：对每个有效成员逐一核对 hell 任务清单（m_hellWorldMap+0x1c，
//   即 CWorldMap::m_field1c；无空检查，ORIG 如此）；存在未清任务 →
//   push_back 成员槽号并跳出该成员。
// ============================================================================
void CParty::CheckClearQuestHellParty(std::vector<int>& out)
{
    out.clear();
    int local_14 = 0;
    for (local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;

        int questId = 0;
        std::vector<int>::iterator it = m_hellWorldMap->m_field1c.begin();
        std::vector<int>::iterator endIt = m_hellWorldMap->m_field1c.end();
        while (it != endIt)
        {
            questId = *it;
            UserQuest* quest = m_member[local_14].m_pUser->getCurCharacQuestR();
            if (!(quest->isClearQuest(questId)))
            {
                out.push_back(local_14);
                break;
            }
            ++it;
        }
    }
}

// ============================================================================
// UseSealDoorItems —— ORIG T 0x085a8038（报告 CParty/UseSealDoorItems.md）
// ============================================================================
bool CParty::UseSealDoorItems(std::vector<int>& itemIds)
{
    bool result = true;
    Inven_Item tmpItem;   // ORIG 栈上构造后未使用，保持一致

    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;
        if (CheckHellDungeonFreepassItemHaveAndDel(m_member[local_14].m_pUser,
                                                   true))
            continue;

        std::vector<int>::const_iterator it = itemIds.begin();
        while (it != itemIds.end())
        {
            int itemId = *it;
            int minLevel = m_dungeon->get_min_level();
            if (minLevel <= 0x2c)
            {
                m_sealDoorItemsOk = 0;
                return false;
            }

            int needCount = getHellPartyEntranceItemNeedCount(minLevel);
            int slot = m_member[local_14].m_pUser->getCurCharacInvenR()
                           ->check_item_exist(itemId);
            if (slot != -1)
            {
                CInventory* invW = m_member[local_14].m_pUser->getCurCharacInvenW();
                if (!invW->delete_item((INVEN_TYPE)1, slot, needCount,
                                       (eItemDelReason)0x17, true))
                {
                    cMyTrace trace("bool CParty::UseSealDoorItems(IntVector&)",
                                   0x1d22, 5);
                    trace("CParty::UseSealDoorItems, delete_item failed , "
                          "User ch =%d , %d %d",
                          m_member[local_14].m_pUser->getCurCharacNo(), slot,
                          needCount);
                }
                else
                {
                    m_member[local_14].m_pUser->SendUpdateItem(
                        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
                }
            }
            else
            {
                m_sealDoorItemsOk = 0;
                return false;
            }
            ++it;
        }
    }

    m_sealDoorItemsOk = 1;
    return result;
}

// ============================================================================
// CheckHaveHellPartyPassItem —— ORIG T 0x085a7b8a（报告 CParty/CheckHaveHellPartyPassItem.md）
// ============================================================================
bool CParty::CheckHaveHellPartyPassItem(CDungeon const* dungeon,
                                        ENUM_DUNGEON_TYPE type)
{
    if (m_hellWorldMap == 0)
    {
        return true;
    }
    bool skip;
    if ((m_hellWorldMap->IsInHellDungeon()) && (type == 1))
    {
        skip = false;
    }
    else
    {
        skip = true;
    }
    if (skip)
    {
        return true;
    }

    std::vector<int> sealCondItems;
    if (!CheckDestroyConditionSealDoor(dungeon, sealCondItems,
                                       m_hellWorldMap->m_field34))
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_byte(0x1b);
        packet.put_byte(static_cast<int>(sealCondItems.size()));
        std::vector<int>::const_iterator it = sealCondItems.begin();
        while (it != sealCondItems.end())
        {
            packet.put_short(*it);
            ++it;
        }
        packet.finalize(true);
        send_to_party(packet);
        m_sealDoorFlag = 0;
    }
    else
    {
        m_sealDoorFlag = 1;
    }
    return m_sealDoorFlag;
}

// ============================================================================
// SendPartyMemberTagChracInfo —— ORIG T 0x085be05a（报告 CParty/SendPartyMemberTagChracInfo.md）
// ============================================================================
void CParty::SendPartyMemberTagChracInfo()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19e);

    int countIdx = packet.get_index();
    packet.put_short(0);

    int tagCount = 0;
    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        CUser* user = get_user(local_14);
        if (user != 0)
        {
            if (user->MakeTagCharacInfo(packet))
            {
                tagCount = tagCount + 1;
            }
        }
    }

    packet.put_short(countIdx, tagCount);
    packet.finalize(true);
    send_to_party(packet);
}

// ============================================================================
// GetDungeonIndex_isTOD —— ORIG T 0x085bacc0（报告 CParty/GetDungeonIndex_isTOD.md）
// ============================================================================
int CParty::GetDungeonIndex_isTOD(int& dungeonIdx, int& apcIdx)
{
    TowerOfDespairMgr* tod =
        reinterpret_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    if (tod == 0)
    {
        cMyTraceNoop trace;
        trace("Error : DungeonIndex : %d", dungeonIdx);
        return 0x13;
    }

    if (tod->GetBaseLayerIndex() == dungeonIdx)
    {
        CUser* manager = m_manager;   // ORIG this+0x74
        if (manager == 0)
        {
            cMyTrace trace("int CParty::GetDungeonIndex_isTOD(int&, int&)",
                           0x41bb, 5);
            trace("TOD no Dungeon Manager!");
            return 0x13;
        }

        TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
            manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
        TOD_Layer todayLayer = state->getTodayEnterLayer();

        dungeonIdx = static_cast<int>(tod->Get_DungeonIndexByLayer(
            todayLayer.GetLayer()));

        CDungeon* target = G_CDataManager()->find_dungeon(dungeonIdx);
        int err = tod->CheckEnterable(manager, target);
        if (err != 0)
        {
            return err;
        }

        apcIdx = tod->Get_APCIndexByLayer(todayLayer.GetLayer());

        char msg[256];
        sprintf(msg, "Enter Layer : %d",
                static_cast<int>(todayLayer.GetLayer()));
        tod->SendMessageToCUser(manager, msg);
    }
    return 0;
}

// ============================================================================
// SetProperDungeonEvent —— ORIG T 0x085bb62a（报告 CParty/SetProperDungeonEvent.md）
// ============================================================================
void CParty::SetProperDungeonEvent(CUser* user, int characInfoAsInt)
{
    CUserCharacInfo* characInfo =
        reinterpret_cast<CUserCharacInfo*>(characInfoAsInt);
    characInfo->ClearProperDungeonUser();

    CDungeon* dungeon =
        reinterpret_cast<CUserCurDungeonView*>(user)->m_dungeon;
    if (G_CDataManager()->m_conditionDungeon.isEventDungeon(
            dungeon->get_index()))
    {
        if (G_CDataManager()->m_conditionDungeon.isApplyLevel(
                characInfo->get_charac_level()))
        {
            int standardLevel = dungeon->get_standard_level();
            int minLevel = dungeon->get_min_level();
            int zero = 0;
            int lower = std::max(minLevel, zero);
            int upperBound = standardLevel + 1;
            int cap = 0x46;
            int upper = std::min(upperBound, cap);
            int level = characInfo->get_charac_level();
            if (lower <= level && level <= upper)
            {
                characInfo->SetProperDungeonUser();
            }
        }
    }
}

// ============================================================================
// SetProperLevelDungeon —— ORIG T 0x085bb746（报告 CParty/SetProperLevelDungeon.md）
// ============================================================================
void CParty::SetProperLevelDungeon(CUser* user)
{
    user->ClearProperLevelDungeonUser();

    if (G_CDataManager()->m_conditionDungeon.isEventDungeon(
            m_dungeon->get_index()))
    {
        int standardLevel = m_dungeon->get_standard_level();
        int minLevel = m_dungeon->get_min_level();
        int zero = 0;
        int lower = std::max(minLevel, zero);
        int upperBound = standardLevel + 1;
        int cap = 0x46;
        int upper = std::min(upperBound, cap);
        int level = user->get_charac_level();
        if (lower <= level && level <= upper)
        {
            user->SetProperLevelDungeonUser();
        }
    }
}
