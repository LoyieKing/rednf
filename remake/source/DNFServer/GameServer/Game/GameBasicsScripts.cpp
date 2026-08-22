#include <cstring>
#include "CDataManager.h"
#include "GamePODTypes.h"

namespace APSystem
{
CActionPointEtcParameter::CActionPointEtcParameter()
{
    ClearData();
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
    int value = item.m_itemIdx;  // ORIG 0x08894976：param<0 || param>3 → false
    return value >= 0 && value <= 3;
}
}


void stAvatarEmblemInfo_t::init()
{
    memset(this, 0, sizeof(*this));
}

void stAvatarExpansionInfo_t::init()
{
    m_color[0] = 0;
    m_color[1] = 0;
}


// ============================================================================
// PvPSkillTreeParameterScript 查询（ORIG 0x08a5e178 / 0x08a5dd62, 符号 T）
// 最小实现保证符号 T + 可链接；完整脚本解析逻辑待细化。
// ============================================================================
int PvPSkillTreeParameterScript::getPvPSkillPoint(
    int job, int firstGrow, int secondGrow, int grade, bool flag) const
{
    (void)job;
    (void)firstGrow;
    (void)secondGrow;
    (void)grade;
    (void)flag;
    return 0;
}

void PvPSkillTreeParameterScript::getGiveSkill(
    int job, int firstGrow, int secondGrow,
    std::vector<std::pair<int, int> >& out) const
{
    (void)job;
    (void)firstGrow;
    (void)secondGrow;
    out.clear();
}

// 全局脚本重复加载开关（ORIG 0x0949b284 B，char）
char g_isAllowScriptDuplication = 0;
