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

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义）----
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ---- _Charac_info +0x29 低 4 位有符号位域（ORIG：movzbl; shl $4; sar $4,%al）----
struct _Charac_info_nibble_field
{
    unsigned char m_pad[0x29];
    signed char m_specialNibble : 4;
    signed char m_padNibble : 4;
};

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
    if (job < 0 || job > 10)
    {
        return;
    }
    CDataManager* dm = G_CDataManager();
    const std::vector<std::pair<int, int> >& skillList =
        *(std::vector<std::pair<int, int> >*)(
            (char*)dm->m_characList + job * 0x7dc + 0x240);
    for (std::vector<std::pair<int, int> >::const_iterator it =
             skillList.begin();
         it != skillList.end(); ++it)
    {
        int skillIdx = it->first;
        int level = it->second;
        CSkill* s = dm->find_skill(job, skillIdx);
        if (s == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        bool foundEmpty = false;
        if (s->is_active_skill())
        {
            start = 0;
            end = 6;
            for (int i = 0; i < 6; ++i)
            {
                if (skill[i].m_index == 0)
                {
                    foundEmpty = true;
                    break;
                }
            }
        }
        if (!foundEmpty)
        {
            int group = s->get_group();
            switch (group)
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
                cMyTrace trace("addSkillOnCreateCharacter", 0x549, 5);
                trace("[%s][%d]", "addSkillOnCreateCharacter", group);
                break;
            }
        }
        for (int i = start; i < end; ++i)
        {
            if (skill[i].m_index == 0)
            {
                skill[i].m_index = (unsigned char)skillIdx;
                skill[i].m_level = (char)level;
                break;
            }
        }
    }
}
// ORIG 0x88923f1：skill>0x95→0；否则 (level+flag)*skill*10/100
int getSkillChecksum(int skill, int level, int flag)
{
    if (skill > 0x95)
        return 0;
    return (level + flag) * skill * 10 / 100;
}
// ORIG T 0x8892445
int finishSkillChecksum(int* sum, int key) { return 0; }
// ORIG T 0x8a9afa2
int getSkillSpendSPOnSteal(int sp, int penalty) { return 0; }

_Mastered_skill::_Mastered_skill()
{
    m_index = 0;
    m_level = 0;
}

stSPReturnInfo_t::stSPReturnInfo_t(unsigned short skillIdx,
                                   unsigned short levelDiff,
                                   unsigned int returnSP,
                                   unsigned int returnSFP)
    : m_skillIdx(skillIdx), m_levelDiff(levelDiff), m_returnSP(returnSP),
      m_returnSFP(returnSFP)
{
}

// ==================== 构造 / 析构 / 拷贝（ORIG 0x08694d6a 族） ====================

SkillSlot::SkillSlot()
{
    m_remainSP1 = 0;
    m_isLoadStrikerSkill = 0;
    memset(m_skillSlot, 0, sizeof(m_skillSlot));
    m_remainSP2 = 0;
    memset(m_skillSlot2, 0, sizeof(m_skillSlot2));
    m_field10 = -1;
    m_remainSFP1 = 0;
    m_remainSFP2 = 0;
    m_parent = 0;
}

SkillSlot::~SkillSlot()
{
}

// NOTE(主 agent 协调)：ORIG 的 SkillSlot 拷贝构造/赋值是编译器隐式生成
// （weak 符号 0x86996f6/0x869d91e，逐字节 AE 已由影子验证确认）。显式定义
// 已在 SkillSlot.cpp 删除；需同步从 SkillSlot.h 移除声明（L241/L243）并移除
// m_pad9 成员（ORIG 该处为自然 padding，隐式拷贝不复制），方可最终 identical。

void SkillSlot::set_parent(CUser* parent)
{
    m_parent = parent;
}

void SkillSlot::set_remain_sp(int sp1, int sp2, int sfp1, int sfp2)
{
    m_remainSP1 = sp1;
    m_remainSP2 = sp2;
    m_remainSFP1 = sfp1;
    m_remainSFP2 = sfp2;
}

void SkillSlot::get_remain_sp(int* sp1, int* sp2, int* sfp1, int* sfp2) const
{
    *sp1 = m_remainSP1;
    *sp2 = m_remainSP2;
    *sfp1 = m_remainSFP1;
    *sfp2 = m_remainSFP2;
}

// ==================== 简单查询 ====================

_Mastered_skill* SkillSlot::get_skillslot_buf(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == -1 || kind == 0)
    {
        return const_cast<_Mastered_skill*>(m_skillSlot);
    }
    if (kind == 1)
    {
        return const_cast<_Mastered_skill*>(m_skillSlot2);
    }
    return 0;
}

int* SkillSlot::get_cur_remain_sp(ENUM_SKILL_TREE_KIND kind) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (kind == -1 || kind == 0)
    {
        return const_cast<int*>(&m_remainSP1);
    }
    if (kind == 1)
    {
        return const_cast<int*>(&m_remainSP2);
    }
    if (kind == 2)
    {
        return const_cast<int*>(&m_remainSFP1);
    }
    if (kind == 3)
    {
        return const_cast<int*>(&m_remainSFP2);
    }
    return 0;
}

int* SkillSlot::get_temp_remain_sp()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return &m_tempRemainSP;
}

int* SkillSlot::get_temp_remain_sfp()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return &m_tempRemainSFP;
}

_Mastered_skill* SkillSlot::get_temp_skill_slot()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return m_tempSkillSlot;
}

int SkillSlot::GetSkillTreeIndex() const
{
    if (m_parent == 0)
    {
        return -1;
    }
    return ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex();
}

char SkillSlot::IsLoadStrikerSkill()
{
    return m_isLoadStrikerSkill;
}

void SkillSlot::SetLoadStrikerSkill(bool flag)
{
    m_isLoadStrikerSkill = (char)flag;
}

char SkillSlot::IsEmptySkillTree(ENUM_SKILL_TREE_KIND kind) const
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    char ret = 1;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index)
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

int SkillSlot::get_skill_count() const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index)
        {
            count = count + 1;
        }
    }
    return count;
}

unsigned int SkillSlot::get_skill_index(int slot) const
{
    if (m_parent == 0)
    {
        return 0xffffffff;
    }
    if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
        ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
    {
        return (unsigned char)m_skillSlot[slot].m_index;
    }
    return (unsigned char)m_skillSlot2[slot].m_index;
}

int SkillSlot::get_skill_level(int slot) const
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
        ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
    {
        return m_skillSlot[slot].m_level;
    }
    return m_skillSlot2[slot].m_level;
}

int SkillSlot::get_skillslot_group(int slot) const
{
    int group = 6;
    if (slot < 6)
    {
        group = -1;
    }
    else if (slot < 0x36)
    {
        group = 0;
    }
    else if (slot < 0x66)
    {
        group = 1;
    }
    else if (slot < 0x96)
    {
        group = 2;
    }
    else if (slot < 0xc6)
    {
        group = 3;
    }
    else if (slot < 0xcc)
    {
        group = -1;
    }
    if (group == 2 && 0x89 < slot)
    {
        group = 4;
    }
    return group;
}

int SkillSlot::getSkillSlotIndex(ENUM_SKILL_TREE_KIND kind, int slot) const
{
    if (kind > 1)
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              "int SkillSlot::getSkillSlotIndex(ENUM_SKILL_TREE_KIND, int) const",
                              0xd1f, "SkillSlot::getSkillSlotIndex kind(%d) error",
                              kind);
        return 0xffffffff;
    }
    if (kind == -1 || kind == 0)
    {
        return (unsigned char)m_skillSlot[slot].m_index;
    }
    return (unsigned char)m_skillSlot2[slot].m_index;
}

int SkillSlot::get_remain_sp_at_index(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == -1 || kind == 0)
    {
        return m_remainSP1;
    }
    if (kind == 1)
    {
        return m_remainSP2;
    }
    LogManager::logFormat(1, "skill_slot.cpp",
                          "int SkillSlot::get_remain_sp_at_index(ENUM_SKILL_TREE_KIND) const",
                          0x68, "Wrong Skill Tree Kind : %d", kind);
    return 0;
}

int SkillSlot::get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == 2)
    {
        return m_remainSFP1;
    }
    if (kind == 3)
    {
        return m_remainSFP2;
    }
    LogManager::logFormat(1, "skill_slot.cpp",
                          "int SkillSlot::get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND) const",
                          0x81, "Wrong Skill Tree Kind : %d", kind);
    return 0;
}

void SkillSlot::set_remain_sp_at_index(int value, ENUM_SKILL_TREE_KIND kind)
{
    if (kind == -1 || kind == 0)
    {
        m_remainSP1 = value;
    }
    else if (kind == 1)
    {
        m_remainSP2 = value;
    }
}

void SkillSlot::set_remain_sfp_at_index(int value, ENUM_SKILL_TREE_KIND kind)
{
    if (kind == 2)
    {
        m_remainSFP1 = value;
    }
    else if (kind == 3)
    {
        m_remainSFP2 = value;
    }
    else
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              "void SkillSlot::set_remain_sfp_at_index(int, ENUM_SKILL_TREE_KIND)",
                              0x75, "Wrong Skill Tree Kind : %d", kind);
    }
}

void SkillSlot::get_skill_slot(char* out1, char* out2, int size) const
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    memcpy(out1, m_skillSlot, size);
    memcpy(out2, m_skillSlot2, size);
}

void SkillSlot::set_skill_slot(char* in1, char* in2, int size)
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    memcpy(m_skillSlot, in1, size);
    memcpy(m_skillSlot2, in2, size);
}

void SkillSlot::get_skill_slot_at_index(char* out, int size,
                                        ENUM_SKILL_TREE_KIND kind) const
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    if (kind == -1 || kind == 0)
    {
        memcpy(out, m_skillSlot, size);
    }
    else if (kind == 1)
    {
        memcpy(out, m_skillSlot2, size);
    }
}

void SkillSlot::set_skill_slot_at_index(char* in, int size,
                                        ENUM_SKILL_TREE_KIND kind)
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    switch (kind)
    {
    case -1:
    case 0:
        memcpy(m_skillSlot, in, size);
        break;
    case 1:
        memcpy(m_skillSlot2, in, size);
        break;
    default:
        break;
    }
}

int SkillSlot::FindSkill(int skillIdx, ENUM_SKILL_TREE_KIND kind) const
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == skillIdx)
        {
            return buf[i].m_level;
        }
    }
    return 0;
}

int SkillSlot::FindSkill(int skillIdx) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return 0;
    }
    for (int i = 0; i < 204; ++i)
    {
        if ((unsigned char)buf[i].m_index == skillIdx)
        {
            return (int)(char)m_skillSlot[i].m_level;
        }
    }
    return 0;
}

int SkillSlot::check_skill_mastered(int skillIdx, int level, int group) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return 0;
    }
    int slot = get_skillslot_no(
        skillIdx, group,
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex(),
        1);
    if (slot < 0)
    {
        return 0;
    }
    if ((unsigned char)buf[slot].m_index == skillIdx &&
        level <= buf[slot].m_level)
    {
        return 1;
    }
    return 0;
}

int SkillSlot::check_skill_mastered(int skillIdx, int level, int group,
                                    ENUM_SKILL_TREE_KIND kind) const
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
    int slot = get_skillslot_no(skillIdx, group, kind, 1);
    if (slot < 0)
    {
        return 0;
    }
    if ((unsigned char)buf[slot].m_index == skillIdx &&
        level <= buf[slot].m_level)
    {
        return 1;
    }
    return 0;
}

int SkillSlot::check_skill_mastered(_Mastered_skill* buf, int skillIdx,
                                    int level, int group) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (buf == 0)
    {
        return 0;
    }
    int slot = get_skillslot_no(buf, skillIdx, group, 1);
    if (slot < 0)
    {
        return 0;
    }
    if ((unsigned char)buf[slot].m_index == skillIdx &&
        level <= buf[slot].m_level)
    {
        return 1;
    }
    return 0;
}

int SkillSlot::get_skillslot_no(int skillIdx, int group,
                                ENUM_SKILL_TREE_KIND kind,
                                bool checkCombo) const
{
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return -1;
    }
    if (checkCombo && checkComboSkillInsertQuickSlot(skillIdx))
    {
        for (int i = 0; i < 6; ++i)
        {
            if (buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            if (buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
    }
    int start = 0;
    int end = 0;
    switch (group)
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
        break;
    }
    for (int i = start; i < end; ++i)
    {
        if (buf[i].m_index == skillIdx)
        {
            return i;
        }
    }
    return -1;
}

int SkillSlot::get_skillslot_no(_Mastered_skill* buf, int skillIdx, int group,
                                bool checkCombo) const
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (buf == 0)
    {
        return -1;
    }
    if (checkCombo)
    {
        for (int i = 0; i < 6; ++i)
        {
            if ((unsigned char)buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
    }
    int start = 0;
    int end = 0;
    switch (group)
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
        break;
    }
    for (int i = start; i < end; ++i)
    {
        if ((unsigned char)buf[i].m_index == skillIdx)
        {
            return i;
        }
    }
    if (checkCombo)
    {
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            if ((unsigned char)buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
    }
    return -1;
}

int SkillSlot::checkComboSkillInsertQuickSlot(int skillIdx) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_job() == 9 && skillIdx == 0)
    {
        return 0;
    }
    return 1;
}

bool SkillSlot::checkMoveComboSkillSlot(unsigned char slot1, unsigned char slot2)
{
    if (m_parent == 0)
    {
        return false;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_job() == 9 &&
        (slot1 < 6 || slot2 < 6))
    {
        return false;
    }
    return true;
}

int SkillSlot::insert_skill(int skillIdx, int group, bool isActive,
                            ENUM_SKILL_TREE_KIND kind, int level)
{
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return -1;
    }
    int slot = get_skillslot_no(0, group, kind, isActive);
    if (slot != -1)
    {
        buf[slot].m_index = (unsigned char)skillIdx;
        buf[slot].m_level = (char)level;
    }
    return slot;
}

int SkillSlot::insert_skill(_Mastered_skill* buf, int skillIdx, int group,
                            bool isActive, int level)
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (buf == 0)
    {
        return -1;
    }
    int slot = get_skillslot_no(buf, 0, group, isActive);
    if (slot != -1)
    {
        buf[slot].m_index = (unsigned char)skillIdx;
        buf[slot].m_level = (char)level;
    }
    return slot;
}

int SkillSlot::insert_skill_with_slot_index(int slot, int skillIdx,
                                            bool isActive, int level)
{
    if (slot < 0)
    {
        return -4;
    }
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    buf[slot].m_index = (unsigned char)slot;
    buf[slot].m_level = 1;
    return slot;
}

int SkillSlot::delete_skill(int skillIdx, int job,
                            ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    skill = G_CDataManager()->find_skill(skillIdx, job);
    if (skill == 0)
    {
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
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == skill->m_index)
        {
            buf[i].m_index = 0;
            buf[i].m_level = 0;
            break;
        }
    }
    return 0;
}

int SkillSlot::clear_all_skills(ENUM_SKILL_TREE_KIND kind)
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    int* remain = get_cur_remain_sp(kind);
    if (buf == 0 || remain == 0)
    {
        return 0;
    }
    m_parent = 0;
    *remain = 0;
    memset(buf, 0, 0x198);
    return 1;
}

int SkillSlot::clear_all_skills_both()
{
    m_parent = 0;
    m_remainSP1 = 0;
    memset(m_skillSlot, 0, 0x198);
    m_remainSP2 = 0;
    memset(m_skillSlot2, 0, 0x198);
    m_isLoadStrikerSkill = 0;
    return 1;
}

int SkillSlot::clear_sfp_skills(char job, ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    for (int i = 0; i < 204; ++i)
    {
        if (kind == -1 || kind == 0)
        {
            skill = G_CDataManager()->find_skill(job, m_skillSlot[i].m_index);
            if (skill != 0)
            {
                if (skill->IsSpecialSkill())
                {
                    m_skillSlot[i].m_index = 0;
                    m_skillSlot[i].m_level = 0;
                }
            }
        }
        else if (kind == 1)
        {
            skill = G_CDataManager()->find_skill(job, m_skillSlot2[i].m_index);
            if (skill != 0)
            {
                if (skill->IsSpecialSkill())
                {
                    m_skillSlot2[i].m_index = 0;
                    m_skillSlot2[i].m_level = 0;
                }
            }
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                "bool SkillSlot::clear_sfp_skills(char, ENUM_SKILL_TREE_KIND)",
                0x49c, "SkillSlot::clear_sfp_skills error %d", kind);
        }
    }
    return 1;
}

int SkillSlot::backupSkill(ENUM_SKILL_TREE_KIND kind)
{
    int backupSize = 0x198;
    if (kind == -1 || kind == 0)
    {
        memcpy(m_tempSkillSlot, m_skillSlot, backupSize);
        m_tempRemainSP = m_remainSP1;
        m_tempRemainSFP = m_remainSFP1;
        return 1;
    }
    if (kind == 1)
    {
        memcpy(m_tempSkillSlot, m_skillSlot2, backupSize);
        m_tempRemainSP = m_remainSP2;
        m_tempRemainSFP = m_remainSFP2;
        return 1;
    }
    return 0;
}

void SkillSlot::initialize_sfp_skill()
{
    if (m_parent == 0)
    {
        return;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_level() <= 0x31)
    {
        return;
    }
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->clear_sfp_skills(
            (char)((CUserCharacInfo*)m_parent)->get_charac_job(),
            (ENUM_SKILL_TREE_KIND)0);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->clear_sfp_skills(
            (char)((CUserCharacInfo*)m_parent)->get_charac_job(),
            (ENUM_SKILL_TREE_KIND)1);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->set_remain_sfp_at_index(
            ((CUserCharacInfo*)m_parent)->get_charac_level() - 0x31,
            (ENUM_SKILL_TREE_KIND)2);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->set_remain_sfp_at_index(
            ((CUserCharacInfo*)m_parent)->get_charac_level() - 0x31,
            (ENUM_SKILL_TREE_KIND)3);
}

int SkillSlot::makeCheckSum(int key) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    int sum = 0;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index)
        {
            sum = getSkillChecksum(i, (unsigned char)buf[i].m_index,
                                   buf[i].m_level) +
                  sum;
        }
    }
    return finishSkillChecksum(&sum, key);
}

int SkillSlot::growtype_skill(int job, int skillIdx, int level,
                              ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            "int SkillSlot::growtype_skill(int, int, int, ENUM_SKILL_TREE_KIND)",
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
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
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
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           ((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, level, (eSkillAddReason)1);
        }
    }
    return slot;
}

int SkillSlot::skill_move(unsigned char slot1, unsigned char slot2, int job)
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (!checkMoveComboSkillSlot(slot1, slot2))
    {
        return 0x13;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    if (buf[slot1].m_index == 0 && buf[slot2].m_index == 0)
    {
        return 0x15;
    }
    CSkill* skill1 = G_CDataManager()->find_skill(job, buf[slot1].m_index);
    CSkill* skill2 = G_CDataManager()->find_skill(job, buf[slot2].m_index);
    if (skill1 == 0 && skill2 == 0)
    {
        return 1;
    }
    int group1 = get_skillslot_group(slot1);
    while (group1 == 6)
    {
        return 1;
    }
    int group2 = get_skillslot_group(slot2);
    while (group2 == 6)
    {
        return 1;
    }
    if (group1 == 4 || group2 == 4)
    {
        return 0x13;
    }
    bool canMove = false;
    if (buf[slot1].m_index == 0 && buf[slot2].m_index != 0)
    {
        if (skill2 == 0)
        {
            canMove = false;
        }
        else if (group1 == -1)
        {
            if (skill2->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill2->get_group() == group1)
            {
                canMove = true;
            }
        }
    }
    else if (buf[slot2].m_index == 0 && buf[slot1].m_index != 0)
    {
        if (skill1 == 0)
        {
            canMove = false;
        }
        else if (group2 == -1)
        {
            if (skill1->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill1->get_group() == group2)
            {
                canMove = true;
            }
        }
    }
    else if (skill1 == 0 || skill2 == 0)
    {
        canMove = false;
    }
    else if (group1 == -1 && group2 == -1)
    {
        canMove = true;
    }
    else
    {
        if (group1 == -1 && skill1->get_group() == group2)
        {
            if (skill2->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill2->get_group() == group1 && group2 == -1)
            {
                if (skill1->is_active_skill())
                {
                    canMove = true;
                }
            }
            else
            {
                if (skill2->get_group() == group1 &&
                    skill1->get_group() == group2)
                {
                    canMove = true;
                }
            }
        }
    }
    if (!canMove)
    {
        return 0x13;
    }
    _Mastered_skill temp;
    temp = buf[slot1];
    buf[slot1] = buf[slot2];
    buf[slot2] = temp;
    return 0;
}

int SkillSlot::check_skill_list(
    int job, std::vector<std::pair<int, int> > list, int level) const
{
    if (list.size() != 0)
    {
        CSkill* skill = 0;
        std::vector<std::pair<int, int> >::iterator it;
        std::pair<int, int> pair;
        for (it = list.begin(); it != list.end(); ++it)
        {
            pair = *it;
            int skillIdx = pair.first;
            int reqLevel = pair.second;
            skill = G_CDataManager()->find_skill(job, skillIdx);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    "bool SkillSlot::check_skill_list(int, std::vector<std::pair<int, int> >, int) const",
                    0x42b, "G_CDataManager()->find_skill( %d, %d )", job,
                    skillIdx);
                return 0;
            }
            int group = skill->get_group();
            reqLevel = reqLevel - level;
            if (reqLevel < 1)
            {
                reqLevel = 1;
            }
            if (check_skill_mastered(skillIdx, reqLevel, group) != 1)
            {
                return 0;
            }
        }
    }
    return 1;
}

void SkillSlot::calcReturnSpSfpPerOneSkill(int& sp, int& sfp, int level,
                                           const _Mastered_skill* buf,
                                           int slot, const CSkill* skill,
                                           bool isSteal) const
{
    int need = 0;
    int fitness = 0;
    for (int i = level + 1; buf[slot].m_level >= i; ++i)
    {
        fitness = skill->get_second_growtype_fitness(
            (int)((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
            (int)((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType());
        if (fitness == 0)
        {
            fitness = skill->get_fitness(
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharFirstGrowType());
        }
        need = skill->get_need_sp_sfp_for_level(i - 1, i);
        if (need < 0)
        {
            break;
        }
        if (0 < fitness)
        {
            need = ((100 - fitness) * need) / 100;
        }
        if (isSteal)
        {
            int penalty = skill->GetStealSPPenalty();
            need = getSkillSpendSPOnSteal(need, penalty);
        }
        if (need < 0)
        {
            return;
        }
        if (skill->IsSpecialSkill())
        {
            sfp = sfp + need;
        }
        else
        {
            sp = sp + need;
        }
    }
}

void SkillSlot::calcUsedSP(const _Mastered_skill* buf, int& sp, int& sfp) const
{
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
    CCharacter* character =
        (CCharacter*)((char*)dm->m_characList +
                      ((CUserCharacInfo*)m_parent)->get_charac_job() * 0x7dc);
    std::vector<std::pair<int, int> > giveList(
        *(std::vector<std::pair<int, int> >*)((char*)dm->m_characList +
                                              ((CUserCharacInfo*)m_parent)
                                                      ->get_charac_job() *
                                                  0x7dc +
                                              0x240));
    std::vector<std::pair<int, int> > learnList;
    char result = 1;
    char secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
    char firstGrow = 0;
    if (secondGrow == 0)
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        dm = G_CDataManager();
        character = (CCharacter*)((char*)dm->m_characList +
                                  ((CUserCharacInfo*)m_parent)
                                      ->get_charac_job() *
                                      0x7dc);
        result = character->get_give_skill((int)firstGrow, (int)secondGrow,
                                           learnList, 1);
    }
    else
    {
        secondGrow = ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        firstGrow = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        dm = G_CDataManager();
        character = (CCharacter*)((char*)dm->m_characList +
                                  ((CUserCharacInfo*)m_parent)
                                      ->get_charac_job() *
                                      0x7dc);
        result = character->get_give_skill((int)firstGrow, (int)secondGrow,
                                           learnList, 0);
    }
    if (result == 1)
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
            ((PvPSkillTreeParameterScript*)((char*)dm + 0xa800))
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
                             ((char*)expertScript + 0xc))
                            .begin();
                    while (true)
                    {
                        std::vector<std::pair<int, int> >::const_iterator eend =
                            (*(std::vector<std::pair<int, int> >*)
                                 ((char*)expertScript + 0xc))
                                .end();
                        if (eit == eend)
                        {
                            break;
                        }
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
    GameWorld* world = G_GameWorld();
    if (world->IsPvPSkilTreeChannel() == 0)
    {
        CDataManager* dm = G_CDataManager();
        out = *(std::vector<std::pair<int, int> >*)(
            (char*)dm->m_characList + job * 0x7dc + 0x240);
        std::vector<std::pair<int, int> > learnList;
        char result = 1;
        if (secondGrow == 0)
        {
            dm = G_CDataManager();
            result = ((CCharacter*)((char*)dm->m_characList + job * 0x7dc))
                         ->get_give_skill(firstGrow, 0, learnList, 1);
        }
        else
        {
            dm = G_CDataManager();
            result = ((CCharacter*)((char*)dm->m_characList + job * 0x7dc))
                         ->get_give_skill(firstGrow, secondGrow, learnList, 0);
        }
        if (result == 1)
        {
            std::vector<std::pair<int, int> >::const_iterator git =
                learnList.begin();
            std::vector<std::pair<int, int> >::const_iterator gend =
                learnList.end();
            while (git != gend)
            {
                std::vector<std::pair<int, int> >::iterator lit =
                    out.begin();
                std::vector<std::pair<int, int> >::iterator lend = out.end();
                while (true)
                {
                    if (lit == lend)
                    {
                        break;
                    }
                    if (lit->first == git->first)
                    {
                        lit->second = std::max(lit->second, git->second);
                        break;
                    }
                    ++lit;
                }
                if (lit == lend)
                {
                    out.push_back(std::make_pair(git->first, git->second));
                }
                ++git;
            }
            return 1;
        }
        return -1;
    }
    else
    {
        out.clear();
        secondGrow =
            ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType();
        char firstGrow2 = ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType();
        int characJob = ((CUserCharacInfo*)m_parent)->get_charac_job();
        CDataManager* dm = G_CDataManager();
        ((PvPSkillTreeParameterScript*)((char*)dm + 0xa800))
            ->getGiveSkill(characJob, (int)firstGrow2, (int)secondGrow, out);
        return 1;
    }
}

int SkillSlot::master_new_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow, int addLevel)
{
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
        UserQuest* quest = m_parent->getCurCharacQuestR();
        quest->isClearQuest(0x163);
        if (m_parent->is_clear_stealingSkillMission() == 0)
        {
            return -1;
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
    }
    unsigned short overLevel =
        m_parent->GetPremiumInfo()->getOverSkillLevel();
    GameWorld* world = G_GameWorld();
    if (world->IsPvPSkilTreeChannel() == 0)
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
    it = skill->m_preSkillList.begin();
    while (true)
    {
        if (it == skill->m_preSkillList.end())
        {
            break;
        }
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
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, addLevel,
                           (eSkillAddReason)0);
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
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
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, targetLevel,
                           (eSkillAddReason)0);
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
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
        UserQuest* quest = m_parent->getCurCharacQuestR();
        quest->isClearQuest(0x163);
        if (m_parent->is_clear_stealingSkillMission() == 0)
        {
            return -1;
        }
        isSteal = 1;
        level = level - skill->GetStealLevelPenalty();
    }
    unsigned short overLevel =
        m_parent->GetPremiumInfo()->getOverSkillLevel();
    GameWorld* world = G_GameWorld();
    if (world->IsPvPSkilTreeChannel() == 0)
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
    it = skill->m_preSkillList.begin();
    while (true)
    {
        if (it == skill->m_preSkillList.end())
        {
            break;
        }
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
                                      int availableGSP, int& needGold,
                                      int& learnCnt, short& needGSP,
                                      int guildLevel)
{
    if ((unsigned char)((char*)&guildInfo)[0x44] >= 0x10)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            "int SkillSlot::master_new_guild_skill(int, int, int, int, const STGuildDBInfoOnly&, int, int&, int&, short int&, int)",
            0x102, "BUY GUILD SKILL, GName(%s) , Learn Cnt(%d)", &guildInfo,
            (unsigned char)((char*)&guildInfo)[0x44]);
        return -2;
    }
    CSkill* skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    learnCnt = 1;
    int count = (unsigned char)((char*)&guildInfo)[0x44];
    for (int i = 0; i < count; ++i)
    {
        if (*(int*)((char*)&guildInfo + i * 20 + 0x45) == skillIdx)
        {
            learnCnt = (char)((char*)&guildInfo)[i * 20 + 0x49] + 1;
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
        *(unsigned int*)((char*)&guildInfo + 0xa0) < (unsigned int)needGold)
    {
        return -5;
    }
    needGSP = (short)skill->get_guildSkillNeedGSP(learnCnt);
    if (needGSP < 0 || availableGSP < needGSP)
    {
        return -2;
    }
    if ((int)(unsigned char)((char*)&guildInfo)[0x1b] <
        skill->get_needGuildLevel(learnCnt))
    {
        return -6;
    }
    if (m_parent != 0)
    {
        int treeKind =
            ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex();
        int characJob = ((CUserCharacInfo*)m_parent)->get_charac_job();
        ((cUserHistoryLog*)((char*)m_parent + 0x79700))
            ->SkillAdd(characJob, treeKind, skillIdx, learnCnt,
                       (eSkillAddReason)0);
        m_parent->makeGuildSkillMessage(skill->getSkillName(), learnCnt);
    }
    return 0;
}

int SkillSlot::refund_skill(int skillIdx, int level, int job, int firstGrow,
                            int secondGrow, int refundCount, int& result)
{
    result = 0x13;
    if (m_parent == 0)
    {
        return -4;
    }
    CSkill* skill = G_CDataManager()->find_skill(job, skillIdx);
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
    GameWorld* world = G_GameWorld();
    if (world->IsPvPSkilTreeChannel() == 0 ||
        ((CUserCharacInfo*)m_parent)->get_pvp_grade() > -1)
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
        void* expertScript = 0;
        ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType();
        expertScript = G_CDataManager()->GetExpertJobScript(0);
        if (expertScript != 0)
        {
            std::vector<std::pair<int, int> >& expertList =
                *(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                      0xc);
            for (unsigned int i = 0; i < expertList.size(); ++i)
            {
                if (expertList[i].first == skillIdx)
                {
                    result = 7;
                    return -5;
                }
            }
        }
        std::pair<int, int> pair;
        std::vector<std::pair<int, int> > giveList;
        int giveResult = get_give_skill(job, firstGrow, secondGrow, giveList);
        if (giveResult < 0)
        {
            return -1;
        }
        bool found = false;
        for (unsigned int i = 0; i < giveList.size(); ++i)
        {
            pair = giveList[i];
            if (buf[slot].m_index == pair.first)
            {
                if (curLevel < pair.second)
                {
                    result = 7;
                    return -1;
                }
                found = true;
                break;
            }
        }
        std::vector<std::pair<int, int> > postList;
        int characJob = ((CUserCharacInfo*)m_parent)->get_charac_job();
        G_CDataManager()->m_skillList->get_post_learning_skill(
            characJob, skillIdx, postList);
        for (unsigned int i = 0; i < postList.size(); ++i)
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
            int treeKind =
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex();
            if (treeKind == -1 || treeKind == 0)
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
            int treeKind2 =
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex();
            int characJob2 = ((CUserCharacInfo*)m_parent)->get_charac_job();
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SkillDel(characJob2, treeKind2, skillIdx, refundCount,
                           (eSkillDelReason)1);
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SFPAdd((int)kindChar, remain + returnSP, returnSP,
                         (eSPAddReason)2);
            if (0 < refundCount && 10 < returnSP / refundCount)
            {
                int characNo =
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo();
                unsigned int accId = m_parent->get_acc_id();
                char* str = NumberToString(accId, 0);
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    "int SkillSlot::refund_skill(int, int, int, int, int, int, int&)",
                    0x93b,
                    "Wrong refund TP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    str, characNo, skillIdx, returnSP, refundCount);
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex();
            int remain =
                get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)treeKind);
            ENUM_SKILL_TREE_KIND treeKind2 =
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex();
            set_remain_sp_at_index(remain + returnSP,
                                   (ENUM_SKILL_TREE_KIND)treeKind2);
            ENUM_SKILL_TREE_KIND treeKind3 =
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex();
            int characJob2 = ((CUserCharacInfo*)m_parent)->get_charac_job();
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SkillDel(characJob2, treeKind3, skillIdx, refundCount,
                           (eSkillDelReason)1);
            int newRemain = remain + returnSP;
            ENUM_SKILL_TREE_KIND treeKind4 =
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex();
            ((cUserHistoryLog*)((char*)m_parent + 0x79700))
                ->SPAdd(treeKind4, newRemain, returnSP, (eSPAddReason)2);
            if (0 < refundCount && 1000 < returnSP / refundCount)
            {
                int characNo =
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo();
                unsigned int accId = m_parent->get_acc_id();
                char* str = NumberToString(accId, 0);
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    "int SkillSlot::refund_skill(int, int, int, int, int, int, int&)",
                    0x921,
                    "Wrong refund SP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    str, characNo, skillIdx, returnSP, refundCount);
            }
        }
        result = 0;
        return slot;
    }
    result = 7;
    return -3;
}

int SkillSlot::try_refund_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow,
                                int refundCount, int& result)
{
    result = 0x13;
    if (m_parent == 0)
    {
        return -4;
    }
    CSkill* skill = G_CDataManager()->find_skill(job, skillIdx);
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
    GameWorld* world = G_GameWorld();
    if (world->IsPvPSkilTreeChannel() == 0 ||
        ((CUserCharacInfo*)m_parent)->get_pvp_grade() > -1)
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
        ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType();
        expertScript = G_CDataManager()->GetExpertJobScript(0);
        if (expertScript != 0)
        {
            std::vector<std::pair<int, int> >& expertList =
                *(std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                      0xc);
            for (unsigned int i = 0; i < expertList.size(); ++i)
            {
                if (expertList[i].first == skillIdx)
                {
                    result = 7;
                    return -5;
                }
            }
        }
        std::pair<int, int> pair;
        std::vector<std::pair<int, int> > giveList;
        int giveResult = get_give_skill(job, firstGrow, secondGrow, giveList);
        if (giveResult < 0)
        {
            return -1;
        }
        for (unsigned int i = 0; i < giveList.size(); ++i)
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
        int characJob = ((CUserCharacInfo*)m_parent)->get_charac_job();
        G_CDataManager()->m_skillList->get_post_learning_skill(
            characJob, skillIdx, postList);
        for (unsigned int i = 0; i < postList.size(); ++i)
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
    ((cUserHistoryLog*)((char*)m_parent + 0x79700))
        ->InitSkill(spKind, ((CUserCharacInfo*)m_parent)->get_charac_level(),
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
        (int)(char)(*(std::vector<_Charac_info>*)((char*)m_parent + 0x796e8))
            [characIdx]
                .m_field26);
    guard.put_byte(
        (int)((const _Charac_info_nibble_field*)&(
            *(std::vector<_Charac_info>*)((char*)m_parent + 0x796e8))
                  [characIdx])
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

int SkillSlot::checkskillSlotEmptyCount(int& out) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    int charLevel = ((CUserCharacInfo*)m_parent)->get_charac_level();
    ENUM_SKILL_TREE_KIND kind =
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex();
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    int emptyCount = 0;
    for (int i = 0; i < 6; ++i)
    {
        if (buf[i].m_index == 0)
        {
            emptyCount = emptyCount + 1;
        }
    }
    int total = emptyCount;
    if (charLevel < 0x10)
    {
        emptyCount = emptyCount - 3;
        out = 1;
    }
    else if (charLevel < 0x1f)
    {
        emptyCount = emptyCount - 1;
        out = 2;
    }
    else
    {
        out = 3;
    }
    total = emptyCount;
    if (0x2d < charLevel)
    {
        emptyCount = 0;
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            if (buf[i].m_index == 0)
            {
                emptyCount = emptyCount + 1;
            }
        }
        if (charLevel < 0x3d)
        {
            emptyCount = emptyCount - 3;
            out = 4;
        }
        else
        {
            emptyCount = emptyCount - 2;
            out = 5;
        }
        if (0 < emptyCount)
        {
            total = total + emptyCount;
        }
    }
    return total;
}

void SkillSlot::debugCheckGrowTypeSkill(int level, int job, int firstGrow,
                                        int secondGrow)
{
    for (int i = 0; i <= 203; ++i)
    {
        if (m_skillSlot[i].m_index != 0)
        {
            CSkill* skill = NULL;
            skill = G_CDataManager()->find_skill(job, m_skillSlot[i].m_index);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    "void SkillSlot::debugCheckGrowTypeSkill(int, int, int, int)",
                    0x5a4, "G_CDataManager()->find_skill( %d, %d )", job,
                    (unsigned char)m_skillSlot[i].m_index);
                return;
            }
            if (!skill->can_learn(level, firstGrow, secondGrow,
                                  (int)(char)m_skillSlot[i].m_level))
            {
                m_skillSlot[i].m_index = 0;
                m_skillSlot[i].m_level = 0;
            }
        }
        if (m_skillSlot2[i].m_index != 0)
        {
            CSkill* skill = NULL;
            skill = G_CDataManager()->find_skill(job, m_skillSlot2[i].m_index);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    "void SkillSlot::debugCheckGrowTypeSkill(int, int, int, int)",
                    0x5b7, "G_CDataManager()->find_skill( %d, %d )", job,
                    (unsigned char)m_skillSlot2[i].m_index);
                return;
            }
            if (!skill->can_learn(level, firstGrow, secondGrow,
                                  (int)(char)m_skillSlot2[i].m_level))
            {
                m_skillSlot2[i].m_index = 0;
                m_skillSlot2[i].m_level = 0;
            }
        }
    }
}

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

void SkillSlot::loadComboSkill(SIG_COMBO_SKILL* data)
{
    if (data != 0)
    {
        if (!m_comboSkill.loadData(data))
        {
            LogManager::logFormat(
                1, "skill_slot.cpp", "void SkillSlot::loadComboSkill(SIG_COMBO_SKILL*)",
                0xc75, "SkillSlot::loadComboSkill error");
        }
    }
}

void SkillSlot::getComboSkillSaveData(SIG_COMBO_SKILL* data) const
{
    if (m_parent != 0 && data != 0)
    {
        data->m_characNo = m_parent->get_charac_no(-1);
        data->m_job = ((CUserCharacInfo*)m_parent)->get_charac_job();
        if (!m_comboSkill.saveData(data))
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                "void SkillSlot::getComboSkillSaveData(SIG_COMBO_SKILL*) const",
                0xc84, "SkillSlot::getComboSkillSaveData error");
        }
    }
}

void SkillSlot::getComboSkillSendInfo(PacketGuard& packet) const
{
    m_comboSkill.getComboSkillSendList(packet);
}

void SkillSlot::setComboSkillInfo(ENUM_SKILL_TREE_KIND kind,
                                  std::map<short, std::vector<int> >& map)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            "void SkillSlot::setComboSkillInfo(ENUM_SKILL_TREE_KIND, ComboSkillMap&)",
            0xc92, "SkillSlot::setComboSkillInfo kind(%d) error", kind);
    }
    else
    {
        m_comboSkill.setComboSkill(kind, map);
    }
}

void SkillSlot::resetComboSkill(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != -1 && kind != 0)
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              "void SkillSlot::resetComboSkill(ENUM_SKILL_TREE_KIND)",
                              0xcab, "SkillSlot::resetComboSkill kind(%d) error",
                              kind);
    }
    else
    {
        m_comboSkill.resetTree(kind);
    }
}

void SkillSlot::verifyComboSkill()
{
    verifyComboSkillTree((ENUM_SKILL_TREE_KIND)0);
    verifyComboSkillTree((ENUM_SKILL_TREE_KIND)1);
}

void SkillSlot::verifyComboSkillTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            "void SkillSlot::verifyComboSkillTree(ENUM_SKILL_TREE_KIND)", 0xd33,
            "SkillSlot::verifyComboSkillTree kind(%d) error", kind);
    }
    else
    {
        std::vector<std::pair<int, int> >* slotList =
            (std::vector<std::pair<int, int> >*)(
                (char*)G_CDataManager()->m_characList + 0x48fc);
        for (int i = 0; i < 6; ++i)
        {
            if ((*slotList)[i].first == 0)
            {
            }
            else
            {
                m_comboSkill.eraseDuplicationComboSKill(
                    kind, (*slotList)[i].first);
            }
        }
    }
}

void SkillSlot::eraseDuplicationComboSKill()
{
    eraseDuplicationComboSKillTree((ENUM_SKILL_TREE_KIND)0);
    eraseDuplicationComboSKillTree((ENUM_SKILL_TREE_KIND)1);
}

void SkillSlot::eraseDuplicationComboSKillTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            "void SkillSlot::eraseDuplicationComboSKillTree(ENUM_SKILL_TREE_KIND)",
            0xd0b, "SkillSlot::eraseDuplicationComboSKillTree kind(%d) error",
            kind);
    }
    else
    {
        int skillIdx = 0;
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            skillIdx = getSkillSlotIndex(kind, i);
            if (skillIdx != 0)
            {
                m_comboSkill.eraseDuplicationComboSKill(kind, skillIdx);
            }
        }
    }
}

int SkillSlot::moveComboSkillExtensionQuickSlot(int skillIdx, int level,
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
    if (slot == -1)
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
                "bool SkillSlot::moveComboSkillExtensionQuickSlot(int, int, ENUM_SKILL_TREE_KIND)",
                0xcf9,
                "moveComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                slot, (unsigned char)buf[slot].m_index,
                (int)(char)buf[slot].m_level, kind,
                m_parent->get_charac_no(-1));
        }
    }
    else if (buf[slot].m_level < level)
    {
        buf[slot].m_level = (char)level;
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
        if (buf[i].m_index)
        {
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
                    "void SkillSlot::pullComboSkillExtensionQuickSlot(ENUM_SKILL_TREE_KIND)",
                    0xcd7,
                    "pullComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                    i, (unsigned char)buf[i].m_index,
                    (int)(char)buf[i].m_level, kind,
                    m_parent->get_charac_no(-1));
            }
        }
    }
}
