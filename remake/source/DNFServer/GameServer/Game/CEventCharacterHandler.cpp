// ============================================================================
// df_game_r 还原 —— CEventCharacterHandler（G2-4 批次，活动角色处理）。
// 依据 docs/class_func_reports/CEventCharacterHandler.md + ORIG 反汇编。
// ============================================================================

#include <algorithm>
#include <utility>
#include <vector>

#include "GlobalData.h"

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

namespace WongWork
{
class CQuestClear
{
public:
    void setClearedQuest(unsigned int questIdx);
};

class CHandlePremium
{
public:
    static void handleSetUserPCRoom(CUser* user, long start, long end);
};
}

struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};

class STEventCharacterItem
{
public:
    unsigned long m_itemIdx;  // +0x00
    int m_count;              // +0x04
    int m_reason;             // +0x08
};

class STEventCharacterInfo
{
public:
    STEventCharacterInfo();
    ~STEventCharacterInfo();

    unsigned char m_growType;       // +0x00
    unsigned char m_secondGrowType; // +0x01
    char m_pad2[2];
    std::vector<std::pair<int, int> > m_skillList;   // +0x04
    std::vector<STEventCharacterItem> m_itemList;    // +0x10
};

class EventCharacterParameterScript
{
public:
    bool GetEventCharacterInfo(int type, int level,
                               STEventCharacterInfo& info) const;
};

class CDataManager
{
public:
    const CItem* find_item(int itemIdx) const;
    const CSkill* find_skill(int job, int skillIdx) const;

    char m_pad[0xa86c];                                  // +0x00
    EventCharacterParameterScript m_eventCharacScript;   // +0xa86c
};

class CEventCharacterHandler
{
public:
    CEventCharacterHandler();

    bool _makeEventCharacter(CUser* user, int level);
    void _UpdateEventCharacInfo(CUser* user);
    bool _checkMakeCommand(CUser* user, int level, int& cmdIdx);
    void _SetClearedQuest(CUser* user);
    bool _MasterNewSkill(CUser* user, int skillIdx, int count);
    bool _AddItem(CUser* user, unsigned long itemIdx, int count, int reason);
    void _SetLevel(CUser* user, int level);
    bool _SetGrowType(CUser* user, int growTypeKind, int growType);
};

CEventCharacterHandler* CEventCharacterHandlerInstance();

CEventCharacterHandler::CEventCharacterHandler() {}

bool CEventCharacterHandler::_checkMakeCommand(CUser* user, int level, int& cmdIdx)
{
    int now = GlobalData::s_systemTime_.getCurSec();
    bool bInEventWindow = true;
    if (now > 0x4b055d6f)
    {
        now = GlobalData::s_systemTime_.getCurSec();
        if (now < 0x4b3bbf51)
        {
            bInEventWindow = false;
        }
    }
    if (bInEventWindow)
    {
        return false;
    }

    if (level < 0x47)
    {
        char growType = user->getCurCharacEventCharacterGrowtype();
        if (growType < 7)
        {
            cmdIdx = user->get_charac_job() * 10 + growType;
            return true;
        }
    }
    return false;
}

void CEventCharacterHandler::_UpdateEventCharacInfo(CUser* user)
{
    user->setCurCharacEventCharacterGrowtype(-1);
}

void CEventCharacterHandler::_SetClearedQuest(CUser* user)
{
    switch (user->get_charac_job())
    {
    case 0:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x37f);
        break;
    case 1:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x382);
        break;
    case 2:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x380);
        break;
    case 3:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x383);
        break;
    case 4:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x381);
        break;
    case 5:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x13f9);
        break;
    case 6:
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x384);
        break;
    default:
        break;
    }
}

bool CEventCharacterHandler::_MasterNewSkill(CUser* user, int skillIdx, int count)
{
    int job = user->get_charac_job();
    const CSkill* skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return false;
    }

    int group = skill->get_group();
    SkillSlot* slot = user->getCurCharacSkillR();
    int slotNo = slot->get_skillslot_no(skillIdx, group,
                                        ENUM_SKILL_TREE_KIND_0, true);
    unsigned char* buf = (unsigned char*)slot->get_skillslot_buf(ENUM_SKILL_TREE_KIND_0);
    if (buf == 0)
    {
        return false;
    }

    stBuySkillInfo info;
    info.m_field0 = 0;
    info.m_skillIdx = (char)skillIdx;
    info.m_count = (char)((char)count - (char)buf[slotNo * 2 + 1]);
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
    if (item != 0 && item->is_stackable())
    {
        const CStackableItem* stack = (const CStackableItem*)item;
        if (stack->getStackableLimit() < count)
        {
            count = stack->getStackableLimit();
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
        "bool CEventCharacterHandler::_AddItem(CUser*, itemIndexOnlyServer_t, int, int)",
        0xda, 0);
    tr("%s:'%s' GM Mode Add Item(%d).", accStr, name, slot);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)itemSpace, slot);
    return true;
}

void CEventCharacterHandler::_SetLevel(CUser* user, int level)
{
    if (level > 0x46)
    {
        return;
    }

    bool bChanged = true;
    int diff = level - user->get_charac_level();
    if (diff == 0)
    {
        bChanged = false;
    }
    else if (diff < 0)
    {
        for (int i = 0; diff < i; --i)
        {
            int newLevel = user->get_charac_level() - 1;
            int maxLevel = 0x46;
            user->SetCharacLevel(std::min(newLevel, maxLevel));
        }
    }
    else
    {
        for (int i = 0; i < diff; ++i)
        {
            int newLevel = user->get_charac_level() + 1;
            int maxLevel = 0x46;
            user->SetCharacLevel(std::min(newLevel, maxLevel));
        }
    }
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

    return user->ChangeGrowType_GM(first, second);
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
    cMyTrace tr("bool CEventCharacterHandler::_makeEventCharacter(CUser*, int)", 0x2b, 0);
    tr("%s:'%s' Make Event Character(%d) level(%d).", accStr, name, cmdIdx, level);

    STEventCharacterInfo info;
    if (!G_CDataManager()->m_eventCharacScript.GetEventCharacterInfo(cmdIdx, level, info))
    {
        return false;
    }

    if (level < 0xf)
    {
        _SetLevel(user, level);
    }
    else
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
