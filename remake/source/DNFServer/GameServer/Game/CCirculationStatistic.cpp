#include "CCirculationStatistic.h"
#include "CUser.h"
#include <string.h>

void CCirculationStatistic::stCirculationStatistic::reset()
{
    memset(m_values, 0, sizeof(m_values));
}

CCirculationStatistic* GetInstanceCirculationStatistic()
{
    static CCirculationStatistic instance;
    return &instance;
}

bool CCirculationStatistic::AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user,
                                                       unsigned int value)
{
    if (!user || field < 0 || field >= 0x30)
        return false;
    const int level = user->get_charac_level();
    if (level <= 0 || level >= 0x47)
        return false;
    stCirculationStatistic& stat = m_data[user->GetServerGroup()];
    stat.m_values[level][field] += value;
    return true;
}
