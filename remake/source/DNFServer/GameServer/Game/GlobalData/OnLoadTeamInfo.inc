// ORIG T 0x08588be4（OnLoadTeamInfo.md 全量转写）：data 首 4 字节为队伍 key，
// 命中 m_teamMap 时委托 loadDataPreliminaryTeamMatchList(data)。
void online_preliminary::COnlinePreliminaryTeamMgr::OnLoadTeamInfo(
    CUser* user, char* data)
{
    // data = 队伍线格式缓冲（首 int 为 team key，ORIG 08588bed 直读 (%eax)）；
    // 该域包 struct 权威头未建模（布局未建模），P156 豁免保留裸读。
    // ORIG 0x08588bea：data 先落局部槽 -0xc(%ebp) 再回读传 find（4.1 实参
    // 地址表达式 spill 形）——具名局部拷贝复现。
    char* pTeam = data;
    std::map<int, COnlinePreliminaryTeam*>::iterator it =
        m_teamMap.find(*(int*)pTeam);
    if (it != m_teamMap.end())
    {
        it->second->loadDataPreliminaryTeamMatchList(data);
    }
}