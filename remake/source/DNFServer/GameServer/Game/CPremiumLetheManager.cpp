// df_game_r 还原 —— CPremiumLetheManager（G2-4 批次，忘川水/付费技能重置管理）
// 参照 docs/class_func_reports/CPremiumLetheManager.md 与 ORIG 反汇编逐函数实现。
// 原源文件 PremiumLetheManager.cpp；数据全局态经 DB 队列（_update）下账。
#include <string.h>

#include <vector>

#include "CDataManager.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "SkillSlot.h"
#include "StreamPool.h"
#include "GameWorld.h"

// ---- 全局单例（CUserCharacInfo.h 已声明 namespace GlobalData；不引入
//      GlobalData.h 以避免 class/namespace 形态冲突） ----
namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
}

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；构造/析构由
// GameStubs 提供桩符号，本 TU 只声明不定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ---- 忘川水状态（ORIG 数据段 .data 0x93abc60 / .bss 0x9472dbc）----
class CPremiumLetheManager
{
public:
    CPremiumLetheManager();
    ~CPremiumLetheManager();

    bool ResetSkillReq(CUser* user);
    bool ConfirmSkillReq(CUser* user);
    bool BackToPre(CUser* user, _Mastered_skill* skills,
                   ENUM_SKILL_TREE_KIND kind);
    bool UpdateBackupSkillFlag(CUser* user, ENUM_SKILL_TREE_KIND kind);
    bool InitLetheSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _resetSkill(CUser* user);
    void InitSkill(CUser* user, bool flag, ENUM_SKILL_TREE_KIND kind);
    bool BackupSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _update(CUser* user, unsigned char state, ENUM_SKILL_TREE_KIND kind);
    void _removeInitSkill(CUser* user, _Mastered_skill* skills);

    static unsigned char USE_LETHE_STATE;       // +0x093abc60 = 1
    static unsigned char SKILL_CONFIRM_STATE;   // +0x093abc61 = 2
    static unsigned char BACK_TO_PRE_STATE;     // +0x093abc62 = 3
    static unsigned char INIT_LETHE_SKILL;      // +0x093abc63 = 4
    static unsigned char NOT_USE_LETHE_STATE;   // +0x09472dbc = 0
};

unsigned char CPremiumLetheManager::USE_LETHE_STATE = 1;
unsigned char CPremiumLetheManager::SKILL_CONFIRM_STATE = 2;
unsigned char CPremiumLetheManager::BACK_TO_PRE_STATE = 3;
unsigned char CPremiumLetheManager::INIT_LETHE_SKILL = 4;
unsigned char CPremiumLetheManager::NOT_USE_LETHE_STATE = 0;

// ============================================================================
// SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD（DB 队列负载；+0x0d 技能区 0x198）
// ============================================================================
struct SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD
{
    int m_accID;                     // +0x00
    int m_characNo;                  // +0x04
    char m_pad08[4];                 // +0x08
    unsigned char m_state;           // +0x0c
    char m_skills[0x198];            // +0x0d
    unsigned char m_state2;          // +0x1a5
    unsigned char m_treeKind;        // +0x1a6
};

// ---- CUser 方法声明见 CUser.h（本 TU 直接真实调用） ----

CPremiumLetheManager::CPremiumLetheManager()
{
}

CPremiumLetheManager::~CPremiumLetheManager()
{
}

bool CPremiumLetheManager::ResetSkillReq(CUser* user)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _resetSkill(user);
    return true;
}

bool CPremiumLetheManager::ConfirmSkillReq(CUser* user)
{
    if (user == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    cMyTrace("bool CPremiumLetheManager::ConfirmSkillReq(CUser*)", 0x35, 0)(
        "ONE_DAY_LETHE : CONFIRM_SKILL, char(%s)", user->getCurCharacName());
    _update(user, SKILL_CONFIRM_STATE,
            (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
    return true;
}

bool CPremiumLetheManager::BackToPre(CUser* user, _Mastered_skill* skills,
                                     ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    InitSkill(user, false, kind);
    if (!user->master_new_skills(skills, kind))
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(NOT_USE_LETHE_STATE);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(NOT_USE_LETHE_STATE);
    }
    cMyTrace("bool CPremiumLetheManager::BackToPre(CUser*, _Mastered_skill*, ENUM_SKILL_TREE_KIND)",
             0x4f, 0)("ONE_DAY_LETHE : SUCCESS_BACK_TO_PRE, char(%s)",
                      user->getCurCharacName());
    return true;
}

bool CPremiumLetheManager::UpdateBackupSkillFlag(CUser* user,
                                                 ENUM_SKILL_TREE_KIND kind)
{
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    _update(user, NOT_USE_LETHE_STATE, kind);
    cMyTrace("bool CPremiumLetheManager::UpdateBackupSkillFlag(CUser*, ENUM_SKILL_TREE_KIND)",
             0x69, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_LEGACY_LETHE, char(%s)",
                      user->getCurCharacName());
    return true;
}

bool CPremiumLetheManager::InitLetheSkill(CUser* user,
                                          ENUM_SKILL_TREE_KIND kind)
{
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _update(user, INIT_LETHE_SKILL, kind);
    cMyTrace("bool CPremiumLetheManager::InitLetheSkill(CUser*, ENUM_SKILL_TREE_KIND)",
             0x75, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_1ST_GROW_CHAR, char(%s)",
                      user->getCurCharacName());
    return true;
}

void CPremiumLetheManager::_resetSkill(CUser* user)
{
    unsigned char flag;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        flag = user->GetOneDayLetheFlag();
    }
    else
    {
        flag = user->GetOneDayLetheFlag2ND();
    }
    if (flag != USE_LETHE_STATE)
    {
        cMyTrace trace("void CPremiumLetheManager::_resetSkill(CUser*)", 0x93, 0);
        trace("ONE_DAY_LETHE : CAN'T USE INIT STATE");
    }
    else
    {
        if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
        {
        }
        else
        {
            InitSkill(user, true,
                      (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
            PacketGuard guard;
            guard.put_header(1, 0xd5);
            guard.put_byte(1);
            guard.finalize(true);
            user->Send(guard);
        }
    }
}

void CPremiumLetheManager::InitSkill(CUser* user, bool flag,
                                     ENUM_SKILL_TREE_KIND kind)
{
    SkillSlot* slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->clear_all_skills(kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->set_parent(user);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        int kind2 = 1;
        if (kind != SKILL_TREE_NONE && kind != SKILL_TREE_1)
        {
            kind2 = 2;
        }
        user->givePvPSkillTree(0, true, kind2);
        int grade = user->get_pvp_grade();
        int second = (signed char)user->getCurCharSecondGrowType();
        int first = (signed char)user->getCurCharFirstGrowType();
        int job = user->get_charac_job();
        PvPSkillTreeParameterScript* script =
            (PvPSkillTreeParameterScript*)((char*)G_CDataManager() + 0xa800);
        int point = script->getPvPSkillPoint(job, first, second, grade, false);
        slot = (SkillSlot*)user->getCurCharacSkillW();
        slot->set_remain_sp_at_index(point, kind);
        if (flag)
        {
            user->send_skill_info();
            const char* name = user->getCurCharacName();
            cMyTrace trace("void CPremiumLetheManager::InitSkill(CUser*, bool, ENUM_SKILL_TREE_KIND)",
                           0xb1, 0);
            trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                  name);
        }
    }
    else
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x46),
                user->get_charac_job());
        }
        else
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x1de),
                user->get_charac_job());
        }
        std::vector<std::pair<int, int> > vec;
        std::pair<int, int> p;
        char second = user->getCurCharSecondGrowType();
        char first = user->getCurCharFirstGrowType();
        CCharacter* ch = (CCharacter*)(
            *(int*)((char*)G_CDataManager() + 0x14) +
            user->get_charac_job() * 0x7dc);
        char got = ch->get_give_skill(first, second, vec, 0);
        if (got == 1)
        {
            int i;
            for (i = 0; i < (int)vec.size(); ++i)
            {
                p = vec[i];
                slot = (SkillSlot*)user->getCurCharacSkillW();
                slot->growtype_skill(user->get_charac_job(), p.first, p.second,
                                     kind);
            }
            int expertJob = user->GetCurCharacExpertJobType();
            void* script = G_CDataManager()->GetExpertJobScript(expertJob);
            if (script != 0)
            {
                std::vector<std::pair<int, int> >* expertVec =
                    (std::vector<std::pair<int, int> >*)((char*)script + 0xc);
                unsigned int i2;
                for (i2 = 0; i2 < expertVec->size(); ++i2)
                {
                    int level =
                        user->GetCurExpertJobLevel(
                            user->GetCurCharacExpertJobExp());
                    int skillIdx = (*expertVec)[i2].first;
                    slot = (SkillSlot*)user->getCurCharacSkillW();
                    slot->growtype_skill(user->get_charac_job(), skillIdx, level,
                                         kind);
                }
            }
            WongWork::CSkillChanger changer;
            int kind3 = 1;
            if (kind != SKILL_TREE_NONE && kind != SKILL_TREE_1)
            {
                kind3 = 2;
            }
            changer.SkillInitialize(user, kind3, false);
            if (flag)
            {
                user->send_skill_info();
                const char* name = user->getCurCharacName();
                cMyTrace trace("void CPremiumLetheManager::InitSkill(CUser*, bool, ENUM_SKILL_TREE_KIND)",
                               0xf7, 0);
                trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                      name);
            }
        }
    }
}

bool CPremiumLetheManager::BackupSkill(CUser* user,
                                       ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag2ND() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_1);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_2);
    }
    return true;
}

void CPremiumLetheManager::_update(CUser* user, unsigned char state,
                                   ENUM_SKILL_TREE_KIND kind)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PremiumLetheManager.cpp",
                                                          0x125),
                       true);
    **guard << 0x15c;
    **guard << user->GetUID();
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* result =
        guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
    result->m_accID = user->get_acc_id();
    result->m_characNo = user->getCurCharacNo();
    if (state == USE_LETHE_STATE || state == SKILL_CONFIRM_STATE ||
        INIT_LETHE_SKILL != 0)
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x46, 0x198);
        }
        else
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x1de, 0x198);
        }
    }
    else
    {
        memset(result->m_skills, 0, 0x198);
    }
    result->m_state = state;
    result->m_state2 = state;
    result->m_treeKind = (unsigned char)kind;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CPremiumLetheManager::_removeInitSkill(CUser* user,
                                            _Mastered_skill* skills)
{
    _Mastered_skill* pSkill;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x46);
    }
    else
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x1de);
    }
    for (int i = 0; i <= 0xcb; ++i)
    {
        for (int j = 0; j <= 0xcb; ++j)
        {
            if (pSkill[i].m_index != 0 &&
                pSkill[i].m_index == skills[j].m_index)
            {
                skills[j].m_level =
                    (char)(skills[j].m_level - pSkill[i].m_level);
                if (skills[j].m_level < 1)
                {
                    skills[j].m_index = 0;
                    skills[j].m_level = 0;
                }
            }
        }
    }
}
