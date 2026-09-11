// ORIG 0x85e26a0：else 分支 operator[](0)（m_kindMissionList[0]，非静态空表）。
std::list<CMission*>& PvP_MissionSystem::get_kind_mission_list(unsigned int idx)
{
    // [b28c-L18] ORIG 尾形 `call size; cmp idx,%eax; seta %al; test %al,%al; je`
    // = 四段物化（P225 while+break 形：值无槽往返 ⇒ 禁具名 bool，P0）。三目直返
    // 在 4.4 恒折 `jbe` 直跳少 3 条；具名 bool 形恒槽化（-0x9 byte 槽实证）。
    while (m_kindMissionList.size() > idx)
    {
        return m_kindMissionList[idx];
        break;
    }
    return m_kindMissionList[0];
}