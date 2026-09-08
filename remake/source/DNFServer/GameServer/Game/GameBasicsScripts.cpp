#include <cstring>
#include "CDataManager.h"
#include "GamePODTypes.h"

namespace APSystem
{
// ORIG C1 0x0889484a：仅按序构造 6 个容器成员，不调用 ClearData。
CActionPointEtcParameter::CActionPointEtcParameter()
{
}

CActionPointEtcParameter::~CActionPointEtcParameter() {}

void CActionPointEtcParameter::ClearData()
{
    m_actionPoints.clear();
    m_actionPointRefs.clear();
    m_rewardMessages.clear();
    m_rewardMessages2.clear();
    m_medalRewards.clear();
    m_todayRewards.clear();
}

bool APSystem::CActionPointEtcParameter::IsValidTodayRewardItem(
    _SIG_LOAD_ACTION_POINT::_TodayRewardItem item)
{
    // ORIG 0x08894976：value 无槽（test %eax/js + 寄存器 cmp/jg 直判，P0 右值内联）。
    return item.m_itemIdx >= 0 && item.m_itemIdx <= 3;
}
}


void stAvatarEmblemInfo_t::init()
{
    // ORIG 0x081506e8：循环 i=0..4，清零 stEmblemSocket[i] 的
    // emblemSocketType_(word, +6i) 与 emblemItemIdx_(dword, +6i+2)
    for (int i = 0; i < 5; ++i)
    {
        stEmblemSocket[i].emblemSocketType_ = 0;
        stEmblemSocket[i].emblemItemIdx_ = 0;
    }
}

void stAvatarExpansionInfo_t::init()
{
    // ORIG 0x083422d：for(i=0;i<=1;++i) 循环写 movw $0,(%eax,%edx,2)（P25 setle 形）。
    for (int i = 0; i <= 1; ++i)
        m_color[i] = 0;
}


// ============================================================================
// PvPSkillTreeParameterScript 查询（ORIG 0x08a5e178 / 0x08a5dd62, 符号 T）
// 最小实现保证符号 T + 可链接；完整脚本解析逻辑待细化。
// ============================================================================
int PvPSkillTreeParameterScript::getPvPSkillPoint(
    int job, int firstGrow, int secondGrow, int grade, bool flag) const
{
    // ORIG 0x08a5dd62：grade 下限钳制到 m_field4c；
    // m_map18[grade][job*5+firstGrow] 为已投入点数；
    // m_map30[(job,firstGrow)] 的 m_pointCost(+0x40)：flag=true 且已有点数则扣减，
    // flag=false 且点数为 0 则取消耗值。secondGrow 在 ORIG 中被强制置 0（未参与）。
    // [b28d-56 判词·P167/P212] ORIG 0x08a5dd62 用三参 ctor `PvPSkillTreeKey(h,h,h)`
    // （_ZN15PvPSkillTreeKeyC1Ehhh，ORIG W 0x08a5e768），第三实参为立即数 0（ORIG
    // 无 secondGrow 形参槽写）；且 `movl $0x0,-0xc`（points）在**函数头**（P212 全
    // 头部声明）。旧码默认 ctor + 逐字段赋值 ⇒ p167 MISSING 三参 ctor/EXTRA 默认 ctor。
    int points = 0;
    if (grade < m_field4c)
        grade = m_field4c;

    std::map<int, std::map<int, int> >::const_iterator gradeIt = m_map18.find(grade);
    if (gradeIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            gradeIt->second.find(job * 5 + firstGrow);
        if (slotIt != gradeIt->second.end())
            points += slotIt->second;
    }

    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow), 0);
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
        m_map30.find(key);
    if (costIt != m_map30.end())
    {
        if (flag)
        {
            if (points != 0)
                points -= costIt->second.m_pointCost;
        }
        else if (points == 0)
        {
            points = costIt->second.m_pointCost;
        }
    }
    return points;
}

bool PvPSkillTreeParameterScript::getGiveSkill(
    int job, int firstGrow, int secondGrow,
    std::vector<std::pair<int, int> >& out) const
{
    // ORIG 0x08a5e178：m_map30[(job,firstGrow)] 命中时 out = value.m_giveSkills(+0x34)，返回 true
    // ORIG 0x8a5e178 首条 `movl $0x0,0x14(%ebp)` + 回读 `movzbl` ⇒ 形参槽清零后
    // 参与 key 构造（非常量内联）。
    secondGrow = 0;
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator it =
        m_map30.find(key);
    if (it != m_map30.end())
    {
        out = it->second.m_giveSkills;
        return true;
    }
    return false;
}

// 全局脚本重复加载开关（ORIG 0x0949b284 B，char）
char g_isAllowScriptDuplication = 0;

// ============================================================================
// PvPSkillTreeParameterScript::getPvPSkillTree —— ORIG 0x08a5db5a（0x119 字节）
// 报告：docs/class_func_reports/PvPSkillTreeParameterScript/getPvPSkillTree.md
// key=(uchar job, uchar firstGrow, uchar secondGrow) 查 m_map0；命中后内层按
// grade 查找；命中时 *out = flag ? &value.m_vec0c(+0x0c) : &value.m_vec00(+0x00)
// 并返回 true，否则返回 false。
// ============================================================================
bool PvPSkillTreeParameterScript::getPvPSkillTree(
    int job, int firstGrow, int secondGrow, int grade, bool flag,
    std::vector<std::pair<int, int> >** out) const
{
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));

    std::map<PvPSkillTreeKey,
             std::map<int, PvPSkillTreeValue> >::const_iterator treeIt =
        m_map0.find(key);
    if (treeIt != m_map0.end())
    {
        std::map<int, PvPSkillTreeValue>::const_iterator gradeIt =
            treeIt->second.find(grade);
        if (gradeIt != treeIt->second.end())
        {
            if (flag)
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec0c);
            else
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec00);
            return true;
        }
    }
    return false;
}
