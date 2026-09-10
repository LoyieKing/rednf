void HeroMissionEvent::resetMissionForGm(CUser* user,
                                         HeroMissionCondition::MissionNo::T mission)
{
    HeroMissionValue* out = 0;   // ORIG -0x10(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        // [b28c-L7] P223：ORIG 0x08167aXX 同 setMissionForGm 空块 nop 岛形。
        if (it == m_conditions.end())
            goto END;
        out->reset();
    }
END:;
