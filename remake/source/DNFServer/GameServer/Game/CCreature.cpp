// df_game_r 还原 —— user_creature::CCreature / CCreatureMgr / CCreatureItemFactory
// 本 TU 定义 user_creature 域宠物品类的成员实现。类定义（权威）见 CInventory.h；
// 原实现从 GameStubs.cpp 迁移至此（用户指示：类声明/实现一律以 GameStubs 外的
// 权威头为准，实现迁移到对应 class 的 TU）。
// 注：各方法当前为语义占位实现（stub），后续按
//   docs/class_func_reports/user_creature__CCreature.md、
//   user_creature__CCreatureMgr.md 逐函数精修至 identical。
#include <string.h>

#include <map>

#include "CDataManager.h"
#include "CInventory.h"
#include "CEquipItem.h"
#include "CStackableItem.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"

namespace user_creature
{

float CStomach::GetStomachValue(int state, bool growFlag)
{
    if (!growFlag)
    {
        return 100.0f;
    }

    int curTime = GlobalData::s_systemTime_.getCurSec();
    if (state == 1)
    {
        m_value += (float)(curTime - m_lastTime);
        if (m_value < 0.0f)
        {
            m_value = 0.0f;
        }
    }
    else if (state > 0 && state < 4)
    {
        m_value += (float)(curTime - m_lastTime);
        if (m_value > 100.0f)
        {
            m_value = 100.0f;
        }
    }
    m_lastTime = curTime;
    return m_value;
}

void CCreatureItem::SetItemId(int itemId)
{
    m_itemId = itemId;
}

void CCreatureItem::SetSlotNo(int slot)
{
    m_slotNo = slot;
}

int CCreatureItem::GetItemId() const
{
    return m_itemId;
}

int CCreatureItem::GetUid() const
{
    return m_itemId;
}

// ============================================================================

CCreature::CCreature()
{
    // ORIG 0x0833705e：CCreatureItem 基类（自动）→ vtable → CStomach/COverSkillTime
    // 构造 → Reset()。CStomach/COverSkillTime 为占位，Reset 统一清零。
    Reset();
}

void CCreature::SetItemId(int itemId)
{
    // ORIG _ZN13user_creature9CCreature9SetItemIdEi（区别于基类 CCreatureItem::SetItemId：
    // 同时刷新 m_creatureId）
    m_itemId = itemId;
    int creatureId = GetCreatureId(itemId);
    m_creatureId = creatureId;
    // GetInstanceCreatureScriptMgr()->RegisterCreatureIdAndItemId(creatureId, itemId);
}
// CCreature
// ============================================================================

int CCreature::GetState() const
{
    return m_state;
}

void CCreature::SetState(CUser* user, int state)
{
    if (m_state != state)
    {
        if (state == 2)
        {
            // CPacketRespondent packet(user, 0, 0);
            // packet.MakeNotipacketRevivalCreature();
            // packet.SendAll();
        }
    }
    m_state = state;
}

// （GetItemId / GetUid 为基类 CCreatureItem 的方法，CCreature 继承之；SetItemId 为
//   CCreature 自身方法，见上方实现）

int CCreature::GetCreatureId(int itemId)
{
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0)
        return -1;
    if (item->is_stackable())
        return -1;
    int creatureId = ((CEquipItem*)item)->get_creature_species();
    m_creatureId = creatureId;
    return creatureId;
}

bool CCreature::IsGrowCreature()
{
    return (m_growFlag ^ 1) != 0;
}

int CCreature::GetCreatureId()
{
    return m_creatureId;
}

// （GetUid 为基类 CCreatureItem 方法）

void* CCreature::GetOverSkillTime()
{
    return m_overSkillTime;
}

int CCreature::GetStomach()
{
    return (int)m_stomach.GetStomachValue(m_state, m_growFlag != 0);
}

int CCreature::GetLearnOverSkillLevel()
{
    if (m_creatureScript == 0)
        return -1;
    void** vtbl = *(void***)m_creatureScript;
    return ((int (*)(void*))vtbl[5])(m_creatureScript);  // GetLearnOverSkillLevel
}

int CCreature::GetOverSkillRecoveryTime()
{
    if (m_creatureScript == 0)
        return 0;
    void** vtbl = *(void***)m_creatureScript;
    return ((int (*)(void*))vtbl[6])(m_creatureScript);  // GetOverSkillRecoveryTime
}

int CCreature::GetSkillRecoveryTime()
{
    if (m_creatureScript == 0)
        return 0;
    void** vtbl = *(void***)m_creatureScript;
    return ((int (*)(void*))vtbl[7])(m_creatureScript);  // GetSkillRecoveryTime
}

int CCreature::UseFeed(CUser* user, int& stomachValue)
{
    if (!IsGrowCreature())
        return 0;
    if (IsDieCreature())
    {
        // CPacketRespondent packet(user, 0, 0);
        // packet.MakeNotipacketRevivalCreature();
        // packet.SendAll();
    }
    // CStomach::IncrementValue(&m_pad34, 30.0f);
    stomachValue = 30;  // simplified
    return 1;
}

bool CCreature::IsDieCreature()
{
    float stomachValue = m_stomach.GetStomachValue(m_state, m_growFlag != 0);
    return stomachValue <= 1.0f;
}
bool CCreature::isMaxLevel()
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    int maxLevel = ((int (*)(void*))vtbl[0])(m_creatureScript);  // GetMaxLevel
    return m_level >= maxLevel;
}

int CCreature::GainExp(int exp, CUser* user)
{
    if (m_creatureScript == 0)
        return 0;
    void** vtbl = *(void***)m_creatureScript;
    int evoLevel = ((int (*)(void*))vtbl[1])(m_creatureScript);  // GetTypeSelectEvolutionLevel
    if (evoLevel != -1 && m_level >= evoLevel)
        return 0;
    int maxLevel = ((int (*)(void*))vtbl[0])(m_creatureScript);  // GetMaxLevel
    if (m_level >= maxLevel)
        return 0;
    if (m_growFlag == 1)
        return GainExpCreature(exp, user);
    return GainItemExpCreature(exp, user);
}

void CCreature::Reset()
{
    m_itemId = 1;
    m_field0c = 0;
    m_slotNo = 0;               // +0x10（ORIG Reset 置 0；CCreatureItem::m_slotNo）
    m_state = 2;
    m_field40 = 0;
    m_creatureScript = 0;
    m_creatureId = 0;
    m_level = 1;
    m_field30 = 0;
    m_field14 = 0;
    m_growFlag = 1;
    m_field4d = 5;
    memset(&m_level, 0, 0xd);  // clears +0x1c to +0x28
    m_level = 1;               // reset again after memset
    // CStomach::SetStomachValue((CStomach*)(this+0x34), 0.0)
    memset(&m_stomach, 0, sizeof(m_stomach));
}

int CCreature::Equip(CUser* user)
{
    if (user->is_fighting())
    {
        SetState(user, 1);
    }
    else
    {
        SetState(user, 3);
    }
    return 1;
}
int CCreature::Dismantle(CUser* user)
{
    SetState(user, 2);
    return 1;
}

int CCreature::Die(CUser* user)
{
    // CPacketRespondent packet(user, 0, 0);
    return 1;
}

bool CCreature::CheckDiedCreature(CUser* user)
{
    if (IsDieCreature())
    {
        Die(user);
        return true;
    }
    return false;
}

int CCreature::GainExpCreature(int exp, CUser* user)
{
    if (!IsGrowCreature())
        return 0;
    int stomach = GetStomach();
    if (stomach <= 0)
        return 0;
    bool levelUp = false;
    CalculateExp(exp, levelUp, user);
    int maxLevel = 0;
    if (m_creatureScript)
    {
        void** vtbl = *(void***)m_creatureScript;
        maxLevel = ((int (*)(void*))vtbl[0])(m_creatureScript);
    }
    bool sendPacket = (m_level < maxLevel) || levelUp;
    if (sendPacket)
    {
        // CPacketRespondent packet(user, 0, 0);
        // packet.MakeNotipacketGainExpCreature(m_field30, m_level, IsGrowCreature());
    }
    bool evolute = false;
    if (levelUp)
        evolute = IsAbleEvolute(user);
    if (evolute)
    {
        if (!Evolute(user))
            return 0;
    }
    m_field40 = 1;
    return 1;
}

bool CCreature::Evolute(CUser* user)
{
    // ORIG 0x083381ea。语义：按进化目标 creature script 更新自身 itemId 与背包槽，
    // 并写库/发包。DB 写库（CDBMsgSender::EvoluteCreature）与客户端包
    // （CPacketRespondent）子系统尚未还原，此处保留核心字段更新与流程，DB/发包
    // 步骤以追踪桩表示（后续按 ORIG 补齐）。
    if (m_creatureScript == 0)
        return 0;
    void** vtbl = *(void***)m_creatureScript;
    int evoItemId = ((int (*)(void*))vtbl[8])(m_creatureScript);  // GetEvolutionCreatureId
    if (evoItemId == 0)
        return 0;
    // GetInstanceCreatureScriptMgr()->FindCreatureScript(evoItemId) 获得进化目标脚本，
    // 目标 itemId 由其 GetItemId() 给出（脚本域未还原，此处以 evoItemId 代）：
    int targetItemId = evoItemId;
    SetItemId(targetItemId);
    CInventory* inven = user->getCurCharacInvenW();
    if (inven == 0)
        return 0;
    Inven_Item* slot = inven->GetInvenRef(0, 0x16);
    if (slot == 0)
        return 0;
    slot->m_addInfo = GetItemId();  // +0x02（ORIG：*(undefined4*)(slot+2) = GetItemId()）
    // DB message and notification sending (underlying methods not yet implemented)
    //       MakeNotipacketEvoluteCreature + SendAccordingToPlace；
    //       CUser::SendUpdateItemList(1, 3, 0x16)
    return 1;
}

int CCreature::GainItemExpCreature(int exp, CUser* user)
{
    if (m_growFlag != 0)
        return 0;
    if (m_creatureScript)
    {
        void** vtbl = *(void***)m_creatureScript;
        if (((bool (*)(void*, int))vtbl[2])(m_creatureScript, exp))  // validItem_AType
        {
            int v = m_field4d + 1;
            if (v > 10) v = 10;
            m_field4d = (char)v;
        }
        if (((bool (*)(void*, int))vtbl[3])(m_creatureScript, exp))  // validItem_BType
        {
            int v = m_field4d - 1;
            if (v < 0) v = 0;
            m_field4d = (char)v;
        }
    }
    int expVal = (m_field30 >> 8) + 1;
    m_field30 = (expVal << 8) | (m_field4d & 0xff);
    int newLevel = GetExpLevel(expVal);
    bool levelUp = false;
    if (m_level < newLevel)
        levelUp = true;
    m_level = newLevel;
    int maxLevel = 0;
    if (m_creatureScript)
    {
        void** vtbl = *(void***)m_creatureScript;
        maxLevel = ((int (*)(void*))vtbl[0])(m_creatureScript);
    }
    bool sendPacket = (m_level < maxLevel) || levelUp;
    if (sendPacket)
    {
        // CPacketRespondent packet(user, 0, 0);
        // packet.MakeNotipacketGainExpCreature(m_field30, m_level, IsGrowCreature());
    }
    return sendPacket ? 1 : 0;
}

int CCreature::CalculateExp(int exp, bool& levelUp, CUser* user)
{
    float rate = GetAddExp(0x17, user) + GetAddExp(0x18, user) + GetAddExp(0x19, user);
    int addExp = (int)(exp * rate / 100.0f) + exp;
    m_field30 += addExp;
    int newLevel = GetExpLevel(m_field30);
    levelUp = (m_level < newLevel);
    m_level = newLevel;
    return 0;
}

bool CCreature::IsAbleEvolute(CUser* user) const
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    int evolutionLevel = ((int (*)(void*))vtbl[3])(m_creatureScript);  // GetEvolutionLevel
    if (evolutionLevel == 0)
        return false;
    if (evolutionLevel > m_level)
        return false;
    bool hasQuest = ((bool (*)(void*))vtbl[4])(m_creatureScript);  // IsExistEvolutionQuest
    if (hasQuest)
    {
        if (user != 0)
        {
            // PacketGuard guard;
            // user->getCurCharacQuestR()->get_quest_info(&guard);
            // user->Send(guard);
        }
        return false;
    }
    return true;
}

int CCreature::GetExpLevel(int exp) const
{
    int level;
    if (m_growFlag == 1)
    {
        level = G_CDataManager()->get_creature_exp_level(exp);
    }
    else
    {
        int growCount = 0;
        if (m_creatureScript)
        {
            void** vtbl = *(void***)m_creatureScript;
            growCount = ((int (*)(void*))vtbl[11])(m_creatureScript);  // GetGrowItemCountforLevelUp
        }
        if (growCount == 0)
            growCount = 0x19;
        level = exp / growCount + 1;
    }
    if (m_creatureScript)
    {
        void** vtbl = *(void***)m_creatureScript;
        int maxLevel = ((int (*)(void*))vtbl[0])(m_creatureScript);  // GetMaxLevel
        if (maxLevel < level)
            level = maxLevel;
    }
    return level;
}

bool CCreature::vaildItemExp(int itemType)
{
    if (m_state == 2)
        return false;
    if (m_growFlag == 0)
    {
        if (m_creatureScript == 0)
            return false;
        void** vtbl = *(void***)m_creatureScript;
        return ((bool (*)(void*, int))vtbl[0])(m_creatureScript, itemType);
    }
    return false;
}

float CCreature::GetAddExp(int type, CUser* user)
{
    float exp = 0.0f;
    if (IsAvailableArtifact(type))
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (inven != 0)
        {
            CCreatureMgr* mgr = inven->GetCreatureMgrW();
            CArtifact* artifact = mgr->GetArtifact(type);
            if (artifact)
            {
                int minLevel = artifact->GetCreatureMinimumLevel();
                if (minLevel == 0 || minLevel <= m_level)
                    exp = artifact->GetExperienceAmount();
            }
        }
    }
    return exp;
}

bool CCreature::IsEvolutionCreature(int creatureId)
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    int evoId = ((int (*)(void*))vtbl[8])(m_creatureScript);
    return evoId == creatureId;
}

bool CCreature::IsEvolutionCreatureMulty(int creatureId)
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    return ((bool (*)(void*, int))vtbl[9])(m_creatureScript, creatureId);
}

bool CCreature::IsEventEvolutionCreature(int creatureId)
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    return ((bool (*)(void*, int))vtbl[10])(m_creatureScript, creatureId);
}

bool CCreature::IsAvailableArtifact(int type)
{
    if (m_creatureScript == 0)
        return false;
    void** vtbl = *(void***)m_creatureScript;
    return ((bool (*)(void*, int))vtbl[0])(m_creatureScript, type);
}

// （GetItemId 为基类 CCreatureItem::GetItemId 方法，CCreature 继承之）

int CCreature::GetLevel() const
{
    return m_level;
}

const char* CCreature::GetName() const
{
    return m_name;
}

// ============================================================================
// CCreatureItemFactory
// ============================================================================

CCreatureItemFactory* GetInstanceCreatureItemFactory()
{
    static CCreatureItemFactory factory;
    return &factory;
}

bool CCreatureItemFactory::FreeCreatureItem(CCreatureItem*)
{
    return true;
}

// ============================================================================
// CCreatureMgr
// ============================================================================

void CCreatureMgr::Reset()
{
    m_equippedCreature = 0;
    m_user = 0;
    m_artifacts[3].m_pItem = (CEquipItem*)-1;
    if (!m_field30.empty())
        m_field30.clear();
    if (!m_creatureItems.empty())
    {
        CCreatureItemFactory* factory = GetInstanceCreatureItemFactory();
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); ++it)
        {
            CCreatureItem* item = it->second;
            if (item != 0)
            {
                if (!factory->FreeCreatureItem(item))
                {
                    LogManager::logFormat(1, "CCreatureMgr.cpp", "CCreatureMgr::Reset", 0xb50,
                                          "FreeCreatureItem failed");
                }
            }
        }
        m_creatureItems.clear();
    }
}

void CCreatureMgr::SetUser(CUser* user)
{
    m_user = user;
}

int CCreatureMgr::UseItem(Inven_Item* item, int slot)
{
    int itemType = item->m_addInfo;
    if (itemType == 0x18)
    {
        if (!UseFeed())
            return 0x85;
    }
    if (m_equippedCreature != 0 && m_equippedCreature->vaildItemExp(itemType))
    {
        if (!GainExp(itemType))
            return 0x11;
    }
    return 0;
}

bool CCreatureMgr::UseFeed()
{
    if (m_equippedCreature == 0)
        return false;
    int stomachValue = 0;
    if (!m_equippedCreature->UseFeed(m_user, stomachValue))
        return false;
    return true;
}

bool CCreatureMgr::GainExp(int exp)
{
    if (m_equippedCreature == 0)
        return false;
    if (!m_equippedCreature->GainExp(exp, m_user))
        return false;
    if (m_equippedCreature->IsGrowCreature())
    {
        m_field48 = m_equippedCreature->isMaxLevel() ? 1 : 0;
    }
    return true;
}

bool CCreatureMgr::IsEquippedCreature() const
{
    return m_equippedCreature != 0;
}

bool CCreatureMgr::IsGrowCreature_Equipped_Creature() const
{
    if (m_equippedCreature == 0)
        return false;
    return m_equippedCreature->IsGrowCreature();
}

int CCreatureMgr::GetCreatureItemId() const
{
    if (m_equippedCreature == 0)
        return 0;
    return m_equippedCreature->GetItemId();
}

int CCreatureMgr::GetEquipedCreatureLevel() const
{
    if (m_equippedCreature == 0)
        return 0;
    return m_equippedCreature->GetLevel();
}

int CCreatureMgr::GetCreatureIndex() const
{
    if (m_equippedCreature == 0)
        return 0;
    return m_equippedCreature->GetCreatureId();
}

const char* CCreatureMgr::GetCreatureName() const
{
    if (m_equippedCreature == 0)
        return "";
    return m_equippedCreature->GetName();
}

unsigned char CCreatureMgr::isGrowCreatureMaxLevel() const
{
    return m_field48;
}

bool CCreatureMgr::checkMaxCreatureLevel(int itemId, bool isGrow) const
{
    for (std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); ++it)
    {
        CCreature* item = (CCreature*)it->second;
        if (item == 0)
            continue;
        if (item->GetItemId() == itemId && item->IsGrowCreature() == isGrow)
            return item->isMaxLevel();
    }
    return false;
}

bool CCreatureMgr::InsertCreatureItem(Inven_Item* item, int a, int b, int c, int d)
{
    if (!IsCreatureEquipmentScope(a))
        return true;
    int itemType = item->m_field1;
    if (itemType != 5)
        return true;
    int expireTime = 0;
    if (b != 0 || c != 0)
    {
        if (c != 0)
            expireTime = c;
        else
            expireTime = b * 0x15180;  // b * 86400 (seconds per day)
    }
    setTempExpireTime(expireTime);
    return true;
}

bool CCreatureMgr::IsCreatureEquipmentScope(int slot) const
{
    if (slot >= 0 && slot <= 0x8b)
        return true;
    return slot == 0x16;
}


void CCreatureMgr::setTempExpireTime(int time)
{
}

CArtifact* CCreatureMgr::GetArtifact(int type)
{
    int idx = type - 0x17;
    if (idx < 0 || idx > 3)
        return 0;
    return &m_artifacts[idx];
}

// 注：CCreature::Reset / CCreatureMgr 域类（CStomach/CCreatureItem/CCreature/
// CCreatureItemFactory）的类定义尚未在头建模，本 TU 暂不入 CMake（等待类定义补齐）。
// SendCreatureItemList 已迁至 CCreatureMgr.cpp。

}  // namespace user_creature