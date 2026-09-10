bool UseFatigueMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    // [b28c-27] N（P168a X1 边界）：ORIG 首判 `call; xor $1,%eax; test %al; je` 与
    // 尾段 `call; test %al; je` 均来自 char 返回 callee；bool 化 isRepeatMission 会
    // 使 callee 自身 movzbl 直返劣化为 setne（ORIG 0x08164fc2 仅 movzbl+ret），
    // 调用点 X1（uchar ^=1）又被 4.4 因"判断后存在依赖同一条件的 bool 物化岛"
    // jump-thread 折叠（同文件 L462-466 已实证 11→13 回退）。保持 char + 短路原形。
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xe5,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xec,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
