void HeroMissionEvent::setMissionValue(CUser* user,
                                       HeroMissionCondition::MissionNo::T mission,
                                       unsigned int value)
{
    // ORIG 0x8167226：`mov (%eax);add $0x34;call *%edx` = 虚调用 IsEventing(NULL)。
    // 继承 CEventBase 后以真实虚调用复现（§4.5 正解），非直接读 m_isEventing。
    if (!IsEventing(0))
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__,
                              0x2a6, "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* out = 0;   // ORIG -0xc(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        // ORIG mov %dx,0x8(%eax)：HeroMissionValue +0x08 写 ushort value
        out->m_field8 = (unsigned short)value;
    }
    sendMissionData(user);
