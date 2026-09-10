void HeroMissionEvent::processMission(CUser* user,
                                      HeroMissionCondition::MissionNo::T mission,
                                      unsigned int param)
{
    // ORIG 0x081672dd `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (!IsEventing(0))
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__, 0x2b8,
                              "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* value = 0;
    // [b28d-solo-HeroMissionEvent] ORIG 0x081672d4 `call; test %al,%al; je tail`：
    // bool callee 的正向直判（2 条）；4.4 对 `!f()`/`f()==0` 恒补 xor $1（3 条）
    // ⇒ 改正向包裹（P222：否定早返 → 正向 if 包裹）。
    if (findMissionValue(user, mission, value))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        if (it == m_conditions.end())
        {
            LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                                  __PRETTY_FUNCTION__, 0x2c2,
                                  "[HeroMission] Invalid mission. (charac:%u, mission:%d)",
                                  user->get_charac_no(-1),
                                  static_cast<int>(mission));
            user->DisConnSig(static_cast<DISCONN_SIG>(0x3e), 1, 0x2c3);
            return;
        }

        unsigned short prev = value->m_fieldA;
        it->second->processMission(user, *value, param);   // 条件虚表 +0x08
        // [b28d-solo-HeroMissionEvent] ORIG 0x08167470 `cmp -0xa(%ebp),%ax; jbe`
        // ⇒ 源码为 `> prev` 而非 `!= 0`。
        if (mission == 4 && prev == 0 && value->m_fieldA > prev)
        {
            processMission(user,
                           static_cast<HeroMissionCondition::MissionNo::T>(6), 1);
        }
    }
