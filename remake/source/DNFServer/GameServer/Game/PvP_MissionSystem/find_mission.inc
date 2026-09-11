CMission* PvP_MissionSystem::find_mission(int idx)
{
    std::map<int, CMission*>::iterator it = m_missionMap.find(idx);
    // [b28c-L18] ORIG 0x085e26xx 尾段实证：调用符号为 iterator::operator!=（this=it、
    // 实参 end），true→hit fall-through、`mov $0` 尾岛 ⇒ 原码 if(it!=end) return
    // it->second; return 0;（三目形在 4.4 产反序岛，P222 正向包裹）。
    if (it != m_missionMap.end())
        return it->second;
    return 0;
}