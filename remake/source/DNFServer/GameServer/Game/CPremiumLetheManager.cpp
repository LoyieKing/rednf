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
#include "PacketGuard.h"

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

CPremiumLetheManager::CPremiumLetheManager()
{
}

unsigned char CPremiumLetheManager::USE_LETHE_STATE = 1;
unsigned char CPremiumLetheManager::SKILL_CONFIRM_STATE = 2;
unsigned char CPremiumLetheManager::BACK_TO_PRE_STATE = 3;
unsigned char CPremiumLetheManager::INIT_LETHE_SKILL = 4;
unsigned char CPremiumLetheManager::NOT_USE_LETHE_STATE = 0;

// SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD 权威定义已迁移至 SigTypes.h
// （FixMissingSigTypes；DB 队列负载，pack(1)，sizeof=0x1a7）。

// ---- CUser 方法声明见 CUser.h（本 TU 直接真实调用） ----

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
    cMyTrace(__PRETTY_FUNCTION__, 0x35, 0)(
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
    cMyTrace(__PRETTY_FUNCTION__,
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
    cMyTrace(__PRETTY_FUNCTION__,
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
    cMyTrace(__PRETTY_FUNCTION__,
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
        goto FAIL_TRACE;    // ORIG jne 尾置 else 块（4.4 threading 会前移，goto 阻断）
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
    return;
FAIL_TRACE:
    {
        // [b28c-L29] P221：具名 trace 对象使 PacketGuard(-0x28)/trace(-0x1c)
        // 的 sibling 块槽方向与 ORIG 相反；全表达式临时链复现 ORIG 槽深。
        cMyTrace(__PRETTY_FUNCTION__, 0x93, 0)("ONE_DAY_LETHE : CAN'T USE INIT STATE");
    }
}

// [b28d-lane15 判词·N（N82 帧槽复用 + P5 虚槽族）] ORIG 0x085c421e（TSV ndiff=330，
// orig 327/new 346）。首段 `mov user; mov (%eax),%eax; add $0x34,%eax; mov (%eax),%edx;
// call *%edx` = P5 虚槽直调 getCurCharacSkillW；残差主体为大函数帧/槽复用策略差
// （N82）与 P210 帧继承。本 TU 级 diff 已仅剩本件 + __static_init。语义完成，封顶。
void CPremiumLetheManager::InitSkill(CUser* user, bool flag,
                                     ENUM_SKILL_TREE_KIND kind)
{
    SkillSlot* slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->clear_all_skills(kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->set_parent(user);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        int kind2;                       // ORIG：kind==NONE(-1)→0、kind==1(0)→1、其余→2
        if (kind == SKILL_TREE_1)
            kind2 = 1;
        else if (kind == SKILL_TREE_NONE)
            kind2 = 0;
        else
            kind2 = 2;
        user->givePvPSkillTree(0, true, kind2);
        int grade = user->get_pvp_grade();
        int second = (signed char)user->getCurCharSecondGrowType();
        int first = (signed char)user->getCurCharFirstGrowType();
        int job = user->get_charac_job();
        PvPSkillTreeParameterScript* script =
            &G_CDataManager()->m_pvpSkillTree;
        int point = script->getPvPSkillPoint(job, first, second, grade, false);
        slot = (SkillSlot*)user->getCurCharacSkillW();
        slot->set_remain_sp_at_index(point, kind);
        if (flag)
        {
            user->send_skill_info();
            const char* name = user->getCurCharacName();
            cMyTrace trace(__PRETTY_FUNCTION__,
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
            int kind3;                  // ORIG：同 kind2（NONE→0、1→1、其余→2）
            if (kind == SKILL_TREE_1)
                kind3 = 1;
            else if (kind == SKILL_TREE_NONE)
                kind3 = 0;
            else
                kind3 = 2;
            changer.SkillInitialize(user, kind3, false);
            if (flag)
            {
                user->send_skill_info();
                const char* name = user->getCurCharacName();
                cMyTrace trace(__PRETTY_FUNCTION__,
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
    // (int)：CUser::GetUID() 返回 unsigned int（CUser.h:2910），不加转换会选中
    // operator<<(unsigned int)；ORIG 0x085c48b2 调用点为 operator<<(int)
    // （P167 调用点差集实证 ORIG lsEi×2 / 修复前 lsEi+lsEj）。
    **guard << (int)user->GetUID();
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
