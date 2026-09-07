// ============================================================================
// df_game_r 还原 —— CEventCharacterHandler（G2-4 批次，活动角色处理）。
// 依据 docs/class_func_reports/CEventCharacterHandler.md + ORIG 反汇编。
// ============================================================================

#include <algorithm>
#include <utility>
#include <vector>

#include "EventCharacterParameterScript.h"
#include "GlobalData.h"
#include "TimerQueue.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CHandlePremium.h"
#include "CEventCharacterHandler.h"

// ---- 外部符号声明（对应 TU 翻译后移除） ----
char* NumberToString(unsigned int value, int radix);

enum eItemAddReason
{
    eItemAddReason_0 = 0
};

enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0
};

enum ENUM_NOTIPACKET
{
    ENUM_NOTIPACKET_0 = 0
};

enum ENUM_SKILL_TREE_KIND
{
    ENUM_SKILL_TREE_KIND_0 = 0
};

#ifndef DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
#define DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
enum ENUM_PREMIUM_TYPE { ENUM_PREMIUM_TYPE_0 = 0 };
#endif

class CItem
{
public:
    bool is_stackable() const;
};

class CStackableItem
{
public:
    int getStackableLimit() const;
};

class CSkill
{
public:
    int get_group() const;
};

class SkillSlot;

class CUserCharacInfo
{
public:
    int get_charac_job() const;
    int get_charac_level() const;
    const char* getCurCharacName() const;
    class SkillSlot* getCurCharacSkillR() const;
    signed char getCurCharFirstGrowType() const;
    char getCurCharacEventCharacterGrowtype();
    void setCurCharacEventCharacterGrowtype(char growType);
};

class SkillSlot
{
public:
    int get_skillslot_no(int skillIdx, int group,
                         ENUM_SKILL_TREE_KIND treeKind, bool flag) const;
    void* get_skillslot_buf(ENUM_SKILL_TREE_KIND treeKind) const;
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    unsigned int get_acc_id() const;
    unsigned int GetUID() const;
    int get_state();
    bool isAffectedPremium(ENUM_PREMIUM_TYPE type) const;   // ORIG _ZNK5CUser18isAffectedPremiumE15ENUM_PREMIUM_TYPE
    void* GetPremiumInfoW();                                // ORIG WongWork::CUserPremium*
    bool HasCurCharacRentalItem(int flag) const;            // ORIG _ZNK5CUser22HasCurCharacRentalItemEi 0x86772d2
    void Send(PacketGuard& packet);                         // ORIG _ZN5CUser4SendER11PacketGuard 0x86485ba
    class UserQuest* getCurCharacQuestW();
    int AddItem(int itemIdx, int count, eItemAddReason reason,
                ENUM_ITEMSPACE& itemSpace, int param5);
    void SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE itemSpace, int slot);
    bool master_new_skill(struct stBuySkillInfo& info, bool flag);
    void SetCharacLevel(int level);
    bool ChangeGrowType_GM(int growType, int secondGrowType);
    void AddDungeonClear(int dungeonIdx, int count);
    void SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET notiType, int param);
};

class CDataManager;
CDataManager* G_CDataManager();

class CSystemTime;
CSystemTime& s_systemTime_holder();

namespace WongWork
{
// ---- CHandlePremium 组件（ORIG 报告 docs/class_func_reports/WongWork__CHandlePremium/）----
class CUserPremium
{
public:
    // ORIG _ZN8WongWork13CUserPremium10AddPremiumEiiii（签名按 handleSetUserPremium 调用点）
    void AddPremium(int type, int start, int end, int flag);
    void RemovePremium(int type);
    void ReCalcAdvantage();
    void RecalcAdditionalInfo(CUser const* user);
};

class CQuestClear
{
public:
    void setClearedQuest(unsigned int questIdx);
};
}

struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};

class CDataManager
{
public:
    const CItem* find_item(int itemIdx) const;
    const CSkill* find_skill(int job, int skillIdx) const;

    char m_pad[0xa86c];                                  // +0x00
    EventCharacterParameterScript m_eventCharacScript;   // +0xa86c
};
CEventCharacterHandler::CEventCharacterHandler() {}

bool CEventCharacterHandler::_checkMakeCommand(CUser* user, int level, int& cmdIdx)
{
    // ORIG 形（P0 律）：两次 getCurSec 均无 now 槽（call 后直接 cmp %eax）；
    // 判断臂 mov$1/mov$0 汇聚；各 early-return 的 fall-through 为 return 块
    // （P137 方向：if(cond>imm) return false;）；growType 双段式（P197③
    // movl $0,-0xc 声明预置 + 独立赋值）；return true 为函数末条 fall-through。
    // 复合短路 || 直接作 if 条件（probe pr5.cpp f4 实证：4.4 出 mov$1/jmp/
    // mov$0 无槽汇聚 + test %al 直测，与 ORIG 判断树逐条同形；具名 bool
    // 赋值则落槽= N3 不可取）。
    if (GlobalData::s_systemTime_.getCurSec() <= 0x4b055d6f ||
        GlobalData::s_systemTime_.getCurSec() >= 0x4b3bbf51)
        return false;
    if (level > 0x46)
        return false;
    int growType = 0;
    growType = user->getCurCharacEventCharacterGrowtype();
    if (growType > 6)
        return false;
    cmdIdx = user->get_charac_job() * 10 + growType;
    return true;
}

void CEventCharacterHandler::_UpdateEventCharacInfo(CUser* user)
{
    user->setCurCharacEventCharacterGrowtype(-1);
}

void CEventCharacterHandler::_SetClearedQuest(CUser* user)
{
    if (user->get_charac_job() == 0)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x37f);
    }
    else if (user->get_charac_job() == 1)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x382);
    }
    else if (user->get_charac_job() == 2)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x380);
    }
    else if (user->get_charac_job() == 3)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x383);
    }
    else if (user->get_charac_job() == 4)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x381);
    }
    else if (user->get_charac_job() == 5)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x13f9);
    }
    else if (user->get_charac_job() == 6)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x384);
    }
}

bool CEventCharacterHandler::_MasterNewSkill(CUser* user, int skillIdx, int count)
{
    // ORIG 0x0848e7d6：skill 两段式（入口 movl $0,-0x18 预清）；job 内联
    // rvalue 经 %ebx 跨 G_CDataManager call（P197②）；getCurCharacSkillR
    // 两独立重调（+0x5a/+0x8e，P48）。
    // [B24D判词 N] 语义正确：find_skill→get_group+get_skillslot_no+buf→
    //   m_count=(char)(count-(buf[slot*2+1]))→<=0早返→master_new_skill。diff=
    //   ORIG movzbl 0x1(%eax) disp内联 vs 我方 add$1+movzbl（多1条），lowering。
    const CSkill* skill = 0;
    skill = (const CSkill*)G_CDataManager()->find_skill(user->get_charac_job(),
                                                        skillIdx);
    if (skill == 0)
    {
        return false;
    }

    int group = skill->get_group();
    int slotNo = ((SkillSlot*)user->getCurCharacSkillR())->get_skillslot_no(
        skillIdx, group, ENUM_SKILL_TREE_KIND_0, true);
    unsigned char* buf = (unsigned char*)
        ((SkillSlot*)user->getCurCharacSkillR())->get_skillslot_buf(
            ENUM_SKILL_TREE_KIND_0);
    if (buf == 0)
    {
        return false;
    }

    stBuySkillInfo info;
    info.m_field0 = 0;
    info.m_skillIdx = (char)skillIdx;
    // ORIG +0xc5：`add -0xc(buf),%eax; movzbl 0x1(%eax)`——(buf+2*slotNo) 先行、
    // +1 留 disp。buf[slotNo*2+1] 被 4.4 折成 add$1+add base 两步。
    info.m_count = (char)((char)count - (char)((buf + slotNo * 2)[1]));
    if ((signed char)info.m_count <= 0)
    {
        return false;
    }

    user->master_new_skill(info, true);
    return true;
}

bool CEventCharacterHandler::_AddItem(CUser* user, unsigned long itemIdx, int count,
                                      int reason)
{
    if (itemIdx == 0xffffffff)
    {
        long start = GlobalData::s_systemTime_.getCurSec();
        long end = GlobalData::s_systemTime_.getCurSec() + 0x15180;
        WongWork::CHandlePremium::handleSetUserPCRoom(user, start, end);
    }

    const CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item != 0)
    {
        if (item->is_stackable())
        {
            const CStackableItem* stack = (const CStackableItem*)item;
            if (stack->getStackableLimit() < count)
            {
                count = stack->getStackableLimit();
            }
        }
    }

    ENUM_ITEMSPACE itemSpace;
    int slot = user->AddItem((int)itemIdx, count, (eItemAddReason)6, itemSpace, reason);
    if (slot < 0)
    {
        return false;
    }

    const char* name = user->getCurCharacName();
    char* accStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace tr(
        __PRETTY_FUNCTION__,
        0xda, 0);
    tr("%s:'%s' GM Mode Add Item(%d).", accStr, name, slot);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)itemSpace, slot);
    return true;
}
bool CEventCharacterHandler::_SetLevel(CUser* user, int level)
{
    if (level > 0x46)
    {
        return false;
    }

    bool bChanged = true;
    int diff = level - user->get_charac_level();
    if (diff == 0)
    {
        bChanged = false;
    }
    else if (diff > 0)
    {
        for (int i = 0; i < diff; ++i)
        {
            int newLevel = user->get_charac_level() + 1;
            int maxLevel = 0x46;
            user->SetCharacLevel(std::min(maxLevel, newLevel));
        }
    }
    else
    {
        for (int i = 0; i > diff; --i)
        {
            int newLevel = user->get_charac_level() - 1;
            int maxLevel = 0x46;
            user->SetCharacLevel(std::min(maxLevel, newLevel));
        }
    }
    return bChanged;
}

bool CEventCharacterHandler::_SetGrowType(CUser* user, int growTypeKind, int growType)
{
    int first = 0;
    int second = 0;
    if (growTypeKind == 1)
    {
        first = growType;
    }
    else if (growTypeKind == 2)
    {
        if (user->getCurCharFirstGrowType() == 0)
        {
            return false;
        }
        first = user->getCurCharFirstGrowType();
        second = growType;
    }
    else
    {
        return false;
    }

    if (user->ChangeGrowType_GM(first, second))
    {
        return true;
    }
    return false;
}

bool CEventCharacterHandler::_makeEventCharacter(CUser* user, int level)
{
    int cmdIdx = -1;
    if (!_checkMakeCommand(user, level, cmdIdx))
    {
        return false;
    }

    const char* name = user->getCurCharacName();
    char* accStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2b, 0);
    tr("%s:'%s' Make Event Character(%d) level(%d).", accStr, name, cmdIdx, level);

    STEventCharacterInfo info;
    if (!G_CDataManager()->m_eventCharacScript.GetEventCharacterInfo(cmdIdx, level, info))
    {
        return false;
    }

    if (level > 0xe)
    {
        _SetLevel(user, 0xf);
        if (info.m_growType != 0)
        {
            _SetGrowType(user, 1, info.m_growType);
        }
        if (level > 0xf)
        {
            _SetLevel(user, level);
            if (level > 0x31 && info.m_secondGrowType != 0)
            {
                _SetGrowType(user, 2, info.m_secondGrowType);
            }
        }
    }
    else
    {
        _SetLevel(user, level);
    }

    user->AddDungeonClear(2, 1);
    user->AddDungeonClear(0x1a, 1);
    _SetClearedQuest(user);

    std::vector<std::pair<int, int> >::iterator skillIt = info.m_skillList.begin();
    for (; skillIt != info.m_skillList.end(); ++skillIt)
    {
        _MasterNewSkill(user, skillIt->first, skillIt->second);
    }

    std::vector<STEventCharacterItem>::iterator itemIt = info.m_itemList.begin();
    for (; itemIt != info.m_itemList.end(); ++itemIt)
    {
        _AddItem(user, itemIt->m_itemIdx, itemIt->m_count, itemIt->m_reason);
    }

    _UpdateEventCharacInfo(user);
    user->SendNotiPacket(CUser::eSendTarget_1, (ENUM_NOTIPACKET)0x25, 0);
    return true;
}

CEventCharacterHandler* CEventCharacterHandlerInstance()
{
    static CEventCharacterHandler instance;
    return &instance;
}
