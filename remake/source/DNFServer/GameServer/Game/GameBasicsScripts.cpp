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

bool CActionPointEtcParameter::IsValidTodayRewardItem(
    _SIG_LOAD_ACTION_POINT::_TodayRewardItem item)
{
    int value = static_cast<int>(item);
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
