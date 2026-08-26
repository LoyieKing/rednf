// OnlinePreliminaryTeam.cpp — online_preliminary::COnlinePreliminaryTeam
// 访问器迁移实现（原 GameStubs.cpp 定义）。
#include "CUserCharacInfo.h"

namespace online_preliminary
{

int COnlinePreliminaryTeam::GetTeamId() const
{
    return m_field00;
}

const char* COnlinePreliminaryTeam::GetTeamName() const
{
    return m_field14;
}

// ORIG T 0x0858831a：data 布局 int matchCount@+4，其后每 8 字节一条
// （key@0 / value@4）；逐条写入 m_playCountMap[key] = value。
int COnlinePreliminaryTeam::loadDataPreliminaryTeamMatchList(char* data)
{
    int count = *(int*)(data + 4);
    for (int i = 0; i < count; ++i)
    {
        int key = *(int*)(data + 8 + i * 8);
        int value = *(int*)(data + 8 + i * 8 + 4);
        m_playCountMap[key].m_count = static_cast<unsigned int>(value);
    }
    return 1;
}

}  // namespace online_preliminary
