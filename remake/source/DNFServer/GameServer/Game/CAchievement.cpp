// df_game_r 成就 CAchievement 实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CAchievement.md（Ghidra C + 汇编）+ ORIG
// 反汇编交叉核对；按用户规则优先具名成员访问，不做裸偏移。
#include <cstdio>
#include <cstring>
#include <ctime>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CAchievement.h"

#include "CDataManager.h"
#include "CItem.h"
#include "CMissionList_Charac.h"
#include "CStreamGuard.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "Stream.h"
#include "StreamPool.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号
// ============================================================================

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义 _ZN11PacketGuardC1/D1Ev）
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

class CExpandEquipslot;
class CInventory;
class UserQuest;

// 本 TU 的 CUser / CUserCharacInfo / CInventory / UserQuest 局部视图
// （真实定义在 CUser.h / CInventory.h，含与 CTitleBook 冲突的局部类；
//  按 CTitleBook.cpp 同款模式避免混入）。
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    unsigned int GetUID() const;
    int get_charac_no(int type);
    ENUM_SERVER_GROUP GetServerGroup() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    UserQuest* getCurCharacQuestR() const;
    void Send(PacketGuard& packet);
    void SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
};

class UserQuest
{
public:
    bool isClearQuest(int questIdx) const;
};

class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
};

// ORIG 调用点怪癖：_forceClear 以 4 参自由函数形态调用 GetInvenSlot
// （压栈 out,this,invenType,slot），符号指向成员 _ZNK10CInventory12GetInvenSlotEii；
// 链接桩由主 agent 提供（同 CInventory.h 声明）。
int GetInvenSlot(Inven_Item* out, CInventory* self, int invenType, int slot);

namespace WongWork
{
class CMailBoxHelper
{
public:
    // ORIG mangled：_ZN8WongWork14CMailBoxHelper22ReqDBSendNewSystemMail
    //   EPKcRK10Inven_ItemjjS2_ij17ENUM_SERVER_GROUPbb
    static void ReqDBSendNewSystemMail(const char* pSender,
                                       const Inven_Item& item,
                                       unsigned int nA, unsigned int nCharacNo,
                                       const char* pTitle, int nTitleLen,
                                       unsigned int nB,
                                       ENUM_SERVER_GROUP nServerGroup,
                                       bool bA, bool bB);
};
}

// cUserHistoryLog::AchievementComplete（CUser.h 权威 cUserHistoryLog 声明，
// CAchievement.cpp 使用本地最小视图；符号 _ZN15cUserHistoryLog19AchievementCompleteEi
// 由 cUserHistoryLog.cpp 提供，ORIG 0x08685762）
class cUserHistoryLog
{
public:
    void AchievementComplete(int idx);
};


// ---- CDataManager 称号表视图（+0xa788，与 CTitleBook.cpp 相同布局） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                        // +0x00
    std::vector<stTitleSection> m_sections;                  // +0x0c（0xa794）
    std::map<int, std::vector<stTitleElement> > m_elements;  // +0x18（0xa7a0）
    char m_pad34[0x8];                                       // +0x34
    std::vector<stAdjust> m_adjustList;                      // +0x3c（0xa7c4）
};

// CDataManager 称号表视图（+0xa788 起，与 CTitleBook.cpp 相同布局）。
// 宏展开为直接偏移访问：ORIG 汇编为 lea 0xa794/0xa7a0/0xa7c4(%eax)，
// 若用 static 函数在 O0 下会生成额外 call，导致助记符序列不一致。
#define GetTitleSections(mgr) \
    (*(std::vector<stTitleSection>*)((char*)(mgr) + 0xa794))
#define GetTitleElements(mgr) \
    (*(std::map<int, std::vector<stTitleElement> >*)((char*)(mgr) + 0xa7a0))
#define GetAdjustList(mgr) \
    (*(std::vector<stAdjust>*)((char*)(mgr) + 0xa7c4))

// Quest::m_postalReward（+0x9c）的逐字段视图：ORIG 成就邮件按
// {标题, 内容, 道具 idx, addInfo} 读取（Quest.h 的 PostalReward 未具名 +0x08）。
struct PostalRewardView
{
    std::string m_title;     // +0x00
    std::string m_contents;  // +0x04
    int m_itemIdx;           // +0x08
    int m_addInfo;           // +0x0c
};

// ============================================================================
// stAchievement / stAchievement_DBData
// ============================================================================

stAchievement::stAchievement()
{
    m_id = 0;
    m_trigger0 = 0;
    m_trigger1 = 0;
    m_trigger2 = 0;
    m_state = 0;
}

bool stAchievement::isComplete()
{
    return (m_trigger0 == 0) && (m_trigger1 == 0) && (m_trigger2 == 0);
}

stAchievement_DBData::stAchievement_DBData()
{
    // ORIG 0x828d8e5：m_entries 数组由编译器自动逐个构造（esi 递减 512 次
    // call stAchievementC1），无需显式赋值循环
    m_count = 0;
    m_saveTime = 0;
}

// ============================================================================
// CAchievement
// ============================================================================

CAchievement::CAchievement() : m_user(0)
{
    // ORIG 0x828b633：m_user 置零在 map 构造之前（初始化列表，
    // 按声明顺序先于 m_achievements）；m_field24/25 在函数体
    m_field24 = 0;
    m_field25 = 0;
}

CAchievement::~CAchievement()
{
}

// ---- 触发 / 校验 / 完成 ----

void CAchievement::setTrigger(unsigned int id, unsigned short t0,
                              unsigned short t1, unsigned short t2)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if ((quest->m_type != 7) ||
        (m_user->get_charac_level() < quest->m_levelMin))
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if (isClearedAchievement(id))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    // ORIG 0x828b781：_verifyTrigger 返回值 xor 1 后 je +0x12c，
    // 即 verify==1 才走 _setTrigger 路径；verify==0 → _sendSetTrigger(id,2)。
    if (!_verifyTrigger(id, t0, t1, t2))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    _setTrigger(id, t0, t1, t2);
    _sendSetTrigger(id, 0);
    _checkComplete(id);
}

bool CAchievement::_verifyTrigger(unsigned int id, unsigned short t0,
                                  unsigned short t1, unsigned short t2)
{
    // ORIG 0x828b81e：cmp 顺序 198b→19c4→1983 且 198b/1983 共享 level 检查块
    // （编译器合并）；本实现语义等价，形态差异为编译器伪影（无法用普通 C++ 复现）
    if ((id == 0x198b) || (id == 0x1983))
    {
        if (m_user->get_charac_level() > 0x45)
        {
            return 1;
        }
        return 0;
    }
    if (id == 0x19c4)
    {
        time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm local;
        localtime_r(&now, &local);
        if (local.tm_hour < 0)
        {
            return 1;
        }
        if (local.tm_hour > 0x14)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}

void CAchievement::_setTrigger(unsigned int id, unsigned short t0,
                               unsigned short t1, unsigned short t2)
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828b8fd：已找到分支为 fall-through（je 跳到未找到块），故用 != end 先行
    if (it != m_achievements.end())
    {
        // ORIG 0x828b916：setae（cur>=t0）为 fall-through 的 sub 分支，zero 分支在跳转目标
        if (it->second.m_trigger0 >= t0)
        {
            it->second.m_trigger0 -= t0;
        }
        else
        {
            it->second.m_trigger0 = 0;
        }
        if (it->second.m_trigger1 >= t1)
        {
            it->second.m_trigger1 -= t1;
        }
        else
        {
            it->second.m_trigger1 = 0;
        }
        if (it->second.m_trigger2 >= t2)
        {
            it->second.m_trigger2 -= t2;
        }
        else
        {
            it->second.m_trigger2 = 0;
        }
    }
    else
    {
        stAchievement ach;
        ach.m_id = id;
        Quest* quest = G_CDataManager()->find_quest((int)id);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828b9dd：get_init 直接写 stAchievement 临时对象字段
        if (!quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                 ach.m_trigger2))
        {
            _sendSetTrigger(id, 1);
            return;
        }
        // ORIG 0x828ba5a：与已找到分支相同，setae 编码（sub 分支 fall-through）
        if (ach.m_trigger0 >= t0)
        {
            ach.m_trigger0 -= t0;
        }
        else
        {
            ach.m_trigger0 = 0;
        }
        if (ach.m_trigger1 >= t1)
        {
            ach.m_trigger1 -= t1;
        }
        else
        {
            ach.m_trigger1 = 0;
        }
        if (ach.m_trigger2 >= t2)
        {
            ach.m_trigger2 -= t2;
        }
        else
        {
            ach.m_trigger2 = 0;
        }
        m_achievements.insert(std::make_pair(id, ach));
    }
    alter();
}

bool CAchievement::isClearedAchievement(unsigned int id)
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828cc37：found 分支为 fall-through（je 跳到 return 0）
    if (it != m_achievements.end())
    {
        return it->second.isComplete();
    }
    return 0;
}

void CAchievement::_checkComplete(unsigned int id)
{
    if (!isClearedAchievement(id))
    {
        return;
    }
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        return;
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    int slot = 0;
    int titleIdx = quest->get_title_reward();
    if (titleIdx != 0)
    {
        CTitleBook::putTitleOnly(m_user, titleIdx, category, slot);
    }
    _sendEachAchieveReward(quest);
    _sendCompleteReward(quest, category, slot);
    _sendComplete(id, titleIdx, category, slot);
    ((cUserHistoryLog*)((char*)m_user + 0x79700))->AchievementComplete((int)id);
}

void CAchievement::_forceClear(unsigned int id)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if ((quest == 0) || (quest->m_type != 7))
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    // ORIG 0x828ccbb：isClearedAchievement 结果 xor 1 后 je 到已清除分支，
    // 未清除分支（重新初始化 trigger+发称号）为 fall-through，故源码用 !cleared 先行。
    if (!isClearedAchievement(id))
    {
        // ORIG 0x828cccc：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(id, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        CTitleBook::putTitleOnly(m_user, titleIdx);
    }
    else
    {
        ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
        int slot = 0;
        m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
        CTitleBook::_getFitSlot(titleIdx, category, slot);
        CTitleBook* book = (CTitleBook*)m_user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)14);
        Inven_Item titleSlot = book->getTitleSlotR(category, slot);
        if (titleSlot.m_addInfo == 0)
        {
            // ORIG 0x828cdcc 调 _ZNK15CUserCharacInfo18getCurCharacInvenREv（只读引用）
            const CInventory* inven = m_user->getCurCharacInvenR();
            Inven_Item invenItem;
            GetInvenSlot(&invenItem, (CInventory*)inven, 0, 0xb);
            if ((invenItem.m_addInfo == 0) ||
                (invenItem.m_addInfo != titleIdx))
            {
                CTitleBook::putTitleOnly(m_user, titleIdx);
            }
        }
    }
    alter();
}

void CAchievement::_forceReset(unsigned int id)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if ((quest == 0) || (quest->m_type != 7))
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    if (isClearedAchievement(id))
    {
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(id);
        if (it != m_achievements.end())
        {
            // ORIG 0x828ceff：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
            stAchievement ach;
            if (!quest->get_init_achievement_trigger(ach.m_trigger0,
                                                     ach.m_trigger1,
                                                     ach.m_trigger2))
            {
                return;
            }
            it->second.m_trigger0 = ach.m_trigger0;
            it->second.m_trigger1 = ach.m_trigger1;
            it->second.m_trigger2 = ach.m_trigger2;
        }
        alter();
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
    int slot = 0;
    m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
    CTitleBook::_getFitSlot(titleIdx, category, slot);
    CTitleBook* book = (CTitleBook*)m_user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)14);
    Inven_Item* titleSlot = book->getTitleSlot(category, slot);
    if (titleSlot == 0)
    {
        return;
    }
    // ORIG 0x828cff8：addInfo!=0 分支为 fall-through（reset），addInfo==0 分支 je 跳库存检查
    if (titleSlot->m_addInfo != 0)
    {
        titleSlot->reset();
    }
    else
    {
        CInventory* inven = (CInventory*)m_user->getCurCharacInvenW();
        Inven_Item* invenItem = inven->GetInvenRef(0, 0xb);
        if ((invenItem != 0) && (invenItem->m_addInfo == titleIdx))
        {
            invenItem->reset();
            m_user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                   0xb);
        }
    }
}

stAchievement CAchievement::_getTrigger(unsigned int id)
{
    stAchievement out;
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828bb72：found 分支 fall-through 复制 it->second 到 sret，
    // not-found 分支复制构造默认 out（双返回，不合并单出口）
    if (it != m_achievements.end())
    {
        return it->second;
    }
    return out;
}

void CAchievement::_verifyAchievement(unsigned int self)
{
    CAchievement* ach = (CAchievement*)self;

    // ORIG 0x828d09e：不缓存 elements 引用，begin 一次、end 每次循环重新取
    for (std::map<int, std::vector<stTitleElement> >::iterator it =
             GetTitleElements(G_CDataManager()).begin();
         it != GetTitleElements(G_CDataManager()).end(); ++it)
    {
        const std::vector<stTitleElement>& list = it->second;
        for (unsigned int i = 0; i < list.size(); ++i)
        {
            unsigned int achievementIdx =
                (unsigned int)list[i].m_achievementIdx;
            if (achievementIdx == 0xffffffff)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest((int)achievementIdx);
            if (quest == 0)
            {
                continue;
            }
            bool special =
                (quest->m_type == 7) && (quest->m_triggerType == 0xc) &&
                (!quest->m_triggerData.empty());
            if (special)
            {
                continue;
            }
            unsigned int clearQuestIdx = (unsigned int)quest->m_triggerData[0];
            if (ach->m_user->getCurCharacQuestR()->isClearQuest(
                    (int)clearQuestIdx))
            {
                ach->_forceClear(achievementIdx);
            }
        }
    }

    int level = ach->m_user->get_charac_level();
    if (level <= 0x45)
    {
        const unsigned int resetIds[2] = { 0x1983, 0x198b };
        for (int i = 0; i <= 1; ++i)
        {
            ach->_forceReset(resetIds[i]);
        }
    }
    else if (level > 0x45)
    {
        ach->_forceClear(0x1983);
    }

    CMissionList_Charac* missionList = (CMissionList_Charac*)
        ach->m_user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (missionList->isClearMission(0x83) ||
        missionList->isClearMission(0x87))
    {
        ach->_forceClear(0x19c0);
    }
}

void CAchievement::adjust()
{
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    int slot = 0;
    // ORIG 0x828c97d：questIdx/achievementIdx 循环外预初始化（movl $0 序列）
    unsigned int questIdx = 0;
    unsigned int achievementIdx = 0;
    // ORIG 0x828c97d：不缓存 adjustList 局部引用（每次循环重新取），
    // isClearQuest 通过才进入（嵌套 if，非 continue 前置）
    for (unsigned int i = 0; i < GetAdjustList(G_CDataManager()).size(); ++i)
    {
        questIdx = GetAdjustList(G_CDataManager())[i].m_questIdx;
        achievementIdx =
            GetAdjustList(G_CDataManager())[i].m_achievementIdx;
        if (m_user->getCurCharacQuestR()->isClearQuest((int)questIdx))
        {
            Quest* quest = G_CDataManager()->find_quest((int)questIdx);
            if ((quest == 0) || (quest->m_type != 7))
            {
                continue;
            }
            // ORIG 0x828ca49：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
            stAchievement ach;
            quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                ach.m_trigger2);
            _setTrigger(achievementIdx, ach.m_trigger0, ach.m_trigger1,
                        ach.m_trigger2);
            int titleIdx = quest->get_title_reward();
            CTitleBook::putTitleOnly(m_user, titleIdx, category, slot);
        }
    }

    if (m_user->get_charac_level() == 0x46)
    {
        Quest* quest = G_CDataManager()->find_quest(0x1983);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828cb43：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(0x1983, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        int titleIdx = quest->get_title_reward();
        CTitleBook::putTitleOnly(m_user, titleIdx, category, slot);
    }
    alter();
}

// ---- 发送 / 存档 ----

void CAchievement::sendList()
{
    stAchievement_DBData data;
    getData((char*)&data);

    PacketGuard packet;
    packet.put_header(0, 0x166);
    packet.put_int(data.m_count);
    for (int i = 0; i < data.m_count; ++i)
    {
        packet.put_int((int)data.m_entries[i].m_id);
        packet.put_short((int)data.m_entries[i].m_trigger0);
        packet.put_short((int)data.m_entries[i].m_trigger1);
        packet.put_short((int)data.m_entries[i].m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
}

void CAchievement::_sendSetTrigger(unsigned int id, unsigned char flag)
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(1, 0x1a1);
    // ORIG 0x828c2a2：cmpb flag; je 跳到 flag==0 分支，flag!=0 分支为 fall-through
    if (flag != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)flag);
        packet.put_int((int)trig.m_id);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int((int)trig.m_id);
        packet.put_short((int)trig.m_trigger0);
        packet.put_short((int)trig.m_trigger1);
        packet.put_short((int)trig.m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
}

void CAchievement::_sendComplete(unsigned int id, int titleIdx,
                                 ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(0, 0x167);
    packet.put_int((int)trig.m_id);
    packet.put_int((int)category);
    packet.put_int(slot);
    packet.put_int(titleIdx);
    // ORIG 0x828c436：GetCharacExpandData 结果不缓存局部变量（edx 直接传 getTitleSlotR）
    Inven_Item titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlotR(category, slot);
    packet.put_short((int)titleSlot.m_fieldb);
    packet.finalize(true);
    m_user->Send(packet);
}

void CAchievement::_sendCompleteReward(const Quest* quest,
                                       ENUM_TITLE_BOOK_CATEGORY category,
                                       int slot)
{
    unsigned int sectionIdx = _getSectionIdx(category, slot);
    if (sectionIdx == 0xffffffff)
    {
        return;
    }
    // ORIG 0x828bf54：stTitleSection 复制构造到局部（非引用）
    stTitleSection section =
        GetTitleSections(G_CDataManager())[sectionIdx];
    stTitleRewardBasis* reward = _getPostalReward(&section);
    if (reward == 0)
    {
        return;
    }
    if (_getTabComplete(&section, category))
    {
        _sendPostal(reward->m_itemIdx, reward->m_addInfo, reward->m_sender,
                    reward->m_subject);
    }
}

void CAchievement::_sendEachAchieveReward(const Quest* quest)
{
    const PostalRewardView* reward =
        (const PostalRewardView*)&quest->m_postalReward;
    if (reward->m_itemIdx == 0)
    {
        return;
    }
    _sendPostal(reward->m_itemIdx, reward->m_addInfo, reward->m_title,
                reward->m_contents);
}

void CAchievement::_sendPostal(int itemIdx, int addInfo, std::string sender,
                               std::string subject)
{
    Inven_Item item;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return;
    }
    item.m_addInfo = itemIdx;
    pItem->make_item(item);
    item.set_add_info(addInfo);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender.c_str(), item, 0, (unsigned int)m_user->getCurCharacNo(),
        subject.c_str(), (int)subject.size(), 0, m_user->GetServerGroup(),
        false, false);
}

void CAchievement::_gm_init()
{
    for (std::map<unsigned int, stAchievement>::iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest((int)it->first);
        if (quest == 0)
        {
            break;
        }
        if (!quest->get_init_achievement_trigger(it->second.m_trigger0,
                                                 it->second.m_trigger1,
                                                 it->second.m_trigger2))
        {
            break;
        }
        _sendSetTrigger(it->first, 0);
    }
    alter();
}

void CAchievement::_gm_report(std::string& out)
{
    char buf[0x400];
    // ORIG 0x828d45c：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    memset(buf, 0, 0x400);
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d489：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(idx);
        if (it == m_achievements.end())
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)ach.m_trigger0, (int)ach.m_trigger1,
                         (int)ach.m_trigger2);
        }
        else
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)it->second.m_trigger0,
                         (int)it->second.m_trigger1,
                         (int)it->second.m_trigger2);
        }
        out += buf;
        std::memset(buf, 0, sizeof(buf));
    }
}

void CAchievement::_gm_clearAll()
{
    // ORIG 0x828d73f：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d756：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        setTrigger(idx, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
    }
}

void CAchievement::_reset()
{
    m_user = 0;
    m_achievements.clear();
    m_field24 = 0;
    m_field25 = 0;
}

bool CAchievement::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("Achievement.cpp", 0x1d4), true);
    **guard << 0x286;
    **guard << (int)pUser->GetUID();
    stAchievement_DBSaveData* pData =
        guard->GetInBuffer<stAchievement_DBSaveData>();
    // ORIG 0x828c805：cmpl pData; jne 到 if 块，各路径直接赋返回常量
    if (pData != 0)
    {
        std::memset(pData, 0, sizeof(stAchievement_DBSaveData));
        getData((char*)&pData->m_data);
        pData->m_characNo = pUser->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
    return 0;
}

bool CAchievement::loadData(CUser* pUser, char* pData)
{
    stAchievement_DBData* in = (stAchievement_DBData*)pData;
    for (int i = 0; i < in->m_count; ++i)
    {
        m_achievements.insert(
            std::make_pair(in->m_entries[i].m_id, in->m_entries[i]));
    }
    m_user = pUser;
    _verifyAchievement((unsigned int)this);
    return 1;
}

void CAchievement::getData(char* pData) const
{
    stAchievement_DBData* out = (stAchievement_DBData*)pData;
    int i = 0;
    for (std::map<unsigned int, stAchievement>::const_iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it, ++i)
    {
        // ORIG 0x828c6e0：按 12 字节整块复制 value（3 个 dword），非逐字段
        out->m_entries[i] = it->second;
    }
    out->m_count = (int)m_achievements.size();
    out->m_saveTime = GlobalData::s_systemTime_.getCurSec();
}

// ---- 章节查询 ----

unsigned int CAchievement::_getSectionIdx(ENUM_TITLE_BOOK_CATEGORY category,
                                          int slot)
{
    // ORIG 0x828bcc0：while(i<size)；section 内层块使析构先于 ++i；
    // 每次循环直接调用 GetTitleSections(G_CDataManager())，不缓存局部引用
    unsigned int i = 0;
    while (i < GetTitleSections(G_CDataManager()).size())
    {
        {
            stTitleSection section = GetTitleSections(G_CDataManager())[i];
            if ((section.m_category == (int)category) &&
                (section.m_startIdx <= slot) && (slot <= section.m_endIdx))
            {
                return i;
            }
        }
        ++i;
    }
    return 0xffffffff;
}

stTitleRewardBasis* CAchievement::_getPostalReward(stTitleSection* section)
{
    // ORIG 0x828bd64：两个局部清零（movl $0,-0x14 / -0x10），-0x14 为死变量
    unsigned int i = 0;
    unsigned int unused = 0;
    while (i < section->m_rewards.size())
    {
        stTitleRewardBasis* reward = section->m_rewards[i];
        if ((reward != 0) && (reward->m_type == 3))
        {
            return reward;
        }
        ++i;
    }
    return 0;
}

bool CAchievement::_getTabComplete(stTitleSection* section,
                                   ENUM_TITLE_BOOK_CATEGORY category)
{
    // ORIG 0x828bdd4：不缓存 elements 引用，find/end 各重新取一次；
    // result 变量 + break（命中未清除置 0 提前退出）
    int result = 1;
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).find((int)category);
    if (it != GetTitleElements(G_CDataManager()).end())
    {
        // ORIG 0x828be47：vector 复制构造到局部（非引用），
        // 且每次成员访问都重新 operator[]（不缓存 elem 引用）
        std::vector<stTitleElement> list = it->second;
        for (unsigned int i = (unsigned int)section->m_startIdx;
             i < (unsigned int)section->m_endIdx; ++i)
        {
            if ((list[i].m_index != -1) &&
                (list[i].m_achievementIdx != -1))
            {
                if (!isClearedAchievement(
                        (unsigned int)list[i].m_achievementIdx))
                {
                    result = 0;
                    break;
                }
            }
        }
    }
    return result;
}
