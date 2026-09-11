// ============================================================================
// resetMissionForGm —— ORIG T 0x08167a3a（resetMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，调
// HeroMissionValue::reset()（+0x04..+0x0a 清零）。
// ============================================================================
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
}