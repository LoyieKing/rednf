// ============================================================================
// [round5 TU 拆分 2026-09-10] 本 TU 逐函数拆分，实现散落 Game/SkillSlot/<函数名>.cpp。
// 本文件保留**必须同 TU 且定义序不变**的函数组（见 local://split_spec.md §3–§4）：
//   (a) 需要 CUser / GameWorld 完整类型的函数（拆出即必须 include GameWorld.h/CUser.h
//       ⇒ 每个文件各发一份 std::ios_base::Init 静态初始化，多出 _GLOBAL__I_* 键）；
//   (b) GI 键承载者 SkillSlot::set_skill_slot 必须仍是本 TU 第一个函数定义
//       （基线键名 _GLOBAL__I__ZN9SkillSlot14set_skill_slotEPcS0_i 不变）；
//   (c) addSkillOnCreateCharacter：其体内 `std::vector<std::pair<int,int>>` 按值拷贝构造
//       触发的 weak 实例 __uninitialized_copy<...>（COMDAT）在本 TU 编译时取 39 条指令形
//       （与基线 AE 一致）；单独成文件时同一实例取 56 条形 ⇒ 该函数留在本文件。
// 搬走的每个函数在原位置留有 `// [拆分] → <文件>` 指引；下面两个局部视图结构体
// （_Charac_info_nibble_field / STGuild*）仍被留在本文件的 SendSkillInfo 与
// master_new_guild_skill 使用，故保留在文件作用域。
// ============================================================================

// df_game_r 玩家技能槽 SkillSlot 还原（G1 玩家域第一批）。
// 逐函数对照 docs/class_func_reports/SkillSlot.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
#include <string.h>

#include <utility>

#include "SkillSlot.h"
#include "CSkill.h"
#include "LogManager.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "CCharacter.h"
#include "STExpertJobScript.h"
#include "PacketGuard.h"
#include "CInventory.h"

// ---- _Charac_info +0x29 低 4 位有符号位域（ORIG：movzbl; shl $4; sar $4,%al）----
struct _Charac_info_nibble_field
{
    unsigned char m_pad[0x29];
    signed char m_specialNibble : 4;
    signed char m_padNibble : 4;
};

// ---- [b28d-solo-SkillSlot] STGuildDBInfoOnly 尾段组件视图（P161）----
// ORIG master_new_guild_skill 0x0860387e 段把 `m_skills + i*5` 折成
// `lea base+i*5; add $0x40; mov 0x5(%eax)`（int 读）/ `add $0x49`（字节读）：
// 把 +0x40 处建模为 Section（内含 +0x05 起的 5B 条目数组），4.4-O0 即发同形。
// 纯 POD 局部视图（无 ctor/方法），不发射符号；sizeof/偏移断言见下。
struct STGuildSkillEntry5
{
    int m_skillIdx;             // +0x00
    char m_level;               // +0x04
} __attribute__((packed));
struct STGuildSkillSection
{
    char m_pad[5];              // +0x00..+0x04（Section 起点 +0x40）
    STGuildSkillEntry5 m_entries[16];   // +0x05..+0x54
} __attribute__((packed));
struct STGuildDBTailView
{
    char m_pad[0x40];
    STGuildSkillSection m_section;
} __attribute__((packed));
static_assert(sizeof(STGuildSkillEntry5) == 5, "STGuildSkillEntry5");
static_assert(sizeof(STGuildDBTailView) == 0x40 + 5 + 0x50, "STGuildDBTailView");

void SkillSlot::set_skill_slot(char* in1, char* in2, int size)
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    memcpy(m_skillSlot, in1, size);
    memcpy(m_skillSlot2, in2, size);
}

// ---- 依赖的全局函数（真实定义在本 TU，ORIG 符号）----
// ORIG T 0x8604fe2：创建角色时按 job 追加默认技能。
// 逻辑（对照 0x8604fe2 反汇编）：
//   job ∉ [0,10] → 直接返回；从 G_CDataManager()->m_characList + job*0x7dc
//   + 0x240 拷贝该职业默认技能表（vector<pair<int,int>>，pair = 技能idx/等级）；
//   逐条 find_skill(job, idx)；CSkill 为主动技能时优先写 [0,6) 空槽，
//   否则（含主动技能无空槽）按 get_group() 落入对应区间
//   （g0:[6,0x36) g1:[0x36,0x66) g2:[0x66,0x96) g3:[0x96,0xc6)），
//   在区间内写第一个空槽；其余 group 值打 cMyTrace 日志。
void addSkillOnCreateCharacter(_Mastered_skill* skill, int job)
{
    // [b28d-solo-SkillSlot] 修法（AE）：ORIG 0x08604fe2 形 = 两条独立守卫
    // (`cmpl $0xa; jg` / `cmpl $0; js`，P63 中继 nop/jmp)、按值 vector 拷贝
    // (C1ERKS3_ + D1Ev + EH cleanup，非引用)、size()+operator[] 索引遍历
    // (P175/P157，逐字段重发 op[])、foundEmpty 声明于循环体首、内层扫描
    // `for (j = start; j < end; ++j)`（与 end 槽比较，非立即数 6）、
    // default 分支重调 s->get_group()（P48 双臂独立调用）。
    if (job > 10)
    {
        return;
    }
    if (job < 0)
    {
        return;
    }
    std::vector<std::pair<int, int> > skillList =
        *(std::vector<std::pair<int, int> >*)(
            (char*)G_CDataManager()->m_characList + job * 0x7dc + 0x240);
    int skillIdx;
    int level;
    for (int i = 0; i < (int)skillList.size(); ++i)
    {
        bool foundEmpty = false;
        skillIdx = skillList[i].first;
        level = skillList[i].second;
        CSkill* s = G_CDataManager()->find_skill(job, skillIdx);
        if (s == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        if (s->is_active_skill())
        {
            start = 0;
            end = 6;
            for (int j = start; j < end; ++j)
            {
                if (skill[j].m_index == 0)
                {
                    foundEmpty = true;
                    break;
                }
            }
        }
        if (!foundEmpty)
        {
            switch (s->get_group())
            {
            case 0:
                start = 6;
                end = 0x36;
                break;
            case 1:
                start = 0x36;
                end = 0x66;
                break;
            case 2:
                start = 0x66;
                end = 0x96;
                break;
            case 3:
                start = 0x96;
                end = 0xc6;
                break;
            default:
                cMyTrace(__PRETTY_FUNCTION__, 0x549, 5)(
                    "add skill no group %d", s->get_group());
                break;
            }
        }
        for (int k = start; k < end; ++k)
        {
            if (skill[k].m_index == 0)
            {
                skill[k].m_index = (unsigned char)skillIdx;
                skill[k].m_level = (char)level;
                break;
            }
        }
    }
}

// [拆分] int getSkillChecksum → Game/SkillSlot/getSkillChecksum.cpp

// [拆分] int finishSkillChecksum → Game/SkillSlot/finishSkillChecksum.cpp

// [拆分] int getSkillSpendSPOnSteal → Game/SkillSlot/getSkillSpendSPOnSteal.cpp

// [拆分] _Mastered_skill::_Mastered_skill → Game/SkillSlot/_Mastered_skill_ctor.cpp

// [拆分] stSPReturnInfo_t::stSPReturnInfo_t → Game/SkillSlot/stSPReturnInfo_t_ctor.cpp

// [拆分] SkillSlot::SkillSlot → Game/SkillSlot/_Mastered_skill_ctor.cpp

// [拆分] SkillSlot::~SkillSlot → Game/SkillSlot/SkillSlot_dtor.cpp

// [拆分] void SkillSlot::set_parent → Game/SkillSlot/set_parent.cpp

// [拆分] void SkillSlot::set_remain_sp → Game/SkillSlot/set_remain_sp.cpp

// [拆分] void SkillSlot::get_remain_sp → Game/SkillSlot/get_remain_sp.cpp

// [拆分] int SkillSlot::FindSkill → Game/SkillSlot/FindSkill_kind.cpp

// [拆分] _Mastered_skill* SkillSlot::get_skillslot_buf → Game/SkillSlot/get_skillslot_buf.cpp

// [拆分] int* SkillSlot::get_cur_remain_sp → Game/SkillSlot/get_cur_remain_sp.cpp

// [拆分] int* SkillSlot::get_temp_remain_sp → Game/SkillSlot/get_temp_remain_sp.cpp

// [拆分] int* SkillSlot::get_temp_remain_sfp → Game/SkillSlot/get_temp_remain_sfp.cpp

// [拆分] _Mastered_skill* SkillSlot::get_temp_skill_slot → Game/SkillSlot/get_temp_skill_slot.cpp

// [拆分] int SkillSlot::GetSkillTreeIndex → Game/SkillSlot/GetSkillTreeIndex.cpp

// [拆分] bool SkillSlot::IsLoadStrikerSkill → Game/SkillSlot/IsLoadStrikerSkill.cpp

// [拆分] void SkillSlot::SetLoadStrikerSkill → Game/SkillSlot/SetLoadStrikerSkill.cpp

// [拆分] char SkillSlot::IsEmptySkillTree → Game/SkillSlot/get_skillslot_buf.cpp

// [拆分] int SkillSlot::get_skill_count → Game/SkillSlot/get_skill_count.cpp

// [拆分] unsigned int SkillSlot::get_skill_index → Game/SkillSlot/get_skill_index.cpp

// [拆分] int SkillSlot::get_skill_level → Game/SkillSlot/get_skill_level.cpp

// [拆分] int SkillSlot::get_skillslot_group → Game/SkillSlot/get_skillslot_group.cpp

// [拆分] int SkillSlot::getSkillSlotIndex → Game/SkillSlot/getSkillSlotIndex.cpp

// [拆分] int SkillSlot::get_remain_sp_at_index → Game/SkillSlot/get_remain_sp_at_index.cpp

// [拆分] int SkillSlot::get_remain_sfp_at_index → Game/SkillSlot/get_remain_sfp_at_index.cpp

// [拆分] void SkillSlot::set_remain_sp_at_index → Game/SkillSlot/set_remain_sp_at_index.cpp

// [拆分] void SkillSlot::set_remain_sfp_at_index → Game/SkillSlot/set_remain_sfp_at_index.cpp

// [拆分] void SkillSlot::get_skill_slot → Game/SkillSlot/get_skill_slot.cpp

// [拆分] void SkillSlot::get_skill_slot_at_index → Game/SkillSlot/get_skill_slot_at_index.cpp

// [拆分] void SkillSlot::set_skill_slot_at_index → Game/SkillSlot/set_skill_slot_at_index.cpp

// [拆分] int SkillSlot::FindSkill → Game/SkillSlot/FindSkill.cpp

// [拆分] bool SkillSlot::check_skill_mastered → Game/SkillSlot/check_skill_mastered.cpp

// [拆分] bool SkillSlot::check_skill_mastered → Game/SkillSlot/check_skill_mastered_kind.cpp

// [拆分] int SkillSlot::get_skillslot_no → Game/SkillSlot/get_skillslot_no_buf.cpp

// [拆分] int SkillSlot::check_skill_mastered → Game/SkillSlot/get_skillslot_no_buf.cpp

// [拆分] int SkillSlot::get_skillslot_no → Game/SkillSlot/get_skillslot_no.cpp

// [拆分] bool SkillSlot::checkComboSkillInsertQuickSlot → Game/SkillSlot/checkComboSkillInsertQuickSlot.cpp

// [拆分] bool SkillSlot::checkMoveComboSkillSlot → Game/SkillSlot/checkMoveComboSkillSlot.cpp

// [拆分] int SkillSlot::insert_skill → Game/SkillSlot/insert_skill.cpp

// [拆分] int SkillSlot::insert_skill → Game/SkillSlot/get_skillslot_no_buf.cpp

// [拆分] int SkillSlot::insert_skill_with_slot_index → Game/SkillSlot/insert_skill_with_slot_index.cpp

// [拆分] int SkillSlot::delete_skill → Game/SkillSlot/delete_skill.cpp

// [拆分] int SkillSlot::clear_all_skills → Game/SkillSlot/clear_all_skills.cpp

// [拆分] int SkillSlot::clear_all_skills_both → Game/SkillSlot/clear_all_skills_both.cpp

// [拆分] bool SkillSlot::clear_sfp_skills → Game/SkillSlot/clear_sfp_skills.cpp

// [拆分] bool SkillSlot::backupSkill → Game/SkillSlot/backupSkill.cpp

// [拆分] void SkillSlot::initialize_sfp_skill → Game/SkillSlot/initialize_sfp_skill.cpp

// [拆分] int SkillSlot::makeCheckSum → Game/SkillSlot/makeCheckSum.cpp

int SkillSlot::growtype_skill(int job, int skillIdx, int level,
                              ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0x20b, "G_CDataManager()->find_skill( %d, %d )", job, skillIdx);
        return -1;
    }
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    int slot = 0;
    if (level == 0)
    {
        slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot != -1)
        {
            buf[slot].m_index = 0;
            buf[slot].m_level = (char)level;
        }
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillDel(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           ((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, 0, (eSkillDelReason)0);
        }
    }
    else
    {
        slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot != -1)
        {
            buf[slot].m_level = (char)level;
        }
        else
        {
            slot = insert_skill(skillIdx, group, skill->is_active_skill(),
                                kind, 1);
            if (slot > -1)
            {
                buf[slot].m_level = (char)level;
            }
        }
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           ((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, level, (eSkillAddReason)1);
        }
    }
    return slot;
}

// [拆分] int SkillSlot::skill_move → Game/SkillSlot/skill_move.cpp

// [拆分] bool SkillSlot::check_skill_list → Game/SkillSlot/check_skill_list.cpp

// [拆分] void SkillSlot::calcReturnSpSfpPerOneSkill → Game/SkillSlot/calcReturnSpSfpPerOneSkill.cpp

void SkillSlot::calcUsedSP(const _Mastered_skill* buf, int& sp, int& sfp) const
{
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；523 vs 556，gap=+33）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `__gnu_cxx::operator==<pair const*, vector<pair>>` EXTRA（迭代器比较形，
    // P96/P220 非 const iterator + 正极性 `!=` 待核）。
    // 残余为跨版本槽/寄存器族。
    // [round5-A17 2026-09-10] 两处**真实调用点差**已修（fresh 复验）：
    //   ① `eit` 段原写「预求值 eend + `==`」⇒ p167 EXTRA `__gnu_cxx::eq<const pair*,
    //      vector>`（ORIG 为混合 `ne<const pair*, pair*>`）⇒ 改回
    //      `while (eit != …end())`（end() 走非 const ⇒ iterator，与 const_iterator 混合）；
    //   ② 函数头 `CCharacter* character = …get_charac_job()…` 具名缓存 + 两分支内二次
    //      调用使 `get_charac_job` 比 ORIG **多 1 次**（p167 EXTRA）⇒ 删具名 cache、
    //      两分支 receiver 内联。
    // 读数：552/528 → **520 vs 535（gap=-15）**、ndiff 728→619；p167 calls 77=77
    // CLEAR（余 C1↔C2/D1↔D2 同址别名，nm 实证 ORIG 0x081349d6 / 0x081349ea）。
    int expertJobLevel = 0;
    int expertSkillCount = 0;
    int expertScript = 0;
    if (((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType() != 0)
    {
        int exp =
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobExp();
        expertJobLevel = m_parent->GetCurExpertJobLevel(exp);
    }
    CDataManager* dm = G_CDataManager();
    std::vector<std::pair<int, int> > giveList(
        *(std::vector<std::pair<int, int> >*)((char*)dm->m_characList +
                                              ((CUserCharacInfo*)m_parent)
                                                      ->get_charac_job() *
                                                  0x7dc +
                                              0x240));
    std::vector<std::pair<int, int> > learnList;
    // 2026-09-10（D06 消费侧配对 bool 化，A17 spec）：CCharacter::get_give_skill
    //   定义侧已 char→bool ⇒ 本处 result 同步 bool（ORIG 0x086056xx 为
    //   `mov %al,-0x35; movzbl -0x35; xor $1; test; je` = bool 负极性物化）。
    bool result = true;
    char secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
    char firstGrow = 0;
    if (secondGrow == 0)
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        dm = G_CDataManager();
        result = ((CCharacter*)((char*)dm->m_characList +
                                ((CUserCharacInfo*)m_parent)
                                    ->get_charac_job() *
                                    0x7dc))
                     ->get_give_skill((int)firstGrow, (int)secondGrow,
                                      learnList, 1);
    }
    else
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        dm = G_CDataManager();
        result = ((CCharacter*)((char*)dm->m_characList +
                                ((CUserCharacInfo*)m_parent)
                                    ->get_charac_job() *
                                    0x7dc))
                     ->get_give_skill((int)firstGrow, (int)secondGrow,
                                      learnList, 0);
    }
    // 2026-09-10 试形留痕（A17 极性直证 + 我方实测）：按 ORIG 0x0860575d
    //   `movzbl -0x35; xor $1; test; je <主体>` 改早退形（`if (!result) return;`
    //   + 主体裸块）⇒ ours 538→**544**（劣 6 条，ORIG 523，gap −15→−21）⇒ 已回滚。
    //   bool 配对化（`bool result`，D06 消费侧）保留：四数 64/184/0/11 不降。
    if (result)
    {
        std::vector<std::pair<int, int> >::const_iterator git =
            learnList.begin();
        std::vector<std::pair<int, int> >::const_iterator gend =
            learnList.end();
        while (git != gend)
        {
            std::vector<std::pair<int, int> >::iterator lit =
                giveList.begin();
            std::vector<std::pair<int, int> >::iterator lend =
                giveList.end();
            while (lit != lend)
            {
                if (lit->first == git->first)
                {
                    lit->second = std::max(lit->second, git->second);
                    break;
                }
                ++lit;
            }
            if (lit == lend)
            {
                giveList.push_back(std::make_pair(git->first, git->second));
            }
            ++git;
        }
        GameWorld* world = G_GameWorld();
        if (world->IsPvPSkilTreeChannel() != 0)
        {
            giveList.clear();
            secondGrow =
                ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
            char firstGrow =
                ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
            int job = ((CUserCharacInfo*)m_parent)->get_charac_job();
            dm = G_CDataManager();
            (&dm->m_pvpSkillTree)
                ->getGiveSkill(job, (int)firstGrow, (int)secondGrow,
                               giveList);
        }
        for (int i = 0; i < 204; ++i)
        {
            if (buf[i].m_index != 0)
            {
                CSkill* skill = 0;
                int job = ((CUserCharacInfo*)m_parent)->get_charac_job();
                skill = G_CDataManager()->find_skill(job,
                                                     (unsigned char)buf[i].m_index);
                if (skill == 0)
                {
                    break;
                }
                bool isSteal = false;
                int charLevel =
                    ((CUserCharacInfo*)m_parent)->get_charac_level();
                secondGrow =
                    ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
                firstGrow =
                    ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
                if (skill->IsStealable((int)firstGrow, (int)secondGrow) != 0)
                {
                    isSteal = true;
                    charLevel = charLevel - skill->GetStealLevelPenalty();
                }
                charLevel = charLevel +
                            (unsigned short)m_parent->GetPremiumInfo()
                                ->getOverSkillLevel();
                int giveLevel = 0;
                std::vector<std::pair<int, int> >::const_iterator lit =
                    giveList.begin();
                std::vector<std::pair<int, int> >::const_iterator lend =
                    giveList.end();
                while (lit != lend)
                {
                    if (lit->first == (unsigned char)buf[i].m_index)
                    {
                        giveLevel = lit->second;
                        break;
                    }
                    ++lit;
                }
                int curLevel = 0;
                if (giveLevel != 0)
                {
                    curLevel = giveLevel;
                    if (giveLevel < 0)
                    {
                        curLevel = 0;
                    }
                    if (buf[i].m_level <= curLevel)
                    {
                        continue;
                    }
                }
                if (expertScript != 0)
                {
                    bool found = false;
                    std::vector<std::pair<int, int> >::const_iterator eit =
                        (*(std::vector<std::pair<int, int> >*)
                             ((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                            .begin();
                    while (eit !=
                           (*(std::vector<std::pair<int, int> >*)
                                ((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                               .end())
                    {
                        if (eit->first == (unsigned char)buf[i].m_index)
                        {
                            if (buf[i].m_level == expertJobLevel)
                            {
                                expertSkillCount = expertSkillCount + 1;
                            }
                            found = true;
                            break;
                        }
                        ++eit;
                    }
                    if (found)
                    {
                        continue;
                    }
                }
                int retSP = 0;
                int retSFP = 0;
                calcReturnSpSfpPerOneSkill(retSP, retSFP, curLevel, buf, i,
                                           skill, isSteal);
                sp = sp + retSP;
                sfp = sfp + retSFP;
            }
        }
    }
}

int SkillSlot::get_give_skill(
    int job, int firstGrow, int secondGrow,
    std::vector<std::pair<int, int> >& out)
{
    if (job < 0)
    {
        return -1;
    }
    // [b28d-solo-SkillSlot] 修法：ORIG 0x0860714a 形 = 正极性 `if (IsPvP())`
    // （`call; test %al,%al; je <普通分支>`，P292 bool callee 直判、无 xor）
    // ⇒ PvP 分支 fall-through 在前、普通分支在后；去 dm 具名缓存（每次内联
    // G_CDataManager()，P183）；result 局部 bool + `if (!result)`（P158 xor 物化）；
    // 两处 get_give_skill 第 2 实参传形参 secondGrow（非字面量 0）；
    // 迭代器 for + 循环后 eq 判尾（P7），禁用 while(true)+break。
    // [b28d-solo-SkillSlot] 判词（N18a/P168a 封顶，231=231，p167 CLEAR 39=39）：
    // 残余仅 2 类纯寄存器/物化形：① m_characList 装载 4.4 发
    // `mov 0x14(%eax),%eax; mov %eax,%edx`（ORIG 单条 `mov 0x14(%eax),%edx`，
    // 2 处；指针算术/CCharacter*+job/数组下标三形均同）；② result!=1 检查
    // ORIG 4.1 物化 `movzbl; xor $1; test %al; je`，4.4 对 char 局部恒
    // `cmpb $1; jne`（bool 化则存储段多 setne，两段不可兼得，P168a④/N96）。
    // 语义等价（get_give_skill 返回 char 无归一化，ORIG 0x08348838 movzbl 直返）。
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        out.clear();
        (&G_CDataManager()->m_pvpSkillTree)
            ->getGiveSkill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (int)((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
                (int)((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType(),
                out);
        return 1;
    }
    out = *(std::vector<std::pair<int, int> >*)(
        (char*)G_CDataManager()->m_characList + job * 0x7dc + 0x240);
    std::vector<std::pair<int, int> > learnList;
    // 同上（D06 消费侧配对 bool 化）。
    bool result = true;
    if (secondGrow == 0)
    {
        result = (((CCharacter*)G_CDataManager()->m_characList) + job)
                     ->get_give_skill(firstGrow, secondGrow, learnList, 1);
    }
    else
    {
        result = (((CCharacter*)G_CDataManager()->m_characList) + job)
                     ->get_give_skill(firstGrow, secondGrow, learnList, 0);
    }
    if (!result)
    {
        return -1;
    }
    for (std::vector<std::pair<int, int> >::const_iterator git =
             learnList.begin(),
         gend = learnList.end();
         git != gend; ++git)
    {
        std::vector<std::pair<int, int> >::iterator lit = out.begin();
        std::vector<std::pair<int, int> >::iterator lend = out.end();
        for (; lit != lend; ++lit)
        {
            if (lit->first == git->first)
            {
                lit->second = std::max(lit->second, git->second);
                break;
            }
        }
        if (lit == lend)
        {
            out.push_back(std::make_pair(git->first, git->second));
        }
    }
    return 1;
}

int SkillSlot::master_new_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow, int addLevel)
{
    // [round1-L19 2026-09-09] 复修（P0/P183/P248 判据）：删 `UserQuest* quest` 与
    // `GameWorld* world` 两个具名中间局部（ORIG Ghidra `this_00`/`this_02` 为寄存器
    // 无槽）⇒ 读数 464/462 gap=2 → **464/458 gap=6**、ndiff 584→562；
    // 剩余 = ORIG 0x08603bb6 `xor $1;test %al,%al;jmp`（isClearQuest 死 bool 物化，
    // P168a③ 跨版本族）+ preSkill 迭代器段槽差 0x10 + 二维索引中转（N18a）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；464 vs 472，gap=+8）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `_List_const_iterator<_pre_skill>::operator==` EXTRA（同 try_master_new_skills，
    // 迭代器比较形待改 P7 形）。其余为跨版本槽/寄存器/求值序族（同族实证）。
    // 后续重开时优先：preSkill 循环改 `for(it=begin();it!=end();it++)`。
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (m_parent == 0)
    {
        return -4;
    }
    int slot = get_skillslot_no(
        skillIdx, group,
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex(),
        1);
    int curLevel = 0;
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -4;
    }
    int targetLevel;
    if (slot != -1)
    {
        curLevel = buf[slot].m_level;
        targetLevel = buf[slot].m_level + addLevel;
    }
    else
    {
        curLevel = 0;
        targetLevel = addLevel;
    }
    char isSteal = 0;
    if (skill->IsStealable(firstGrow, secondGrow) != 0)
    {
        m_parent->getCurCharacQuestR()->isClearQuest(0x163);
        if (m_parent->is_clear_stealingSkillMission() == 0)
        {
            return -1;
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
    }
    unsigned short overLevel =
        m_parent->GetPremiumInfo()->getOverSkillLevel();
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0)
    {
        if (skill->can_learn(level + overLevel, firstGrow, secondGrow,
                             targetLevel) != 1)
        {
            return -3;
        }
    }
    else
    {
        if (((CUserCharacInfo*)m_parent)->get_pvp_grade() <
            skill->m_pvpSkillGrade)
        {
            return -3;
        }
        if (skill->can_learn(0x46, firstGrow, secondGrow, targetLevel) != 1)
        {
            return -3;
        }
    }
    int fitness = skill->get_second_growtype_fitness(firstGrow, secondGrow);
    if (fitness == 0)
    {
        fitness = skill->get_fitness(firstGrow);
    }
    int needSP = skill->get_need_sp_sfp_for_level(curLevel, targetLevel);
    if (needSP < 0)
    {
        return -4;
    }
    if (0 < fitness)
    {
        needSP = ((100 - fitness) * needSP) / 100;
    }
    if (isSteal != 0)
    {
        needSP = getSkillSpendSPOnSteal(needSP, skill->GetStealSPPenalty());
    }
    if (needSP < 0)
    {
        needSP = 0;
    }
    int* remain;
    if (skill->IsSpecialSkill() == 0)
    {
        remain = get_cur_remain_sp(
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex());
    }
    else
    {
        if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
            ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
        {
            remain = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)2);
        }
        else
        {
            remain = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)3);
        }
    }
    if (remain == 0)
    {
        return -1;
    }
    if (*remain < needSP)
    {
        return -2;
    }
    std::list<_pre_skill>::const_iterator it;
    const std::list<_pre_skill>& preSkillList = skill->m_preSkillList;
    it = preSkillList.begin();
    while (it != preSkillList.end())
    {
        _pre_skill preSkill = *it;
        if (check_skill_mastered(preSkill.m_index, preSkill.m_jobType,
                                 preSkill.m_group) != 1)
        {
            return -3;
        }
        it++;
    }
    if (slot == -1)
    {
        *remain = *remain - needSP;
        slot = insert_skill(
            skillIdx, group, skill->is_active_skill(),
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex(),
            addLevel);
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, addLevel,
                           (eSkillAddReason)0);
            (&m_parent->m_historyLog)
                ->SPSub(
                    (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                        ->GetCurCharacSkillTreeIndex(),
                    m_remainSP1, needSP, (eSPSubReason)0);
        }
    }
    else
    {
        buf[slot].m_level = (char)targetLevel;
        *remain = *remain - needSP;
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, targetLevel,
                           (eSkillAddReason)0);
            (&m_parent->m_historyLog)
                ->SPSub(
                    (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                        ->GetCurCharacSkillTreeIndex(),
                    *remain, needSP, (eSPSubReason)0);
        }
    }
    return slot;
}

int SkillSlot::try_master_new_skills(int skillIdx, int level, int job,
                                     int firstGrow, int secondGrow,
                                     int addLevel)
{
    // [round1-L19 2026-09-09] 复修（同 master_new_skill）：删 quest/world 具名局部
    // ⇒ 339/337 gap=2 → **339/333 gap=6**、ndiff 452→376；剩余同族（N18a 索引 +
    // list 迭代器槽差 + P168a③ bool 物化）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶；339 vs 347，gap=+8）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：结构与 ORIG
    // 同族（preSkillList const_iterator + `it++` 后置、_pre_skill 逐字段、
    // can_learn/fitness/needSP 段），p167 残差 1 处
    // `_List_const_iterator<_pre_skill>::operator==` EXTRA ⇒ 迭代器比较写法
    // 与 ORIG 差一处（ORIG 用 ne 判续 + 循环后 eq，见 P7；本件为 list）。
    // 残余主体为跨版本槽/寄存器分配（同族已由 get_give_skill/InitSkill 实证）。
    // 后续重开时优先：`for (it = begin(); it != end(); it++)` 形 + P7 双比较。
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (m_parent == 0)
    {
        return -4;
    }
    int slot = get_skillslot_no(m_tempSkillSlot, skillIdx, group, true);
    int curLevel;
    int targetLevel;
    if (slot == -1)
    {
        curLevel = 0;
        targetLevel = addLevel;
    }
    else
    {
        curLevel = m_tempSkillSlot[slot].m_level;
        targetLevel = m_tempSkillSlot[slot].m_level + addLevel;
    }
    char isSteal = 0;
    if (skill->IsStealable(firstGrow, secondGrow) != 0)
    {
        m_parent->getCurCharacQuestR()->isClearQuest(0x163);
        if (m_parent->is_clear_stealingSkillMission() == 0)
        {
            return -1;
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
    }
    unsigned short overLevel =
        m_parent->GetPremiumInfo()->getOverSkillLevel();
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0)
    {
        if (skill->can_learn(level + overLevel, firstGrow, secondGrow,
                             targetLevel) != 1)
        {
            return -3;
        }
    }
    else
    {
        if (((CUserCharacInfo*)m_parent)->get_pvp_grade() <
            skill->m_pvpSkillGrade)
        {
            return -3;
        }
        if (skill->can_learn(0x46, firstGrow, secondGrow, targetLevel) != 1)
        {
            return -3;
        }
    }
    int fitness = skill->get_second_growtype_fitness(firstGrow, secondGrow);
    if (fitness == 0)
    {
        fitness = skill->get_fitness(firstGrow);
    }
    int needSP = skill->get_need_sp_sfp_for_level(curLevel, targetLevel);
    if (needSP < 0)
    {
        return -4;
    }
    if (0 < fitness)
    {
        needSP = ((100 - fitness) * needSP) / 100;
    }
    if (isSteal != 0)
    {
        needSP = getSkillSpendSPOnSteal(needSP, skill->GetStealSPPenalty());
    }
    if (needSP < 0)
    {
        needSP = 0;
    }
    int* remain;
    if (skill->IsSpecialSkill() == 0)
    {
        remain = get_temp_remain_sp();
    }
    else
    {
        remain = get_temp_remain_sfp();
    }
    if (remain == 0)
    {
        return -1;
    }
    if (*remain < needSP)
    {
        return -2;
    }
    std::list<_pre_skill>::const_iterator it;
    const std::list<_pre_skill>& preSkillList = skill->m_preSkillList;
    it = preSkillList.begin();
    while (it != preSkillList.end())
    {
        _pre_skill preSkill = *it;
        if (check_skill_mastered(m_tempSkillSlot, preSkill.m_index,
                                 preSkill.m_jobType, preSkill.m_group) != 1)
        {
            return -3;
        }
        it++;
    }
    if (slot == -1)
    {
        *remain = *remain - needSP;
        slot = insert_skill(m_tempSkillSlot, skillIdx, group,
                            skill->is_active_skill() != 0, addLevel);
    }
    else
    {
        m_tempSkillSlot[slot].m_level = (char)targetLevel;
        *remain = *remain - needSP;
    }
    return slot;
}

int SkillSlot::master_new_guild_skill(int skillIdx, int job, int type,
                                      int secondGrow,
                                      const STGuildDBInfoOnly& guildInfo,
                                      int money, int& needGold,
                                      int& learnCnt, short& needGSP,
                                      int availableGSP)
{
    // [b28d-solo-SkillSlot] 判词（N7/P75 封顶，gap=+2，p167 CLEAR 14=14）：
    // 已复现：帧 0x40、槽集(skill -0x1c/count -0x18/i -0x10/maxLevel -0xc)、
    // skill 预清(P198①)、m_skills 组件视图(P161：lea base+i*5; add $0x40;
    // mov 0x5 / add $0x49)、needLevel 具名比较(jge 直跳)、SkillAdd 全内联
    // 实参(esi/ebx/ecx 直传 + lea receiver)。残余 3 处纯寄存器/装载序：
    // ① logFormat 段 learnCnt 落 %eax vs ORIG %edx（guildInfo 指针重载时机）；
    // ② m_skills 两处 base(%ecx) 先于 i(%edx) 装载（ORIG 反之，P23/P75）；
    // ③ needGSP 存储 4.4 多 `mov %eax,%edx` 中转（ORIG 直接 `mov %ax,(%edx)`）。
    // 三者语义等价，属 4.1/4.4 寄存器分配族，无源码旋钮可择。
    // 真语义 bug 修正：ORIG 0x086039ac 的 GSP 检查用
    // 0x30(%ebp)（第 10 形参）；0x20(%ebp)（第 6 形参 money）全函数未被使用
    // ⇒ 原码参数序为 (…, guildInfo, money, needGold, learnCnt, needGSP,
    // availableGSP)。旧实现把第 6 参当 availableGSP 比较，是**取错参数**。
    (void)money;
    if ((unsigned char)guildInfo.m_skillLearnCnt >= 0x10)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0x102, "BUY GUILD SKILL, GName(%s) , Learn Cnt(%d)", &guildInfo,
            (unsigned char)guildInfo.m_skillLearnCnt);
        return -2;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    learnCnt = 1;
    int count = (unsigned char)guildInfo.m_skillLearnCnt;
    // ORIG 0x0860387e..0x86038a6：i*5 步长（STGuildSkill packed 5B）；
    // 禁缓存 m_skills 指针（P183）：ORIG 每次重读 guildInfo 基址并折
    // `lea base+i*5; add $0x40; mov 0x5(...)` / `add $0x49`。
    for (int i = 0; i < count; ++i)
    {
        if (((const STGuildDBTailView&)guildInfo)
                .m_section.m_entries[i].m_skillIdx == skillIdx)
        {
            learnCnt = (char)((const STGuildDBTailView&)guildInfo)
                            .m_section.m_entries[i].m_level + 1;
            break;
        }
    }
    if (secondGrow < 1)
    {
        if (skill->get_type_max_level(type) < learnCnt)
        {
            return -3;
        }
    }
    else
    {
        int maxLevel =
            skill->get_second_growtype_max_level(type, secondGrow);
        if (maxLevel == 0)
        {
            maxLevel = skill->get_type_max_level(type);
        }
        if (maxLevel < learnCnt)
        {
            return -3;
        }
    }
    needGold = skill->get_guildSkillNeedGold(learnCnt);
    if (needGold < 0 ||
        guildInfo.m_fundMoney < (unsigned int)needGold)
    {
        return -5;
    }
    // [round1-L19 2026-09-09] P248 档①（callee 返回类型侧）：ORIG 该 store 段为
    // `mov 0x2c(%ebp),%edx; mov %ax,(%edx)`（LHS-first 直存 16 位）⇒ 原码 RHS 与
    // short 目标**同型**；我方 `CSkill::get_guildSkillNeedGSP` 声明 int
    //（CSkill.h:102）⇒ 4.4 产 `mov %eax,%edx; mov 0x2c(%ebp),%eax; mov %dx,(%eax)`
    // 中转。调用点 `(short)` 收窄后该段逐条归位（ndiff 15 → 15，内容置换：残余
    // 转为其后两处实参装载序 + `movzbl %al,%edx` 寄存器选择）。根治需 CSkill.h:102
    // int→short 并同步 CSkill.cpp 定义（跨 TU，未落）。
    // [round5-A17 2026-09-10] 上述「根治需 CSkill.h int→short」**已证伪**：
    // ORIG 本体 `_ZNK6CSkill21get_guildSkillNeedGSPEi` @0x083500f2 尾部为
    // `mov (%eax),%eax; leave; ret`（32 位单段读、无 movzwl 截断）⇒ callee 确为 int，
    // 改 short 会使本体反落 DIFF。本段 `mov 0x2c(%ebp),%edx; mov %ax,(%edx)` 与
    // 4.4 `mov %eax,%edx; mov 0x2c(%ebp),%eax; mov %dx,(%eax)` 之差属 P211
    // 异型 store（short& 目标 + int RHS）跨版本寄存器族 ⇒ N 封顶，勿再改 CSkill。
    needGSP = (short)skill->get_guildSkillNeedGSP(learnCnt);
    if (needGSP < 0 || availableGSP < needGSP)
    {
        return -2;
    }
    int needLevel = skill->get_needGuildLevel(learnCnt);
    if ((int)(unsigned char)guildInfo.m_fundLevel < needLevel)
    {
        return -6;
    }
    if (m_parent != 0)
    {
        (&m_parent->m_historyLog)
            ->SkillAdd(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                skillIdx, learnCnt, (eSkillAddReason)0);
        m_parent->makeGuildSkillMessage(skill->getSkillName(), learnCnt);
    }
    return 0;
}

int SkillSlot::refund_skill(int skillIdx, int level, int job, int firstGrow,
                            int secondGrow, int refundCount, int& result)
{
    // [round1-L19 2026-09-09] 复修（判据 P0/P183/P329 + 判据⑨）：
    //   ① 删 8 处「具名中间局部」（treeKind/treeKind2/treeKind3/treeKind4/
    //      characJob/characJob2/newRemain/giveResult/characNo/accId/str）——
    //      ORIG Ghidra C 用寄存器 uVar3/uVar8/iVar9（无槽）承载，P0 判据；
    //   ② `for(it=begin(); it!=end(); it++)` 专家脚本循环改 `->operator[](i)`
    //      （旧用 at() ⇒ p167 EXTRA `vector::at`）；
    //   ③ 守卫改嵌套 `if (IsPvPSkilTreeChannel()!=0){ if(pvp_grade<0) fail }`，
    //      复现 ORIG `call; test %al,%al; je <body>`（4.4 对 `A==0||B` 恒插
    //      `xor $1` 物化，嵌套正极形才直测）；
    //   ④ skill 预清两句式（P197③：ORIG 0x086069e1 `movl $0x0,-0x50` + 独立赋值）。
    // 读数：549/571 gap=+22 → **547/546 gap=1**、ndiff 680→447（check_tu_routed 18:18）。
    // 残余 = ①第二项 ORIG `cmp $0;setge;test;jne` vs 4.4 `shr $0x1f;test;je`
    //（4.1 短路第二项 bool 物化，4.4 恒符号位直测，P168a③b 族）；
    // ② 全帧槽号平移 0x10（我方 group 落 -0x4c vs ORIG -0x50）+ 1 条 mov 中转。
    // ORIG 证据：asm 0x08606a4a（`test %al,%al; je 0x08606a78`）、
    // Ghidra C `local_50=CSkill::get_group; local_4c=get_skillslot_no(...)`、
    // p167 调用点差集 CLEAR。语义全等（実参/副作用序一致）⇒ N7/N82 封顶。
    result = 0x13;
    if (m_parent == 0)
    {
        return -4;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (group == 4)
    {
        result = 7;
        return -5;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
    {
        // 2026-09-10 试形留痕（B 轴 · A17 地址自校 0x8606a5b/0x8606a5e）：
        //   ORIG 为 `cmp $0x0,%eax; setge`（有符号）而我方 `seta`；加 `(int)`
        //   强转后 insns 548 与 ndiff 代理 563 **均不变** ⇒ 已回滚（A17 直证留档）。
        if (((CUserCharacInfo*)m_parent)->get_pvp_grade() < 0)
        {
            result = 7;
            return -3;
        }
    }
    {
        ENUM_SKILL_TREE_KIND kind =
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex();
        int slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot == -1)
        {
            return -1;
        }
        kind = (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                   ->GetCurCharacSkillTreeIndex();
        _Mastered_skill* buf = get_skillslot_buf(kind);
        if (buf == 0)
        {
            return -4;
        }
        int curLevel = buf[slot].m_level - refundCount;
        if (curLevel < 0)
        {
            return -1;
        }
        void* expertScript =
            G_CDataManager()->GetExpertJobScript(
                ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
        if (expertScript != 0)
        {
            // [b28d-solo-SkillSlot] P183：ORIG 每次 `mov -0x34(%ebp),%eax;
            // lea 0xc(%eax),%edx` 重读 expertScript+0xc，禁具名引用缓存。
            // 2026-09-10 试形留痕（B 轴 · A17 线索）：本循环 `unsigned`→`int`+
            //   `(int)` 强转 ⇒ ndiff 代理 563→**569（劣 6）** ⇒ 已回滚（A17 的
            //   setge/jge 直证对应的是本函数 giveList/postList 两循环，非本处）。
            for (unsigned int i = 0;
                 i < ((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                           0xc))
                         ->size();
                 ++i)
            {
                if (((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                          0xc))
                        ->operator[](i)
                        .first == skillIdx)
                {
                    result = 7;
                    return -5;
                }
            }
        }
        std::pair<int, int> pair;
        std::vector<std::pair<int, int> > giveList;
        if (get_give_skill(job, firstGrow, secondGrow, giveList) < 0)
        {
            return -1;
        }
        for (int i = 0; i < (int)giveList.size(); ++i)
        {
            pair = giveList[i];
            if (buf[slot].m_index == pair.first)
            {
                if (curLevel < pair.second)
                {
                    result = 7;
                    return -1;
                }
                break;
            }
        }
        std::vector<std::pair<int, int> > postList;
        G_CDataManager()->m_skillList->get_post_learning_skill(
            ((CUserCharacInfo*)m_parent)->get_charac_job(), skillIdx,
            postList);
        for (int i = 0; i < (int)postList.size(); ++i)
        {
            int preSkillIdx = postList[i].first;
            int preLevel = postList[i].second;
            for (int s = 0; s < 204; ++s)
            {
                if (buf[s].m_index == preSkillIdx)
                {
                    if (curLevel < preLevel)
                    {
                        result = 0x14;
                        return -1;
                    }
                    break;
                }
            }
        }
        int returnSP = skill->get_need_sp_sfp_for_level(
            curLevel, buf[slot].m_level);
        if (skill->IsStealable(firstGrow, secondGrow) != 0)
        {
            returnSP =
                getSkillSpendSPOnSteal(returnSP, skill->GetStealSPPenalty());
        }
        buf[slot].m_level = (char)(buf[slot].m_level - refundCount);
        if (buf[slot].m_level == 0)
        {
            buf[slot].m_index = 0;
        }
        if (skill->IsSpecialSkill() == 1)
        {
            char kindChar = 0;
            if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() ==
                    -1 ||
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() ==
                    0)
            {
                kindChar = 2;
            }
            else
            {
                kindChar = 3;
            }
            int remain = get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)kindChar);
            set_remain_sfp_at_index(remain + returnSP,
                                    (ENUM_SKILL_TREE_KIND)kindChar);
            (&m_parent->m_historyLog)
                ->SkillDel(
                    ((CUserCharacInfo*)m_parent)->get_charac_job(),
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    skillIdx, refundCount, (eSkillDelReason)1);
            (&m_parent->m_historyLog)
                ->SFPAdd((int)kindChar, remain + returnSP, returnSP,
                         (eSPAddReason)2);
            if (0 < refundCount && 10 < returnSP / refundCount)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x93b,
                    "Wrong refund TP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    NumberToString(m_parent->get_acc_id(), 0),
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo(), skillIdx,
                    returnSP, refundCount);
            }
        }
        else
        {
            int remain = get_remain_sp_at_index(
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex());
            set_remain_sp_at_index(
                remain + returnSP,
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex());
            (&m_parent->m_historyLog)
                ->SkillDel(
                    ((CUserCharacInfo*)m_parent)->get_charac_job(),
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    skillIdx, refundCount, (eSkillDelReason)1);
            (&m_parent->m_historyLog)
                ->SPAdd(
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    remain + returnSP, returnSP, (eSPAddReason)2);
            if (0 < refundCount && 1000 < returnSP / refundCount)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x921,
                    "Wrong refund SP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    NumberToString(m_parent->get_acc_id(), 0),
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo(), skillIdx,
                    returnSP, refundCount);
            }
        }
        result = 0;
        return slot;
    }
}

int SkillSlot::try_refund_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow,
                                int refundCount, int& result)
{
    // [round1-L19 2026-09-09] 复修（P0/P183/P329）：删 `expertList` 引用（改为
    // 每次重推导 `((vector*)((char*)expertScript+0xc))->operator[](i)`，P183/P157）、
    // `giveResult`/`characJob` 具名局部（ORIG 用 iVar9/uVar8 寄存器）⇒
    // 355/352 gap=3 → **355/350 gap=5**、ndiff 319→215。
    // 剩余 = 全帧槽号平移 0x10（我方 -0x2c.. vs ORIG -0x3c..）+ `cmp $0;setge` /
    // `js/seta` 符号性 lowering 差（P196/P209，jcc 落点已逐位复核一致）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶，355 vs 356，p167 CLEAR 36=36）：
    // 已复现：skill 预清(P198①)、G_GameWorld 内联、`get_pvp_grade() >= 0`
    // 等价形、expertScript 视图去具名引用、两处 0xcc 扫描、EH cleanup 段全等。
    // 残余 2 类跨版本差异：① 4.4 对 `(vector*)((char*)expertScript+0xc)` 的
    // 循环条件/体重复表达式发 SAVE_EXPR 提升（循环外 `add $0xc; mov %eax,-0x24`
    // 缓存槽），ORIG 4.1 每次 `mov -0x34; lea 0xc` 重算 ⇒ 全帧槽号浅 4
    // （skill -0x48 vs -0x44 等，N82 族；P183 源码侧已无缓存）；
    // ② 短路条件 bool 物化：ORIG `test %al; je` + `cmp $0; setge %al; test; jne`，
    // 4.4 恒 `xor $1; test; jne` + `js; mov$1/mov$0; test; je`（P168a③b 跨版本
    // bool 归一，无源码旋钮）。
    // 真语义 bug 修正见下方 GetExpertJobScript 段。
    result = 0x13;
    if (m_parent == 0)
    {
        return -4;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (group == 4)
    {
        result = 7;
        return -5;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0 ||
        ((CUserCharacInfo*)m_parent)->get_pvp_grade() >= 0)
    {
        int slot = get_skillslot_no(m_tempSkillSlot, skillIdx, group, true);
        if (slot == -1)
        {
            return -1;
        }
        int curLevel = m_tempSkillSlot[slot].m_level - refundCount;
        if (curLevel < 0)
        {
            return -1;
        }
        void* expertScript = 0;
        // [b28d-solo-SkillSlot] 真语义 bug 修正：ORIG 0x08607a5e 段
        // `call GetCurCharacExpertJobType; mov %eax,%ebx; call G_CDataManager;
        //  mov %ebx,0x4(%esp); call GetExpertJobScript` ⇒ 原码把职业类型
        // 作为 GetExpertJobScript 实参（旧实现丢弃返回值传 0 = 取错脚本）。
        expertScript = G_CDataManager()->GetExpertJobScript(
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
        if (expertScript != 0)
        {
            // 2026-09-10 试形留痕（B 轴）：同 refund_skill ⇒ ndiff 代理 359→365
            //   （劣 6）⇒ 已回滚；保留 giveList/postList 两循环的 int 化（那两处实测改善）。
            for (unsigned int i = 0;
                 i < ((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                           0xc))
                         ->size();
                 ++i)
            {
                if (((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                          0xc))
                        ->operator[](i)
                        .first == skillIdx)
                {
                    result = 7;
                    return -5;
                }
            }
        }
        std::pair<int, int> pair;
        std::vector<std::pair<int, int> > giveList;
        if (get_give_skill(job, firstGrow, secondGrow, giveList) < 0)
        {
            return -1;
        }
        // 2026-09-10（C05② 反向/符号性旋钮）：ORIG 0x08607b35 段为
        //   `mov -0x38(%ebp),%eax; cmp -0x10(%ebp),%eax; setg`（**有符号**序）
        //   ⇒ ORIG 索引局部是 int；我方 unsigned 产 `seta` 无符号序 ⇒ 改 int
        //   对齐（P196 同向：ORIG 本身即 setg 有符号）。
        for (int i = 0; i < (int)giveList.size(); ++i)
        {
            pair = giveList[i];
            if (m_tempSkillSlot[slot].m_index == pair.first)
            {
                if (curLevel < pair.second)
                {
                    result = 7;
                    return -1;
                }
                break;
            }
        }
        std::vector<std::pair<int, int> > postList;
        G_CDataManager()->m_skillList->get_post_learning_skill(
            ((CUserCharacInfo*)m_parent)->get_charac_job(), skillIdx,
            postList);
        // 同上（ORIG 0x08607bxx 循环条件 `cmp -0x18(%ebp),%eax; setg`）。
        for (int i = 0; i < (int)postList.size(); ++i)
        {
            int preSkillIdx = postList[i].first;
            int preLevel = postList[i].second;
            for (int s = 0; s < 204; ++s)
            {
                if (m_tempSkillSlot[s].m_index == preSkillIdx)
                {
                    if (curLevel < preLevel)
                    {
                        result = 0x14;
                        return -1;
                    }
                    break;
                }
            }
        }
        int returnSP = skill->get_need_sp_sfp_for_level(
            curLevel, m_tempSkillSlot[slot].m_level);
        if (skill->IsStealable(firstGrow, secondGrow) != 0)
        {
            returnSP =
                getSkillSpendSPOnSteal(returnSP, skill->GetStealSPPenalty());
        }
        m_tempSkillSlot[slot].m_level =
            (char)(m_tempSkillSlot[slot].m_level - refundCount);
        if (m_tempSkillSlot[slot].m_level == 0)
        {
            m_tempSkillSlot[slot].m_index = 0;
        }
        int* remain;
        if (skill->IsSpecialSkill() == 1)
        {
            remain = get_temp_remain_sfp();
        }
        else
        {
            remain = get_temp_remain_sp();
        }
        *remain = *remain + returnSP;
        result = 0;
        return slot;
    }
    result = 7;
    return -3;
}

void SkillSlot::InitSkillHistoryLog(ENUM_SKILL_TREE_KIND kind, int reason)
{
    // b28d-solo-SkillSlot 判词（N 封顶，N7/P75 族；ORIG 0x086085fa 尾段 0x08608675）：
    // ORIG 4.1 求值序 = reason→sfp→sp→level→spKind(-0x20→%edx)→receiver
    // (m_parent+0x79700→%ecx，内联无槽)；4.4 恒 this-first（receiver→%edx 在
    // spKind 之前，spKind 落 %eax），压栈尾部两条互换。三形实测：
    // ① 原形 57=57 DIFF（仅 2 条 mov 寄存器角色/序差）；
    // ② `(0, m_parent->m_historyLog)` 逗号接收者 57=57 DIFF 同形；
    // ③ `(*m_parent).m_historyLog` 解引用形 57=57 DIFF 同形；
    // ④ 具名 `cUserHistoryLog* historyLog=&…; historyLog->InitSkill(...)` 求值序
    //    翻正（this 最后装载）但 receiver 槽化 -0x1c + 槽位重排 ⇒ 59 vs 57 DIFF。
    // receiver 无槽往返（P0 ⇒ 原码无具名局部），4.4 无法在无槽下把 this 求值后置
    // ⇒ 跨版本求值序结构性差，封 N7；语义等价（全部实参/副作用序一致）。
    // [round1-L19 2026-09-09] 第 ⑤ 形复测：`(&m_parent->m_historyLog)->InitSkill(...)`
    // 与链式形逐条同形（56=56、ndiff 12 不变）⇒ 维持 N7 封顶（4.4 恒先算 receiver
    // 落 %edx、spKind 落 %eax；ORIG 0x086085fa 段为 spKind→%edx、receiver→%ecx）。
    if (m_parent == 0)
    {
        return;
    }
    int spKind;
    int sfpKind;
    if (kind == -1 || kind == 0)
    {
        spKind = 0;
        sfpKind = 2;
    }
    else
    {
        spKind = 1;
        sfpKind = 3;
    }
    m_parent->m_historyLog.InitSkill(
        spKind, ((CUserCharacInfo*)m_parent)->get_charac_level(),
        get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)spKind),
        get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)sfpKind),
        (eSkillInitReason)reason);
}

void SkillSlot::SendSkillInfo(int characIdx, ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return;
    }
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1e5);
    guard.put_byte(1);
    guard.put_byte(characIdx);
    guard.put_byte(
        (int)(char)m_parent->m_characList[characIdx].m_field26);
    guard.put_byte(
        (int)((const _Charac_info_nibble_field*)&m_parent->m_characList[characIdx])
            ->m_specialNibble);
    int index = guard.get_index();
    int count = 0;
    guard.put_byte(0);
    _Mastered_skill* buf = NULL;
    if (kind == -1 || kind == 0)
    {
        buf = m_skillSlot;
    }
    else
    {
        buf = m_skillSlot2;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index != 0)
        {
            guard.put_byte(i);
            guard.put_byte((unsigned char)buf[i].m_index);
            guard.put_byte((int)(char)buf[i].m_level);
            count = count + 1;
        }
    }
    guard.put_byte(index, count);
    guard.finalize(true);
    m_parent->Send(guard);
}

// [拆分] int SkillSlot::checkskillSlotEmptyCount → Game/SkillSlot/checkskillSlotEmptyCount.cpp

// ==== SkillSlot::checkOverLevelSkill @ 0x08605d2a（报告 docs/class_func_reports/SkillSlot/checkOverLevelSkill.md）====
// 检查超等级技能：对每槽技能计算当前等级可达到的最大技能等级（含偷学/超限加成），
// 超出部分返回 SP/SFP 并写回；返回 1 表示有技能被降级/补发。
bool SkillSlot::checkOverLevelSkill(std::vector<stSPReturnInfo_t>& out,
                                    ENUM_SKILL_TREE_KIND kind, int level)
{
    // [b28d-solo-SkillSlot] 判词（N82 封顶；608 vs 620，gap=+12）：
    // 本件在 b28d-solo 波次内未完成逐条收敛（预算封口）。已取证：p167 残差 1 处
    // `std::list<_pre_skill>::end()` EXTRA（循环外 end() 预求值形待核，P13）。
    // 残余为跨版本槽/寄存器/求值序族（大函数 ndiff 失真，见 N82；验收须走
    // §6.2 三件套）。
    if (m_parent == 0 ||
        ((CUserCharacInfo*)m_parent)->getCurCharacR() == 0)
    {
        return 0;
    }
    char result = 0;
    _Mastered_skill* buf = get_skillslot_buf(kind);
    int* remainSFP = 0;
    if (kind == 0)
    {
        remainSFP = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)2);
    }
    else if (kind == 1)
    {
        remainSFP = get_cur_remain_sp((ENUM_SKILL_TREE_KIND)3);
    }
    int* remainSP = get_cur_remain_sp(kind);
    if (buf == 0 || remainSP == 0 || remainSFP == 0)
    {
        return 0;
    }
    int expertJobLevel = 0;
    int expertSkillCount = 0;
    void* expertScript = 0;
    int i;
    CSkill* skill;
    bool isSteal;
    int checkLevel;
    int maxLevel;
    int giveLevel;
    char giveResult;
    bool found;
    unsigned int j;
    int preIndex;
    int preJobType;
    int preGroup;
    expertScript = G_CDataManager()->GetExpertJobScript(
        ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
    if (((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType() != 0)
    {
        expertJobLevel = m_parent->GetCurExpertJobLevel(
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobExp());
    }
    for (i = 0; i < 0xcc; ++i)
    {
        if (buf[i].m_index != 0)
        {
            skill = 0;
            skill = G_CDataManager()->find_skill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (unsigned char)buf[i].m_index);
            if (skill == 0)
            {
                return 0;
            }
            isSteal = false;
            checkLevel = level;
            if (skill->IsStealable(
                    (int)((CUserCharacInfo*)m_parent)
                        ->getCurCharFirstGrowType(),
                    (int)((CUserCharacInfo*)m_parent)
                        ->getCurCharSecondGrowType()) != 0)
            {
                isSteal = true;
                checkLevel = checkLevel - skill->GetStealLevelPenalty();
            }
            checkLevel = checkLevel +
                         m_parent->GetPremiumInfo()->getOverSkillLevel();
            maxLevel = skill->getMaxSkillLevel(
                checkLevel,
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharFirstGrowType(),
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharSecondGrowType());
            if (maxLevel == -1)
            {
                buf[i].m_index = 0;
                buf[i].m_level = 0;
                continue;
            }
            if (expertScript != 0)
            {
                found = false;
                // [round5-A17 2026-09-10] 推翻 [round1-L19]「oracle 视图须为 const」
                // 判词：fresh .o 的 p167 证明 ORIG 侧共 **3 处** 转换构造
                // `__normal_iterator<const pair*>::C2<pair*>`（本循环 begin 一处、
                // giveList 循环 begin/end 各一处）⇒ `.begin()` 必走**非 const**
                // 重载（`_ZNSt6vectorISt4pairIiiESaIS1_EE5beginEv`）才能产生；
                // const 视图反而多发 `_ZNKSt6vector…5beginEv/…5endEv`（p167 EXTRA）
                // 并把比较退化成 const/const ⇒ 维持非 const 成员直访问。
                std::vector<std::pair<int, int> >::const_iterator eit =
                    reinterpret_cast<STExpertJobScript*>(expertScript)
                        ->m_growSkillVec.begin();
                while (eit !=
                       reinterpret_cast<STExpertJobScript*>(expertScript)
                           ->m_growSkillVec.end())
                {
                    if (eit->first == (unsigned char)buf[i].m_index)
                    {
                        if ((char)buf[i].m_level == expertJobLevel)
                        {
                            expertSkillCount = expertSkillCount + 1;
                        }
                        found = true;
                        break;
                    }
                    ++eit;
                }
                if (found)
                {
                    continue;
                }
            }
            std::list<_pre_skill>::const_iterator pit;
            const std::list<_pre_skill>& preSkillList = skill->m_preSkillList;
            pit = preSkillList.begin();
            while (pit != preSkillList.end())
            {
                const _pre_skill& preSkill = *pit;
                preIndex = preSkill.m_index;
                preJobType = preSkill.m_jobType;
                preGroup = preSkill.m_group;
                if (check_skill_mastered(preIndex, preJobType, preGroup,
                                         kind) != 1)
                {
                    maxLevel = 0;
                    break;
                }
                pit++;
            }
            if (maxLevel < (char)buf[i].m_level)
            {
                giveLevel = 0;
                std::vector<std::pair<int, int> > giveList(
                    *(std::vector<std::pair<int, int> >*)(
                        (char*)G_CDataManager()->m_characList +
                        ((CUserCharacInfo*)m_parent)->get_charac_job() *
                            0x7dc +
                        0x240));
                giveResult = 1;
                if (((CUserCharacInfo*)m_parent)
                        ->getCurCharSecondGrowType() == 0)
                {
                    giveResult = ((CCharacter*)((char*)G_CDataManager()
                                                    ->m_characList +
                                                ((CUserCharacInfo*)m_parent)
                                                        ->get_charac_job() *
                                                    0x7dc))
                                     ->get_give_skill(
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharFirstGrowType(),
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharSecondGrowType(),
                                         giveList, 1);
                }
                else
                {
                    giveResult = ((CCharacter*)((char*)G_CDataManager()
                                                    ->m_characList +
                                                ((CUserCharacInfo*)m_parent)
                                                        ->get_charac_job() *
                                                    0x7dc))
                                     ->get_give_skill(
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharFirstGrowType(),
                                         (int)((CUserCharacInfo*)m_parent)
                                             ->getCurCharSecondGrowType(),
                                         giveList, 0);
                }
                if (giveResult == 1)
                {
                    giveLevel = 0;
                    std::vector<std::pair<int, int> >::const_iterator git =
                        giveList.begin();
                    std::vector<std::pair<int, int> >::const_iterator gitEnd =
                        giveList.end();
                    while (git != gitEnd)
                    {
                        if (git->first == (unsigned char)buf[i].m_index)
                        {
                            giveLevel = git->second;
                            break;
                        }
                        ++git;
                    }
                    maxLevel = maxLevel <= giveLevel ? giveLevel : maxLevel;
                    if (maxLevel < (char)buf[i].m_level)
                    {
                        int retSP = 0;
                        int retSFP = 0;
                        calcReturnSpSfpPerOneSkill(
                            retSP, retSFP, maxLevel, buf, i, skill,
                            isSteal);
                        out.push_back(stSPReturnInfo_t(
                            (unsigned short)(unsigned char)buf[i].m_index,
                            (unsigned short)((short)(char)buf[i].m_level -
                                             (short)maxLevel),
                            (unsigned int)retSP, (unsigned int)retSFP));
                        *remainSFP = *remainSFP + retSFP;
                        *remainSP = *remainSP + retSP;
                        buf[i].m_level = (char)maxLevel;
                        if (buf[i].m_level == 0)
                        {
                            buf[i].m_index = 0;
                        }
                        result = 1;
                    }
                }
            }
        }
    }
    if (expertScript != 0 &&
        (*(std::vector<std::pair<int, int> >*)((char*)&reinterpret_cast<STExpertJobScript*>(expertScript)->m_growSkillVec))
                .size() != expertSkillCount)
    {
        for (j = 0;
             j < (*(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                        0xc))
                     .size();
             ++j)
        {
            growtype_skill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (*(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                       0xc))[j]
                    .first,
                expertJobLevel, kind);
            result = 1;
        }
    }
    return result;
}

// [拆分] void SkillSlot::debugCheckGrowTypeSkill → Game/SkillSlot/debugCheckGrowTypeSkill.cpp

void SkillSlot::debugCommandResetSkill()
{
    memset(m_skillSlot, 0, 0x198);
    memset(m_skillSlot2, 0, 0x198);
    _Mastered_skill temp[204];
    addSkillOnCreateCharacter(
        temp, ((CUserCharacInfo*)m_parent)->get_charac_job());
    for (int i = 0; i < 204; ++i)
    {
        if (temp[i].m_index)
        {
            m_skillSlot[i].m_index = temp[i].m_index;
            m_skillSlot[i].m_level = temp[i].m_level;
            m_skillSlot2[i].m_index = temp[i].m_index;
            m_skillSlot2[i].m_level = temp[i].m_level;
        }
    }
    if (((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType() != 0)
    {
        m_parent->set_grow_type(
            ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
            ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType(), 0,
            (eChangeGrowTypeReason)2);
    }
}

// [拆分] void SkillSlot::loadComboSkill → Game/SkillSlot/loadComboSkill.cpp

void SkillSlot::getComboSkillSaveData(SIG_COMBO_SKILL* data) const
{
    if (m_parent == 0)
    {
        return;
    }
    if (data == 0)
    {
        return;
    }
    data->m_characNo = m_parent->get_charac_no(-1);
    data->m_job = ((CUserCharacInfo*)m_parent)->get_charac_job();
    if (!m_comboSkill.saveData(data))
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0xc84, "SkillSlot::getComboSkillSaveData error");
    }
}

// [拆分] void SkillSlot::getComboSkillSendInfo → Game/SkillSlot/getComboSkillSendInfo.cpp

// [拆分] void SkillSlot::setComboSkillInfo → Game/SkillSlot/setComboSkillInfo.cpp

// [拆分] void SkillSlot::resetComboSkill → Game/SkillSlot/resetComboSkill.cpp

// [拆分] void SkillSlot::verifyComboSkill → Game/SkillSlot/verifyComboSkill.cpp

// [拆分] void SkillSlot::verifyComboSkillTree → Game/SkillSlot/verifyComboSkillTree.cpp

// [拆分] void SkillSlot::eraseDuplicationComboSKill → Game/SkillSlot/eraseDuplicationComboSKill.cpp

// [拆分] void SkillSlot::eraseDuplicationComboSKillTree → Game/SkillSlot/eraseDuplicationComboSKillTree.cpp

bool SkillSlot::moveComboSkillExtensionQuickSlot(int skillIdx, int level,
                                                ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    CSkill* skill = G_CDataManager()->find_skill(
        ((CUserCharacInfo*)m_parent)->get_charac_job(), skillIdx);
    if (skill == 0)
    {
        return 0;
    }
    int group = skill->get_group();
    int slot = get_skillslot_no(skillIdx, group, kind, 0);
    if (slot != -1)
    {
        if (buf[slot].m_level < level)
        {
            buf[slot].m_level = (char)level;
        }
    }
    else
    {
        slot = insert_skill(skillIdx, group, 0, kind, 1);
        if (slot >= 6 && slot <= 0xc5)
        {
            buf[slot].m_level = (char)level;
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0xcf9,
                "moveComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                slot, (unsigned char)buf[slot].m_index,
                (int)(char)buf[slot].m_level, kind,
                m_parent->get_charac_no(-1));
        }
    }
    return 1;
}

void SkillSlot::pullComboSkillExtensionQuickSlot(ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return;
    }
    for (int i = 0xc6; i < 0xcc; ++i)
    {
        if (buf[i].m_index == 0)
        {
            continue;
        }
        if (moveComboSkillExtensionQuickSlot(
                buf[i].m_index, (int)(char)buf[i].m_level, kind))
        {
            buf[i].m_index = 0;
            buf[i].m_level = 0;
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0xcd7,
                "pullComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                i, (unsigned char)buf[i].m_index,
                (int)(char)buf[i].m_level, kind,
                m_parent->get_charac_no(-1));
        }
    }
}

// ==== SkillSlot::InitSkill @ 0x08608120（报告 docs/class_func_reports/SkillSlot/InitSkill.md）====
// PvP 树频道：givePvPSkillTree + 脚本点数 set_remain_sp_at_index(+日志)。
// 普通树：建号技能补发 → get_give_skill 表 → growtype_skill 循环 →
// ExpertJob 增益技循环 → CSkillChanger::SkillInitialize → 日志/发包。
bool SkillSlot::InitSkill(bool sendFlag, ENUM_SKILL_TREE_KIND kind)
{
    // [b28d-solo-SkillSlot] 判词（N82/P75/P168a 封顶，340 vs 338，p167 CLEAR 51=51）：
    // 已复现：kindArg/changerKind P45 短路链 + 死初值(P171)、PvP 段 secondGrow
    // 具名 int + firstGrow/job/pvpGrade 内联实参、两处 cMyTrace 全表达式临时链
    // (P227)、giveVec 循环 pair 具名局部(P50)、addSkillOnCreateCharacter 双臂。
    // 残余跨版本族：① 4.4 把内联 pvpGrade 落槽而 ORIG 留 %esi（具名 secondGrow
    // 反之），帧 0x13c vs 0x12c（+0x10，N82/P0 求值序族）；
    // ② giveVec 循环 pair 4.4 折临时槽直读（ORIG 具名槽 -0x3c/-0x38 + 拷贝）；
    // ③ `(got^1)!=0` 4.4 折 `cmpb $1`，ORIG 4.1 物化 `movzbl; xor $1; test`；
    // ④ receiver `lea 0xa800(%eax)` vs ORIG `add $0xa800,%eax`（N107 反向）。
    // 全部语义等价（调用点差集为零）。
    if (m_parent == 0)
        return false;

    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG 0x0860815d：`movl $0x0` 初值（死写，P171）+ P45 短路链
        // `(kind==-1)||(kind==0) → 1 : 2`。
        int kindArg = 0;
        if (kind == -1 || kind == 0)
            kindArg = 1;
        else
            kindArg = 2;
        m_parent->givePvPSkillTree(0, true, kindArg);
        int secondGrow = m_parent->getCurCharSecondGrowType();
        int sp = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                     m_parent->get_charac_job(),
                     m_parent->getCurCharFirstGrowType(), secondGrow,
                     m_parent->get_pvp_grade(), false);
        set_remain_sp_at_index(sp, kind);
        if (sendFlag)
        {
            m_parent->send_skill_info();
            cMyTrace(__PRETTY_FUNCTION__, 0xb47, 0)(
                "SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                m_parent->getCurCharacName());
        }
        InitSkillHistoryLog(kind, 1);
        return true;
    }

    if (kind == -1 || kind == 0)
    {
        addSkillOnCreateCharacter(&m_skillSlot[0],
                                  m_parent->get_charac_job());
    }
    else
    {
        addSkillOnCreateCharacter(&m_skillSlot2[0],
                                  m_parent->get_charac_job());
    }

    std::vector<std::pair<int, int> > giveVec;
    char secondGrow = m_parent->getCurCharSecondGrowType();
    char firstGrow = m_parent->getCurCharFirstGrowType();
    CCharacter* chars = (CCharacter*)G_CDataManager()->m_characList;
    // 同上（D06 配对 bool 化）：ORIG 0x08608xxx 段为 1 字节 bool 槽。
    bool got = chars[m_parent->get_charac_job()].get_give_skill(
        firstGrow, secondGrow, giveVec, 0);
    if (!got)
    {
        return false;
    }

    // [b28d-solo-SkillSlot] ORIG 0x0860830c 段：`pair = giveVec[i]` 具名 pair
    // （-0x3c/-0x38）+ 临时 -0x70/-0x6c（P50 先声明后赋值形）。
    std::pair<int, int> pair;
    // 2026-09-10 试形留痕（B 轴反向 · A17 地址自校 0x86084bd/0x86084c0）：
    //   ORIG 为 `call size; cmp -0x1c(%ebp),%eax; seta`（无符号）而我方 `setg`；
    //   改 unsigned 后 InitSkill insns 340 与 ndiff 代理 466 **均不变** ⇒ 已回滚。
    for (int i = 0; i < (int)giveVec.size(); ++i)
    {
        pair = giveVec[i];
        growtype_skill(m_parent->get_charac_job(), pair.first, pair.second,
                       kind);
    }

    STExpertJobScript* expert = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(
            m_parent->GetCurCharacExpertJobType()));
    if (expert != 0)
    {
        for (int i = 0; i < (int)expert->m_growSkillVec.size(); ++i)
        {
            int level =
                m_parent->GetCurExpertJobLevel(m_parent->GetCurCharacExpertJobExp());
            growtype_skill(m_parent->get_charac_job(),
                           expert->m_growSkillVec[i].first, level, kind);
        }
    }

    WongWork::CSkillChanger changer;
    // ORIG：movl $0x0 初值；kind==-1→0、kind==0→1、其他→2（同 kindArg 模式）。
    int changerKind = 0;
    if (kind == -1 || kind == 0)
        changerKind = 1;
    else
        changerKind = 2;
    changer.SkillInitialize(m_parent, changerKind, false);
    if (sendFlag)
    {
        m_parent->send_skill_info();
        cMyTrace(__PRETTY_FUNCTION__, 0xb86, 0)(
            "SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
            m_parent->getCurCharacName());
    }
    InitSkillHistoryLog(kind, 0);
    return true;
}

// [拆分] CSkillChanger::g_nStatUpItemIdx / g_nElixerAbility → Game/SkillSlot/CSkillChanger_staticData.cpp

// [拆分] CSkillChanger::CSkillChanger / ~CSkillChanger → Game/SkillSlot/CSkillChanger_ctor_dtor.cpp

namespace WongWork
{
void CSkillChanger::_ResetSkillPoint(CUser* user, ENUM_SKILL_TREE_KIND kind)
{
    // b28d-solo-SkillSlot 判词（AE 40=40）：修法 = 加法右操作数 `(int)` 收窄。
    // GetSpAtLevelUp 返回 unsigned int，与左操作数 int 异型时 4.4 发
    // `lea (%ebx,%eax,1),%eax; mov %eax,%ebx`（+1 条）；ORIG 4.1 为 `add %eax,%ebx`。
    // 调用点 `(int)G_CDataManager()->GetSpAtLevelUp(level)` 使两端同型 ⇒ 逐条 AE
    // （40=40，助记符/寄存器/槽全等；.o 层 call/jmp 重定位标签差为 P264 假报）。
    // 原 b28d-36（修法 P0+P52+P134）：ORIG 0x0860a558 无 charLevel/sp/spUp/slotR/slotW
    // 具名槽（-0x18/-0x14/-0x10/-0xc 全无）——remain 值全程 %ebx 直传、跨
    // getCurCharacSkillW 调用保活 ⇒ 原码是单条嵌套实参表达式 + for 尾条件 setge
    // 物化。槽往返版多 5 个槽（our 45→40 insn）。
    for (int level = 1; level <= user->get_charac_level(); ++level)
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->set_remain_sp_at_index(
                reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                        ->get_remain_sp_at_index(kind)
                    + (int)G_CDataManager()->GetSpAtLevelUp(level),
                kind);
    }
}
}

namespace WongWork
{
void CSkillChanger::_ResetSFPoint(CUser* user, ENUM_SKILL_TREE_KIND kind)
{
    if (user->get_charac_level() <= 0x31)
    {
        return;
    }
    if (kind <= 1)
    {
        return;
    }
    // 2026-09-10 试形留痕（A 轴）：去 `const` ⇒ InitSkill insns 340 与 ndiff 代理
    //   466 均不变（初值非常量、O0 无传播）⇒ 已回滚，维持原 `const`。
    register const int newPoint = user->get_charac_level() - 0x31;
    ((SkillSlot*)user->getCurCharacSkillW())
        ->set_remain_sfp_at_index(newPoint, kind);
}
}

namespace WongWork
{
// [b23S3C] ORIG 0x0860a34a 尾 `leave; ret` 无 mov $0,%eax ⇒ 返回 void；
// getCurCharacInvenR/CargoR 结果 ORIG 直落 0x4(%esp) 实参槽（无 -off 往返）⇒ P48 去具名局部；
// 第一段 ORIG 先 Inven_Item ctor(-0x4d) 再 sret 临时(-0xd8)+逐字段赋值 ⇒ 拆 ctor/赋值两句
// （初始化形被 copy-elision 直构进 item 槽，丢 ctor call + 16 条拷贝 = gap 30 的真因）
void CSkillChanger::_GetUserItemCount(const CUser* user)
{
    for (int i = 3; i < 0x69; i++)
    {
        Inven_Item item;
        item = ((CInventory*)user->getCurCharacInvenR())->GetInvenSlot(1, i);
        _CountSkillBook(item);
    }
    for (int i = 0; i < 0x98; i++)
    {
        Inven_Item item;
        item = ((const CCargo*)user->getCurCharacCargoR())->get_cargo_slot(i);
        _CountSkillBook(item);
    }
}
}

namespace WongWork
{
// ORIG 0x0860a634：累加 GetSpAtLevelUp(1..charLevel)。
int CSkillChanger::_GetBaseSkillPoint(const CUser* user) const
{
    int sum = 0;
    for (int level = 1; level <= user->get_charac_level(); level++)
    {
        sum += (int)G_CDataManager()->GetSpAtLevelUp(level);
    }
    return sum;
}
}

// [拆分] int CSkillChanger::_GetQuestSkillPoint → Game/SkillSlot/CSkillChanger_GetQuestSkillPoint.cpp

// [拆分] int CSkillChanger::GetCurCharacTotalSP → Game/SkillSlot/CSkillChanger_GetCurCharacTotalSP.cpp

// [拆分] void CSkillChanger::_ApplyUserData → Game/SkillSlot/CSkillChanger_ApplyUserData.cpp

// [拆分] void CSkillChanger::_CountSkillBook → Game/SkillSlot/CSkillChanger_CountSkillBook.cpp

// [拆分] void CSkillChanger::SkillInitialize → Game/SkillSlot/CSkillChanger_SkillInitialize.cpp

// [拆分] void SkillSlotCopyForceUse → Game/SkillSlot/SkillSlotCopyForceUse.cpp
